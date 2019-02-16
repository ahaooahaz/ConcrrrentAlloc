#pragma once
#include <assert.h>

//Common.h主要存放项目所用到的数据结构及常量

//表示自由链表中一共有240类大小的内存块
const size_t NLISTS = 240;

//表示自由链表中可以给出的最大的内存块的大小为64KB
const size_t MAXBYTES = 64 * 1024 * 1024;

void*& NEXT_OBJ(void* ptr)
{
	return *((void**)ptr);
}

class FreeList
{
public:
	bool Empty()
	{
		return _ptr == nullptr;
	}

	void* Pop()
	{
		void* obj = _ptr;
		_ptr = NEXT_OBJ(_ptr);
		return obj;
	}

private:
	void* _ptr = nullptr;

	//记录该自由链表链接的内存块的size
	size_t _size = 0;
};

class ClassSize
{
public:

	//align在这里表示对齐数，该函数的作用为根据size计算出应该分配多大的内存块
	static inline size_t _Roundup(size_t size, size_t align)
	{
		//return ((size + align) / align * align);
		return ((size + align - 1) & ~(align - 1));
	}

	//根据size的大小计算应该给出的内存块大小
	static inline size_t Roundup(size_t size)
	{
		assert(size <= MAXBYTES);

		//总体上将freelist分为四段	为什么要采用这样的对齐规则？
		//[8, 128]									8B对齐 采用STL内存池的分段规则
		//[129, 1024]							16B对齐
		//[1025, 8 * 1024]					128B对齐
		//[8 * 1024 + 1, 64 * 1024]		512B对齐
		if(size <= 128)
		{
			_Roundup(size, 8);
		}
		else if(size <= 1024)
		{ }
		else if(size <= 8 * 1024)
		{ }
		else if(size < 64 * 1024)
		{ }

		//程序走到这里时说明size已经超越最大的内存块，与首部相呼应
		assert(false);
		return -1;
	}
	static inline size_t _Index(size_t size, size_t align)
	{
		return _Roundup(size, align) / align - 1;
	}

	static inline size_t Index(size_t size)
	{
		assert(size <= MAXBYTES);

		if(size <= 128)
		{
			return _Index(size, 8);
		}
		else if(size <= 1024)
		{
			return _Index(size - 128, 16) + 16;
		}
		else if(size <= 8 * 1024)
		{
			return _Index(size - 1024, 128) + 16 + 56;
		}
		else if(size <= 64 * 1024)
		{
			return _Index(size - 8 * 1024, 512) + 16 + 56 + 112;
		}

		//程序到达该步骤，一定是之前某处出错了
		assert(false);
		return -1;
	}
};