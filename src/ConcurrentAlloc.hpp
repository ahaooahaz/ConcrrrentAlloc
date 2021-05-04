#ifndef __CONCURRENTALLOC_HPP__
#define __CONCURRENTALLOC_HPP__

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
        // ThreadCache* _thr_threadCache = nullptr;
          
};

#endif