#ifndef __CONCURRENTALLOC_HPP__
#define __CONCURRENTALLOC_HPP_s_

#include "Common.h"
#include "ThreadCache.h"

template <class T>
class ConcurrentAlloc {
    public:
        ConcurrentAlloc() noexcept {}
        // constexpr ConcurrentAlloc() noexcept {}
        ConcurrentAlloc( const ConcurrentAlloc& other ) noexcept {}

        // TODO: Free.
        ~ConcurrentAlloc() noexcept {}

        T* address(T& t) {
            return &t;
        }
        T* allocate(std::size_t n) {
            return nullptr;
        }
        void deallocate( T* p, std::size_t n ) {

        }
        size_t max_size() {
            return 1;
        }
        void construct(T& p, const T& val) {}
        void destory(T* p) {}

    
    private:
        void* ConcurrentAllocMem(size_t size); // block size bytes
        void ConcurrentFree(void* ptr);


    public:
        typedef T value_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef std::size_t size_type;

    private:
        // ThreadCache* _thr_threadCache = nullptr;	//��֤threadcacheΪ�̶߳���
          
};

// template <class T>
// void* ConcurrentAlloc<T>::ConcurrentAllocMem(size_t size)
// {
// 	if (size > MAXBYTES)
// 	{
// 		size_t npage = (size + (4 * 1024)) / (4 * 1024);
// 		return PageCache::GetInstance()->NewSpan(npage);
// 	}
	
// 	if (_thr_threadCache != nullptr)
// 		return _thr_threadCache->Allocate(size);
// 	_thr_threadCache = new ThreadCache;
// 	return ConcurrentAllocMem(size);
// }

// template <class T>
// void ConcurrentAlloc<T>::ConcurrentFree(void* ptr)
// {
// 	if (_thr_threadCache != nullptr)
// 	{
// 		_thr_threadCache->Deallocate(ptr);
// 		return;
// 	}
// }

// template <class T>
// T* ConcurrentAlloc<T>::allocate(std::size_t n) {
//     return (T*)ConcurrentAllocMem(sizeof(T) * n);
// }

// template <class T>
// void ConcurrentAlloc<T>::deallocate( T* p, std::size_t n ) {
//     ConcurrentFree((void*)p);
// }

#endif