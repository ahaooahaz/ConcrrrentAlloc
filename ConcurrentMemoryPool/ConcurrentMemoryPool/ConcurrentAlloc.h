#pragma once
#include "Common.h"
#include "ThreadCache.h"

void* ConcurrentAlloc(size_t size);
void ConcurrentFree(void* ptr, size_t size);