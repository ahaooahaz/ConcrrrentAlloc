#include "PageCache.h"


//创建单例对象
PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage)
{
	assert(npage < NPAGES);

	//首先在对应的PageList上查看有没有空闲的Span
	if (!_pagelist[npage].Empty())
	{
		//目标pagelist不为空
		return _pagelist[npage].Pop();
	}

	//到这里即说明目标pagelist为空，需要向下寻找，进行分割
	for (size_t i = npage + 1; i < NPAGES; ++i)
	{
		SpanList& pagelist = _pagelist[i];
		if (!pagelist.Empty())
		{
			//找到的pagelist不为空
			Span* span = pagelist.Pop();
			Span* split = new Span;
			split->_pageid = span->_pageid + span->_npage - npage;
			split->_npage = npage;
			span->_npage -= npage;
			_pagelist[span->_npage].PushFront(span);

			for (size_t i = 0; i < split->_npage; ++i)
			{
				//建立页的映射关系
				_id_span_map[split->_pageid + i] = split;
			}
			return split;
		}
	}

	//走到这里说明下面没用可以用来分割的Pagelist，需要重新向内存申请
	void* ptr = VirtualAlloc(NULL, (NPAGES - 1) * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);	//直接向系统申请128页的大小
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	//将新申请的内存挂载在最下面
	Span* maxspan = new Span;
	maxspan->_objlist = ptr;
	maxspan->_pageid = (PageID)ptr >> 12;
	maxspan->_npage = NPAGES - 1;
	_pagelist[NPAGES - 1].PushFront(maxspan);
	return NewSpan(npage);
}
	
	/*Span* newspan = new Span;
	void* ptr = VirtualAlloc(NULL, npage * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	newspan->_objlist = ptr;
	newspan->_pageid = (PageID)ptr >> 12;
	newspan->_npage = npage;
	return newspan;*/
