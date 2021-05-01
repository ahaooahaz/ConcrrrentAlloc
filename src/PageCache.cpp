#include "PageCache.h"

PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage) {
	std::unique_lock<std::mutex> _lock(_mtx);
	return _NewSpan(npage);
}

Span* PageCache::_NewSpan(size_t npage) {
	assert(npage <= NPAGES && npage != 0);

	if (!_pagelist[npage].Empty()) {
		return _pagelist[npage].Pop();
	}

	if (npage != NPAGES) {
		for (size_t i = npage + 1; i <= NPAGES; ++i) {
			Span* split = nullptr;
			SpanList& pagelist = _pagelist[i];
			if (!pagelist.Empty()) {
				Span* span = pagelist.Pop();

				split = new Span;
				split->_pageid = span->_pageid;
				split->_npage = npage;
				split->_objlist = span->_objlist;

				span->_pageid += npage;
				span->_npage -= npage;
				span->_objlist = (void*)((char*)span->_objlist + getpagesize() * npage);

				_pagelist[span->_npage].InsertFront(span);

				for (size_t i = 0; i < split->_npage; ++i) {
					_id_span_map[split->_pageid + i] = split;
				}
				return split;
			}
		}
	}
	/**
	 * @note mmap windows平台用于申请大块连续内存
	 */
	// void* ptr = VirtualAlloc(NULL, (NPAGES - 1) * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	/**
	 * @note mmap linux系统调用，用于申请大块连续内存
	 */
    void* ptr = mmap(NULL, getpagesize() * NPAGES, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (ptr == MAP_FAILED) {
		throw std::bad_alloc();
	}

	Span* maxspan = new Span;
	maxspan->_objlist = ptr;
	maxspan->_pageid = (PageID)ptr >> 12;
	maxspan->_npage = NPAGES;
	_pagelist[NPAGES].InsertFront(maxspan);
	return _NewSpan(npage);
}

Span* PageCache::MapObjectToSpan(void* obj) {
	PageID pageid = (PageID)obj >> 12;
	auto it = _id_span_map.find(pageid);

	if(it == _id_span_map.end()) {
		return nullptr;
	}
	return it->second;
}

void PageCache::TakeSpanToPageCache(Span* span) {
	assert(span != nullptr);
	std::unique_lock<std::mutex> lock(_mtx);

	auto previt = _id_span_map.find(span->_pageid - 1);
	while (previt != _id_span_map.end()) {
		Span* prevspan = previt->second;
		if (prevspan->_usecount != 0) {
			break;
		}

		if ((prevspan->_npage + span->_npage) > NPAGES) {
			break;
		}

		_pagelist[prevspan->_npage].Earse(prevspan);
		prevspan->_npage += span->_npage;
		delete span;

		span = prevspan;

		_id_span_map.erase(previt);
		previt = _id_span_map.find(span->_pageid - 1);
	}

	auto nextit = _id_span_map.find(span->_pageid + span->_npage + 1);
	while (nextit != _id_span_map.end()) {
		Span* nextspan = nextit->second;
		if ((span->_npage + nextspan->_npage) > NPAGES) {
			break;
		}

		if (nextspan->_usecount != 0) {
			break;
		}

		_pagelist[nextspan->_npage].Earse(nextspan);
		span->_npage += nextspan->_npage;
		delete nextspan;

		nextit = _id_span_map.find(span->_pageid + span->_npage);
	}

	for (size_t i = 0; i < span->_npage; ++i) {
		_id_span_map[span->_pageid + i] = span;
	}

	_pagelist[span->_npage].InsertFront(span);

}
