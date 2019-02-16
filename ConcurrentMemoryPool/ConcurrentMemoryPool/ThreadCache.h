#pragma once
#include "Common.h"

//ThreadCache是一个线程独有的资源，直接与本线程交互
class ThreadCache
{
public:
	//ThreadCache提供两个接口，一个为申请内存，另一个为释放内存
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

private:
	FreeList _freelist[NLISTS];
};