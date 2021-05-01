#pragma once
#include "Common.h"
#include <map>
#include <sys/mman.h>
#include <unordered_map>
#include <mutex>
#include <unistd.h>

class PageCache {
public:
	static PageCache* GetInstance() {
		return &_Inst;
	}

	/**
	 * @brief 获取以页为单位的连续内存
	 * @param [in] npage n页大小内存
	 * @return 失败返回空
	 */
	Span* NewSpan(size_t npage);

	/**
	 * @brief 获取ptr位置对应的Span
	 * @param [in] ptr 指针
	 * @return 失败返回空
	 */
	Span* MapObjectToSpan(void* ptr);

	/**
	 * @brief 归还使用完的Span
	 * @param [in] span 要归还的span
	 */
	void TakeSpanToPageCache(Span* span);

private:
	Span* _NewSpan(size_t npage);
	PageCache() = default;
	PageCache(const PageCache&) = delete;
	PageCache& operator=(const PageCache&) = delete;
private:
	SpanList _pagelist[NPAGES + 1];
	std::map<PageID, Span*> _id_span_map;
	std::mutex _mtx;

	static PageCache _Inst;
};
