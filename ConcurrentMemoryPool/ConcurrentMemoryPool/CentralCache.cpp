#include "CentralCache.h"

//创建出单例对象
CentralCache CentralCache::_Inst;

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t byte)
{
	//测试代码
	/*start = malloc(byte * num);
	end = (char*)start + byte * (num - 1);

	void* cur = start;
	while (cur <= end)
	{
		void* next = (char*)cur + byte;
		NEXT_OBJ(cur) = next;

		cur = next;
	}

	NEXT_OBJ(end) = nullptr;

	return num;*/


	assert(byte <= MAXBYTES);

	std::unique_lock<std::mutex> _lock(_mtx);
	size_t index = ClassSize::Index(byte);
	SpanList& spanlist = _spanlist[index];
	size_t fetchnum = 1;
	Span* span = GetOneSpan(spanlist, byte);

	void* prev = nullptr;
	void* cur = span->_objlist;
	while (cur != nullptr && fetchnum < num)
	{
		prev = cur;
		cur = NEXT_OBJ(cur);
		++fetchnum;
	}
	start = span->_objlist;
	end = cur;
	NEXT_OBJ(end) = nullptr;

	span->_usecount += fetchnum;
	return fetchnum;
}

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte)
{
	assert(byte <= MAXBYTES);
	if(!spanlist.Empty())
	{
		//当前spanlist不为空
		return spanlist.Pop();
	}

	//当对应的spanlist为空时，就需要向PageCache申请新的spanlist
	size_t npage = ClassSize::NumMovePage(byte);	//计算需要几页的大小
	Span* newspan = PageCache::GetInstance()->NewSpan(npage);	//从PageCache获得新的Span

	//将分配好的Span进行链接  此处span->_objlist还未进行设置  根据页号计算出newspan的地址
	char* start = (char*)(newspan->_pageid * 4 * 1024);
	char* end = (start + (newspan->_npage) * 4 * 1024);
	char* cur = start;
	char* next = start + byte;
	while (next < end)
	{
		NEXT_OBJ(cur) = next;
		cur = next;
		next += byte;
	}
	NEXT_OBJ(cur) = nullptr;
	newspan->_objlist = start;
	newspan->_usecount = 0;
	newspan->_objsize = byte;

	//将新的spanlist挂载在CentralCache中
	spanlist.PushFront(newspan);
	
	return newspan;
}

void CentralCache::ReturnToCentralCache(void* start)
{
	while (start)
	{
		void* next = NEXT_OBJ(start);

		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);	//根据地址拿到内存所在的页
		NEXT_OBJ(start) = span->_objlist;
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

