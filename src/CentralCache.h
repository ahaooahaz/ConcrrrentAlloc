#pragma once
#include <malloc.h>
#include <mutex>

#include "Common.hpp"
#include "PageCache.h"


class CentralCache {
public:
	static inline CentralCache* GetInstance() {
		return &_Inst;
	}

	/**
	 * @brief threadCache 获取内存块的接口
	 * @param [out] start 可用内存的开始位置
	 * @param [out] end 可用内存的结束位置
	 * @param [in] num 期望获取到的内存块个数
	 * @param [in] byte 内存块的规格
	 * @return 实际返回的内存块个数
	 */
	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t byte);

	/**
	 * @brief 返回使用完的内存块
	 * @param start 开始位置
	 */
	void ReturnToCentralCache(void* start);
private:
	Span* GetOneSpan(SpanList& spanlist, size_t byte);

private:
	CentralCache() = default;
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) = delete;
private:
	SpanList _spanlist[NLISTS + 1];

	std::mutex _mtx;
	static CentralCache _Inst;
};
