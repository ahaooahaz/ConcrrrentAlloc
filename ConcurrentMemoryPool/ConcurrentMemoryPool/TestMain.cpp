#include "ThreadCache.h"
//#include "Common.h"
#include <iostream>
#include <vector>
#include <thread>
#include "BeachMark.hpp"
using std::cout;
using std::endl;

//测试对齐算法
void TestRangeupALG(size_t size, size_t align)
{
	cout << "((size + align) / align * align) : "<<((size + align) / align * align) << endl;
	cout << "((size + align - 1) & ~(align - 1)) : " << ((size + align - 1) & ~(align - 1)) << endl;

}

//测试freelist下标算法
void TestIndexALG(size_t size)
{
	cout << "size: " << size << ";  _index: "<<  ClassSize::Index(size) << endl;
}

void TestAllocate(size_t size)
{
	ThreadCache t;
	void* ptr = t.Allocate(size);
	t.Deallocate(ptr);
}

void TestAlloc(size_t bytes, size_t count)
{
	std::vector<void*> v_ptr;
	for(size_t i = 0; i < count; ++i)
	{
		v_ptr.push_back(ConcurrentAlloc(bytes));
	}

	
	for(auto & e: v_ptr)
	{
		ConcurrentFree(e);
	}
	
}

int main()
{
	//TestRangeupALG(127, 8);
	//TestIndexALG(1025);
	//TestAllocate(7);
	//AHAOAHA::BeachMark(1, 100000, 7);
	//ThreadCache().Allocate(3);
	TestAlloc(3, 2541);


	system("pause");
	return 0;
}