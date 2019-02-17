#pragma once
#include "Common.h"
#include <windows.h>
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

	static PageCache _Inst;
};
