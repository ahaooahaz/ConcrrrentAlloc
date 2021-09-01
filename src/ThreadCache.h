#pragma once
#include "Common.hpp"
#include "CentralCache.h"

class ThreadCache {
public:

	static ThreadCache* GetInstance() {
		return &_Inst;
	}

	/**
	 * @brief 申请内存资源
	 * @param [in] size 申请内存的大小(byte)
	 * @return 申请到的内存的地址，失败为空
	 */
	void* Allocate(size_t size);

	/**
	 * @brief 释放内存
	 * @param [in] ptr 释放的内存位置
	 */
	void Deallocate(void* ptr);

private:
	void ReturnToCentralCache(FreeList &freelist);
	void* FetchFromCentralCache(size_t index, size_t byte);
private:
	FreeList _freelist[NLISTS + 1];

	static __thread ThreadCache _Inst;
};