# 高并发内存池

## 项目主体结构

![1550382576382](C:\Users\hp-pc\AppData\Roaming\Typora\typora-user-images\1550382576382.png)

* ThreadCache为个线程独有的对象，主要负责向线程提供所需要的内存块大小，ThreadCache直接与线程进行交互，当ThreadCache中没有可用的内存块时，ThreadCache就需要向下层的CentralCache申请内存块继续使用。
* CentralCache在整个项目中仅有一个对象，CentralCache主要负责向各个线程中的ThreadCache合理分配内存块资源，当CentralCache中没有可用的内存块时，它就要继续向下层的PageCache申请内存块。
* PageCache在整个项目中仅有一个对象，PageCache主要负责向系统申请一大块内存来向上分配。

## ThreadCache

### ThreadCache的结构

![1550318598002](C:\Users\hp-pc\AppData\Roaming\Typora\typora-user-images\1550318598002.png)

### Thread为各线程独有

如何控制各线程独有自己的ThreadCache？

> * 用链表链接所有的Threadcache 申请内存时 遍历链表寻找属于线程自己的ThreadCache
> * tls : 线程自己独有的全局变量，使用tls标记线程自己的threadCache

### ThreadCache的接口

> * Allocate 申请空间
> * Deallocate 释放空间

## CentralCache



### CentralCache结构

![1550318114266](C:\Users\hp-pc\AppData\Roaming\Typora\typora-user-images\1550318114266.png)

### CentralCache有且仅有一个对象

如何控制CentralCache仅有为一对象?

> * 采用单例设计模式

### CentralCache接口

> * FetchRangeObj 向ThreadCache分配内存块
> * GetOneSpan 获取一个Span
> * ReleaseListToSpans 向PageCache申请一段SpanList

## PageCache

### PageCache的结构

![1550318125677](C:\Users\hp-pc\AppData\Roaming\Typora\typora-user-images\1550318125677.png)

### PageCache仅有一个对象

如何控制PageCache仅有为一对象?

> * 采用单例模式设计PageCache类

### PageCache接口

> * NewSpan(size_t npage) 获得一个新的Span