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

	size_t index = ClassSize::Index(byte);
	SpanList& spanlist = _spanlist[index];
	size_t fetchnum = 0;
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
	
	return newspan;
}

