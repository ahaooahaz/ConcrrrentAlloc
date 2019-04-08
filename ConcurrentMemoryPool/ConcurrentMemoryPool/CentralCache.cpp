#include "CentralCache.h"

//创建出单例对象
CentralCache CentralCache::_Inst;

//向threadcache返回10个对应大小的内存块
size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t byte/*想要得到内存块的大小*/)	
{
	assert(byte <= MAXBYTES);

	std::unique_lock<std::mutex> _lock(_mtx);
	size_t index = ClassSize::Index(byte);	//此处对齐是不必要的，为了万无一失，添加在此
	SpanList& spanlist = _spanlist[index];		//拿到应该分配内存块的spanlist
	size_t fetchnum = 0;	//fetchnum表示返回的内存块的大小
	Span* span = GetOneSpan(spanlist, byte);		//获得一块对应的span，有可能来自centralcache/pagecache

	void* prev = nullptr;
	void* cur = span->_objlist;

	//此处为了找到end应该标记的位置：最后一块内存的地址
	while (cur != nullptr && fetchnum < num)
	{
		prev = cur;	//跳出循环时，prev记录最后一块内存的地址
		cur = NEXT_OBJ(cur);
		++fetchnum;
	}
	start = span->_objlist;
	
	NEXT_OBJ(prev) = nullptr;
	end = prev;

	span->_usecount += fetchnum;
	span->_objlist = (void*)((char*)prev + byte);	//将剩余的内存块继续链接在centralcache中
	return fetchnum;
}

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte)
{
	assert(byte <= MAXBYTES);
	if(!spanlist.Empty())
	{
		//当前spanlist不为空
		//寻找到当前spanlist上还有空闲obj的span并返回
		Span* cur = spanlist.Begin();
		while (cur != spanlist.End())
		{
			if (cur->_objlist != nullptr)
			{
				return cur;
			}
			cur = cur->_next;
		}
	}

	//当对应的spanlist不存在空闲的span时，就需要向PageCache申请新的spanlist
	size_t npage = ClassSize::NumMovePage(byte);	//计算需要几页的大小
	Span* newspan = PageCache::GetInstance()->NewSpan(npage);	//从PageCache获得新的Span

	//将分配好的Span进行链接  此处span->_objlist还未进行设置  根据页号计算出newspan的地址
	char* start = (char*)(newspan->_pageid * 4 * 1024);
	char* end = (start + (newspan->_npage) * 4 * 1024);
	char* cur = start;
	char* next = start + byte;
	size_t count = 0;
	while (next < end)
	{
		NEXT_OBJ(cur) = next;
		cur = next;
		next += byte;
		count++;
	}
	NEXT_OBJ(cur) = nullptr;		//会将span->_objlist上的最后一块内存设置为nullptr，作为span是否空闲的标志
	newspan->_objlist = start;
	newspan->_usecount = 0;
	newspan->_objsize = byte;

	//将新的spanlist挂载在CentralCache中
	spanlist.PushFront(newspan);
	
	return newspan;
}

void CentralCache::ReturnToCentralCache(void* start)
{
	std::unique_lock<std::mutex> _lock(_mtx);
	while (start)
	{
		void* next = NEXT_OBJ(start);	//next保存下一个内存块

		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);	//根据地址拿到内存块所在的span
		NEXT_OBJ(start) = span->_objlist;	//与对应的span->_objlist连接
		span->_objlist = start;	

		span->_usecount--;

		if (span->_usecount == 0)
		{
			//说明整个页span全部被返还，即可以让改span返还给PageCache
			SpanList& spanlist = _spanlist[span->_objsize];
			spanlist.Earse(span);

			span->_objlist = nullptr;
			span->_objsize = 0;
			span->_next = nullptr;
			span->_prev = nullptr;

			PageCache::GetInstance()->TakeSpanToPageCache(span);
			cout << "返还给PageCache" << endl;
		}
		start = next;
	}
}

