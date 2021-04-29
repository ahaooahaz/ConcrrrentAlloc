#include "PageCache.h"

PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage) {
	std::unique_lock<std::mutex> _lock(_mtx);
	return _NewSpan(npage);
}

Span* PageCache::_NewSpan(size_t npage) {
	assert(npage < NPAGES);

	if (!_pagelist[npage].Empty()) {
		return _pagelist[npage].Pop();
	}

	for (size_t i = npage + 1; i < NPAGES; ++i) {
		Span* split = nullptr;
		SpanList& pagelist = _pagelist[i];
		if (!pagelist.Empty()) {
			Span* span = pagelist.Pop();

			split = new Span;
			split->_pageid = span->_pageid + span->_npage - npage;
			split->_npage = npage;

			span->_npage -= npage;

			_pagelist[span->_npage].PushFront(span);

			for (size_t i = 0; i < split->_npage; ++i) {
				_id_span_map[split->_pageid + i] = split;
			}
			return split;
		}
	}

	//void* ptr = VirtualAlloc(NULL, (NPAGES - 1) * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);	//ֱ����ϵͳ����128ҳ�Ĵ�С
    void* ptr = mmap(NULL, (NPAGES - 1) * 4 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | 0x40000 /*MAP_HUGETLB*/, -1, 0);
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}

	Span* maxspan = new Span;
	maxspan->_objlist = ptr;
	maxspan->_pageid = (PageID)ptr >> 12;
	maxspan->_npage = NPAGES - 1;
	_pagelist[NPAGES - 1].PushFront(maxspan);
	return _NewSpan(npage);
}

Span* PageCache::MapObjectToSpan(void* obj) {
	PageID pageid = (PageID)obj >> 12;
	auto it = _id_span_map.find(pageid);

	if(it == _id_span_map.end()) {
		assert(false);
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

	auto nextit = _id_span_map.find(span->_pageid + span->_npage);
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

	_pagelist[span->_npage].PushFront(span);

}
