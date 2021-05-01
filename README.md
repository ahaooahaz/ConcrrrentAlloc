# ConcrrrentAlloc

[![Build Status](https://travis-ci.com/AHAOAHA/ConcurrentMemoryPool.svg?branch=master)](https://travis-ci.com/AHAOAHA/ConcurrentMemoryPool)

适用于高并发场景的空间配置器，内部为内存分层结构，逐级合理分配内存块，减少内存碎片的产生。

# 分层

## ThreadCache

线程独有的对象，负责向线程提供所需要的内存块大小，直接与目标线程进行交互，当ThreadCache资源用尽时，向下层申请合适数量的资源继续用于分配。

## CentralCache

负责向各个系统中的ThreadCaches合理分配内存块资源，当CentralCache资源用尽时，继续向下层的PageCache申请页级资源。

## PageCache

负责与OS系统接口交互，申请资源保证内存连续并且足够大，维护并分配所申请到的内存资源。
