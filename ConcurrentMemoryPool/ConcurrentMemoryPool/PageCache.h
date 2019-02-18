#pragma once
#include "Common.h"
#include <windows.h>
#include <map>
//PageCache类为单例类
class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_Inst;
	}

	Span* NewSpan(size_t npage);
private:
	PageCache() = default;
	PageCache(const PageCache&) = delete;
	PageCache& operator=(const PageCache&) = delete;

private:
	SpanList _pagelist[NPAGES];
	std::map<PageID, Span*> _id_span_map;

	static PageCache _Inst;
};
