#pragma once
#include <assert.h>

//Common.h主要存放项目所用到的数据结构及常量

//表示自由链表中一共有240类大小的内存块
const size_t NLISTS = 240;

//表示自由链表中可以给出的最大的内存块的大小为64KB
const size_t MAXBYTES = 64 * 1024;

//表示PageCache中页种类的数量 即最大为128页 0页处浪费
const size_t NPAGES = 129;

inline void*& NEXT_OBJ(void* ptr)
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

	void Push(void* obj)
	{
		NEXT_OBJ(obj) = _ptr;
		_ptr = obj;
		++_size;
	}

	void PushRange(void* start, void* end, size_t num)
	{
		NEXT_OBJ(end) = _ptr;
		_ptr = start;
		_size += num;
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
			return _Roundup(size, 8);
		}
		else if(size <= 1024)
		{
			return _Roundup(size, 16);
		}
		else if(size <= 8 * 1024)
		{
			return _Roundup(size, 128);
		}
		else if(size <= 64 * 1024)
		{
			return _Roundup(size, 512);
		}

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

	//计算应该给出多少个内存块，内存块数控制在[2, 512]之间
	static size_t NumMoveSize(size_t byte)
	{
		if (byte == 0)
		{
			return 0;
		}
		int num = (int)(MAXBYTES / byte);
		if (num < 2)
			num = 2;

		if (num > 512)
			num = 512;
		return num;
	}

	static size_t NumMovePage(size_t byte)
	{
		//计算应该给出多少块内存块
		size_t num = NumMoveSize(byte);
		size_t npage = (size_t)((num * byte) / (4 * 1024));	//根据所需要给出的内存块数计算应该需要几页的连续内存 1页=4K
		if (npage == 0)
			npage = 1;

		return npage;
	}
};

typedef size_t PageID;
struct Span
{
	PageID _pageid = 0;	// 记录页号
	size_t _npage = 0;	// 记录该Span中一共有几页 1页=4k

	//Span为带头双向循环链表
	Span* _prev = nullptr;
	Span* _next = nullptr;

	void* _objlist = nullptr;	// 对象自由链表
	size_t _objsize = 0;	//对象内存块的大小
	size_t _usecount = 0;	//对象内存块实用计数
};

//CentralCache的数据结构
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	bool Empty()
	{
		return _head->_next == _head;
	}

	Span* Pop()
	{
		assert(!Empty());
		Span* span = _head->_next;
		Span* next = span->_next;

		_head->_next = next;
		next->_prev = _head;
		return span;
	}

private:
	Span* _head = nullptr;
};