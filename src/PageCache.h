#pragma once
#include "Common.h"
#include <map>
#include <sys/mman.h>
#include <unordered_map>
#include <mutex>

/* PageCache
 * 与系统物理内存交互，一次申请128页连续内存
 */
class PageCache
{
public:
	static PageCache* GetInstance()
	{
		return &_Inst;
	}

	Span* NewSpan(size_t npage);
	Span* MapObjectToSpan(void* ptr);
	void TakeSpanToPageCache(Span* span);
private:
	Span* _NewSpan(size_t npage);
	PageCache() = default;
	PageCache(const PageCache&) = delete;
	PageCache& operator=(const PageCache&) = delete;
private:
	SpanList _pagelist[NPAGES];
	std::map<PageID, Span*> _id_span_map;
	std::mutex _mtx;

	static PageCache _Inst;
};
