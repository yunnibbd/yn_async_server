﻿#ifndef __CMEMORY_POOL_H__
#define __CMEMORY_POOL_H__

#include <mutex>
#include "./../export_api.h"

#define MAX_MEMORY_SIZE 4096

class CMemoryAlloc;

//内存块 最小单元
class CMemoryBlock
{
public:
	//所属内存块(池)
	CMemoryAlloc* pAlloc;
	//下一块位置
	CMemoryBlock* pNext;
	//内存块编号
	int nID;
	//引用次数
	int nRef;
	//是否在内存池中
	bool bPool;
private:
	//预留
	char c1;
	char c2;
	char c3;
};

//内存池
class CMemoryAlloc
{
public:
	CMemoryAlloc();

	~CMemoryAlloc();

	void *AllocMemory(size_t nSize);

	void FreeMemory(void* pMem);

	//初始化
	void InitMemory();
protected:
	//内存池地址
	char* p_buf_;
	//头部头部单元
	CMemoryBlock* p_header_;
	//内存单元的大小
	size_t block_size_;
	//内存单元的数量
	size_t block_num_;
	//申请内存的锁
	std::mutex mutex_;
};

//这个确定类型的模板类用于确定内存池的内存单元大小和内存单元数量
template<size_t block_size, size_t block_num>
class CMemoryAlloctor : public CMemoryAlloc
{
public:
	CMemoryAlloctor()
	{
		//如果block_size不是内存对齐的数 4(32位)或8(64位)的倍数
		//指针大小在32位操作系统中是4字节，在64位操作系统中是8字节
		const size_t n = sizeof(void*);
		CMemoryAlloc::block_size_ = (block_size / n) * n + (block_size % n ? n : 0);
		CMemoryAlloc::block_num_ = block_num;
	}
};

//内存管理
class COMMON_EXPORT CMemoryPool
{
public:
	//单例
	static CMemoryPool* Get()
	{
		static CMemoryPool mgr;
		return &mgr;
	}

	void* AllocMem(size_t nSize);

	void FreeMem(void* pMem);

	//增加1内存块的引用计数，暂时用不上
	void AddRef(void* pMem);
private:
	//初始化内存池映射数组
	void Init_szAlloc(int nBegin, int nEnd, CMemoryAlloc* pMem);

	CMemoryPool();

	~CMemoryPool();

	//内存池的每个内存单元是64字节大小，共有100000块内存单元
	CMemoryAlloctor<64, 100000> mem64_;
	//内存池的每个内存单元是128字节大小，共有100000块内存单元
	CMemoryAlloctor<128, 100000> mem128_;
	//内存池的每个内存单元是256字节大小，共有100000块内存单元
	CMemoryAlloctor<256, 100000> mem256_;
	//内存池的每个内存单元是256字节大小，共有100000块内存单元
	CMemoryAlloctor<512, 100000> mem512_;
	CMemoryAlloctor<1024, 10000> mem1024_;
	CMemoryAlloctor<2048, 10000> mem2048_;
	CMemoryAlloctor<4096, 10000> mem4096_;
	CMemoryAlloc* sz_alloc_[MAX_MEMORY_SIZE + 1];
};

//重载内存申请操作符
void* operator new(size_t size);

void operator delete(void* p);

void* operator new[](size_t size);

void operator delete[](void* p);

#endif
