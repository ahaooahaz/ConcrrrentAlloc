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
	return FetchFromCentralCache(index, size);
}

//想Centralcache申请一定数量的目标内存块
void* ThreadCache::FetchFromCentralCache(size_t index, size_t byte)
{
	assert(byte <= MAXBYTES);
	FreeList& freelist = _freelist[index];
	size_t num = 10;	//想要从CentralCache拿到的内存块的个数

	void *start, *end;	//标记拿到的内存	fetchnum表示真实拿到的内存块个数
	size_t fetchnum = CentralCache::GetInstance()->FetchRangeObj(start, end, num, byte);
	if (fetchnum == 1)
	{
		//如果只从CentralCache拿到一块就不用将剩余的内存块挂载在自由链表下
		return start;
	}

	freelist.PushRange(NEXT_OBJ(start), end, fetchnum - 1);
	return start;
}

//释放内存块
void ThreadCache::Deallocate(void* ptr, size_t size)
{
	size = ClassSize::Roundup(size);
	size_t index = ClassSize::Index(size);
	FreeList& freelist = _freelist[index];

	//将内存块头插
	freelist.Push(ptr);

	//当自由链表中对象数量超过一次从CentralCache中获取的对象数量时
	//开始将内存返还到中心CentralCache
	if (freelist.Size() >= freelist.MaxSize())
	{
		ReturnToCentralCache(freelist, size);
	}
}

void ThreadCache::ReturnToCentralCache(FreeList &freelist, size_t byte)
{
	void* start = freelist.Clear();
	CentralCache::GetInstance()->ReturnToCentralCache(start, byte);
}