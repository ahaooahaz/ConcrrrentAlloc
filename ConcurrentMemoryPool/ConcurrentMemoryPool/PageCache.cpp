#include "PageCache.h"


//创建单例对象
PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage)
{
	//解决对递归函数加锁的方式
	std::unique_lock<std::mutex> _lock(_mtx);
	return _NewSpan(npage);
}

Span* PageCache::_NewSpan(size_t npage)
{
	assert(npage < NPAGES);

	//首先在对应的PageList上查看有没有空闲的Span
	if (!_pagelist[npage].Empty())
	{
		//目标pagelist不为空
		return _pagelist[npage].Pop();
	}

	//到这里即说明目标pagelist为空，需要向下寻找，进行分割，从下一个位置开始寻找
	for (size_t i = npage + 1; i < NPAGES; ++i)
	{
		Span* split = nullptr;
		SpanList& pagelist = _pagelist[i];
		if (!pagelist.Empty())
		{
			//找到的pagelist不为空
			Span* span = pagelist.Pop();

			split = new Span;
			split->_pageid = span->_pageid + span->_npage - npage;	//从后面切内存	此处存在bug		如果剩余span不足应对情况
			split->_npage = npage;

			//计算剩余的内存
			span->_npage -= npage;

			//将剩余的大块内存挂载pageCache的相应位置下
			_pagelist[span->_npage].PushFront(span);

			//只有正在使用的页才需要建立映射
			for (size_t i = 0; i < split->_npage; ++i)
			{
				//建立页的映射关系，xxx页-xxx页都为split管理
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
	return _NewSpan(npage);
}

Span* PageCache::MapObjectToSpan(void* obj)
{ 
	PageID pageid = (PageID)obj >> 12;	//计算指针所在的页
	auto it = _id_span_map.find(pageid);	//查找该页对应的管理页

	if(it == _id_span_map.end())
	{
		//不应该进来这里
		assert(false);
	}
	return it->second;
}

void PageCache::TakeSpanToPageCache(Span* span)
{
	assert(span != nullptr);
	std::unique_lock<std::mutex> lock(_mtx);

	auto previt = _id_span_map.find(span->_pageid - 1);
	while (previt != _id_span_map.end())
	{
		//查看前一个Span
		Span* prevspan = previt->second;
		if (prevspan->_usecount != 0)
		{
			//前一个span正在使用中，则不能与前一个合并
			break;
		}

		//走到这里说明可以与前一个span进行合并
		if ((prevspan->_npage + span->_npage) > NPAGES)
		{
			//说明与前一个span合并之后的页大小超过128
			break;
		}

		_pagelist[prevspan->_npage].Earse(prevspan);
		prevspan->_npage += span->_npage;
		delete span;

		span = prevspan;

		//继续向前合并
		_id_span_map.erase(previt);
		previt = _id_span_map.find(span->_pageid - 1);

	}

	//向后合并
	auto nextit = _id_span_map.find(span->_pageid + span->_npage);
	while (nextit != _id_span_map.end())
	{
		Span* nextspan = nextit->second;
		if ((span->_npage + nextspan->_npage) > NPAGES)
		{
			//超过128不进行合并
			break;
		}

		if (nextspan->_usecount != 0)
		{
			//前一个span不空闲不合并
			break;
		}

		_pagelist[nextspan->_npage].Earse(nextspan);
		span->_npage += nextspan->_npage;
		delete nextspan;

		nextit = _id_span_map.find(span->_pageid + span->_npage);
	}

	//合并完成，重新挂载，并重新建立映射
	for (size_t i = 0; i < span->_npage; ++i)
	{
		_id_span_map[span->_pageid + i] = span;
	}

	_pagelist[span->_npage].PushFront(span);

}
