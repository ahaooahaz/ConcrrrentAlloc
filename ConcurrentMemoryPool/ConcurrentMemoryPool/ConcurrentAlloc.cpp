#include "ConcurrentAlloc.h"

_declspec(thread) ThreadCache* _thr_threadCache = nullptr;

void* ConcurrentAlloc(size_t size)
{
	if (size > MAXBYTES)
	{
		//当申请的内存大于MAXBYTES时，就直接去pageCache分配内存
		size_t npage = ClassSize::NumMovePage(size);
		return PageCache::GetInstance()->NewSpan(npage);
	}
	
	if (_thr_threadCache != nullptr)
		return _thr_threadCache->Allocate(size);
	_thr_threadCache = new ThreadCache;
	return ConcurrentAlloc(size);
}

void ConcurrentFree(void* ptr, size_t size)
{
	if (_thr_threadCache != nullptr)
	{
		_thr_threadCache->Deallocate(ptr, size);
		return;
	}
	throw Unknown;	//当释放内存时不存在threadcache时就抛出未知异常

}

