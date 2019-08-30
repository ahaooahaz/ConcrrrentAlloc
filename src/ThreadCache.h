#pragma once
#include "Common.h"
#include "CentralCache.h"

//ThreadCache是一个线程独有的资源，直接与本线程交互
class ThreadCache
{
public:
	//ThreadCache提供两个接口，一个为申请内存，另一个为释放内存
	void* Allocate(size_t size);
	void Deallocate(void* ptr);

	void ReturnToCentralCache(FreeList &freelist);

private:
	//向CentralCache申请内存块的接口,返回一块内存块，将剩下的挂载在ThreadCache的对应处
	void* FetchFromCentralCache(size_t index, size_t byte);
private:
	FreeList _freelist[NLISTS];
};