#include "CentralCache.h"

CentralCache CentralCache::_Inst;

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t byte) {
	assert(byte <= MAXBYTES);

	std::unique_lock<std::mutex> _lock(_mtx);
	size_t index = ClassSize::Index(byte);
	SpanList& spanlist = _spanlist[index];
	size_t fetchnum = 0;
	Span* span = GetOneSpan(spanlist, byte);

	void* prev = nullptr;
	void* cur = span->_objlist;


	while (cur != nullptr && fetchnum < num) {
		prev = cur;
		cur = NEXT_OBJ(cur);
		++fetchnum;
	}
	start = span->_objlist;
	
	NEXT_OBJ(prev) = nullptr;
	end = prev;

	span->_usecount += fetchnum;
	span->_objlist = (void*)((char*)prev + byte);
	return fetchnum;
}

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte) {
	assert(byte <= MAXBYTES);

	if(!spanlist.Empty()) {
		Span* cur = spanlist.Begin();
		while (cur != spanlist.End()) {
			if (cur->_objlist != nullptr) {
				return cur;
			}
			cur = cur->_next;
		}
	}

	size_t npage = ClassSize::NumMovePage(byte);
	Span* newspan = PageCache::GetInstance()->NewSpan(npage);

	char* start = (char*)(newspan->_pageid * 4 * 1024);
	char* end = (start + (newspan->_npage) * 4 * 1024);
	char* cur = start;
	char* next = start + byte;
	size_t count = 0;
	while (next < end) {
		NEXT_OBJ(cur) = next;
		cur = next;
		next += byte;
		count++;
	}
	NEXT_OBJ(cur) = nullptr;
	newspan->_objlist = start;
	newspan->_usecount = 0;
	newspan->_objsize = byte;

	spanlist.PushFront(newspan);
	return newspan;
}

void CentralCache::ReturnToCentralCache(void* start) {
	std::unique_lock<std::mutex> _lock(_mtx);
	while (start)
	{
		void* next = NEXT_OBJ(start);	//next������һ���ڴ��

		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);	//���ݵ�ַ�õ��ڴ�����ڵ�span
		NEXT_OBJ(start) = span->_objlist;	//���Ӧ��span->_objlist����
		span->_objlist = start;	

		span->_usecount--;

		if (span->_usecount == 0)
		{
			//˵������ҳspanȫ�����������������ø�span������PageCache
			SpanList& spanlist = _spanlist[span->_objsize];
			spanlist.Earse(span);

			span->_objlist = nullptr;
			span->_objsize = 0;
			span->_next = nullptr;
			span->_prev = nullptr;

			PageCache::GetInstance()->TakeSpanToPageCache(span);
		}
		start = next;
	}
}

