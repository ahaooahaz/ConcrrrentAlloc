#include "ThreadCache.h"

__thread ThreadCache ThreadCache::_Inst;

void* ThreadCache::Allocate(size_t size) {
	assert(size <= MAXBYTES);

	size = ClassSize::Roundup(size);

	size_t index = ClassSize::Index(size);
	FreeList& freelist = _freelist[index];
	if (!freelist.Empty()) {
		return freelist.Pop();
	}


	return FetchFromCentralCache(index, size);
}

void* ThreadCache::FetchFromCentralCache(size_t index, size_t byte) {
	assert(byte <= MAXBYTES);

	FreeList& freelist = _freelist[index];
	size_t num = 10;

	void *start, *end;
	size_t fetchnum = CentralCache::GetInstance()->FetchRangeObj(start, end, num, byte);
	if (fetchnum == 1) {
		return start;
	}

	freelist.PushRange(NEXT_OBJ(start), end, fetchnum - 1);
	return start;
}

void ThreadCache::Deallocate(void* ptr) {
	Span* span = PageCache::GetInstance()->MapObjectToSpan(ptr);

	assert(span);

	if (span->_objsize >= MAXBYTES) {
		PageCache::GetInstance()->TakeSpanToPageCache(span);
		return;
	}
	FreeList& freelist = _freelist[ClassSize::Index(span->_objsize)];

	freelist.Push(ptr);

	if (freelist.Size() >= freelist.MaxSize()) {
		ReturnToCentralCache(freelist);
	}
}

void ThreadCache::ReturnToCentralCache(FreeList &freelist) {
	void* start = freelist.Clear();
	CentralCache::GetInstance()->ReturnToCentralCache(start);
}