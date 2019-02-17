#include "PageCache.h"


//创建单例对象
PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage)
{
	assert(npage < NPAGES);
	Span* newspan = new Span;
	void* ptr = VirtualAlloc(NULL, npage * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	newspan->_objlist = ptr;
	newspan->_pageid = (PageID)ptr >> 12;
	newspan->_npage = npage;
	return newspan;
}