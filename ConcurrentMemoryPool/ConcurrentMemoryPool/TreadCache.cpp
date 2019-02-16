#include "ThreadCache.h"

//申请空间
void* ThreadCache::Allocate(size_t size)
{
	//预防措施，防止将要给出的内存大于可以给出的最大大小
	assert(size <= MAXBYTES);

	//根据用户需要size的大小计算ThreadCache应该给出的内存块大小
	size = ClassSize::Roundup(size);

	//根据内存块的大小计算出内存块所在自由链表的下标
	size_t index = ClassSize::Index(size);
	FreeList& freelist = _freelist[index];
	if (!freelist.Empty())
	{
		//表示该处的自由链表下含有可用的内存块
		return freelist.Pop();
	}

	//走到该处及说明该自由链表没有可用的内存块
	
	return nullptr;
}