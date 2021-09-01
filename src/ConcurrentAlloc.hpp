#ifndef __CONCURRENTALLOC_HPP__
#define __CONCURRENTALLOC_HPP__

#include "Common.hpp"
#include "ThreadCache.h"

template <class T>
class ConcurrentAlloc {
    public:
        ConcurrentAlloc() noexcept {}
        ConcurrentAlloc( const ConcurrentAlloc& other ) noexcept {}
        template<class U> ConcurrentAlloc( const ConcurrentAlloc<U>& other) noexcept {} // 泛化的拷贝构造函数
        ~ConcurrentAlloc() noexcept {}

        // 返回某个对象的地址
        T* address(T& t) {
            return &t;
        }

        // 返回某个const对象的地址
        const T* address(const T& t) const {
            return &t;
        }

        // 配置空间，足以存储n个T对象
        T* allocate(std::size_t n, const void* = 0) {
            DLOG("allocate sizeof(T): %ld, size: %ld\n", sizeof(T), n);
            ThreadCache::GetInstance()->Allocate(sizeof(T)*n);
        }

        // 释放之前配置好的空间
        void deallocate( T* p, std::size_t n ) {
            ThreadCache::GetInstance()->Deallocate(p);
        }

        // 返回可成功配置的最大量，未动态变化
        size_t max_size() const {
            return INT64_MAX;
        }

        // 调用对象的构造函数
        void construct(T& p, const T& val) {}

        // 调用对象的析构函数
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
};

#endif