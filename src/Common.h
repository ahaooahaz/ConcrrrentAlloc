#pragma once
#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;

const size_t NLISTS = 240;

const size_t MAXBYTES = 64 * 1024;

const size_t NPAGES = 129;

inline void*& NEXT_OBJ(void* ptr)
{
	return *((void**)ptr);
}

class FreeList
{
public:
	size_t Size()
	{
		return _size;
	}

	size_t MaxSize()
	{
		return 10;
	}

	void* Clear()
	{
		void* start = _ptr;
		_ptr = nullptr;
		_size = 0;
		return start;
	}

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
		--_size;
		return obj;
	}

private:
	void* _ptr = nullptr;

	//��¼�������������ӵ��ڴ���size
	size_t _size = 0;
};

class ClassSize
{
public:
	//align�������ʾ���������ú���������Ϊ����size�����Ӧ�÷�������ڴ��
	static inline size_t _Roundup(size_t size, size_t align)
	{
		//return ((size + align) / align * align);
		return ((size + align - 1) & ~(align - 1));
	}

	//����size�Ĵ�С����Ӧ�ø������ڴ���С
	static inline size_t Roundup(size_t size)
	{
		assert(size <= MAXBYTES);

		//�����Ͻ�freelist��Ϊ�Ķ�	ΪʲôҪ���������Ķ������
		//[8, 128]									8B���� ����STL�ڴ�صķֶι���
		//[129, 1024]							16B����
		//[1025, 8 * 1024]					128B����
		//[8 * 1024 + 1, 64 * 1024]		512B����
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

		//�����ߵ�����ʱ˵��size�Ѿ���Խ�����ڴ�飬���ײ����Ӧ
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

		//���򵽴�ò��裬һ����֮ǰĳ��������
		assert(false);
		return -1;
	}

	//����Ӧ�ø������ٸ��ڴ�飬�ڴ����������[2, 512]֮��
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
		//����Ӧ�ø������ٿ��ڴ��
		size_t num = NumMoveSize(byte);
		size_t npage = (size_t)((num * byte) / (4 * 1024));	//��������Ҫ�������ڴ��������Ӧ����Ҫ��ҳ�������ڴ� 1ҳ=4K
		if (npage == 0)
			npage = 1;

		return npage;
	}
};

typedef size_t PageID;
struct Span {
	PageID _pageid = 0;
	size_t _npage = 0;

	Span* _prev = nullptr;
	Span* _next = nullptr;

	void* _objlist = nullptr;
	size_t _objsize = 0;
	size_t _usecount = 0;
};

class SpanList {
public:
	SpanList() {
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	void Insert(Span* cur, Span* newspan)
	{
		assert(cur);
		Span* prev = cur->_prev;

		prev->_next = newspan;
		newspan->_prev = prev;
		newspan->_next = cur;
		cur->_prev = newspan;
	}

	void Earse(Span* cur)
	{
		assert(cur != nullptr && cur != _head);

		Span* prev = cur->_prev;
		Span* next = cur->_next;

		prev->_next = next;
		next->_prev = prev;
	}

	bool Empty()
	{
		return _head->_next == _head;
	}

	Span* Begin()
	{
		return _head->_next;
	}

	Span* End()
	{
		return _head;
	}

	Span* Pop()
	{
		Span* span = Begin();
		Earse(span);
		return span;
	}


	void PushFront(Span* span)
	{
		Insert(Begin(), span);
	}


private:
	Span* _head = nullptr;
};