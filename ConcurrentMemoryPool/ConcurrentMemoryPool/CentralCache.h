#pragma once
#include "Common.h"
#include "PageCache.h"
#include <malloc.h>
#include <mutex>
//因为在整个进程中只能有一个CentralCache对象
//所以将CentralCache设计为单例类
class CentralCache
{
public:
	static inline CentralCache* GetInstance()
	{
		return &_Inst;
	}

	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t byte);
	void ReturnToCentralCache(void* start, size_t byte);
private:
	Span* GetOneSpan(SpanList& spanlist, size_t byte);

private:
	CentralCache() = default;
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) = delete;
private:
	SpanList _spanlist[NLISTS];

	std::mutex _mtx;
	static CentralCache _Inst;
};
