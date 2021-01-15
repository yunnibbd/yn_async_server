#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include "./../export_api.h"
#include <vector>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

class CAbstractTask;
class CThread;

//一般不直接创建, 使用工厂创建
class COMMON_EXPORT CThreadPool
{
public:
	//构造
	CThreadPool();
	//析构
	~CThreadPool();

	//初始化线程池，threadnums为线程数量
	bool Init(int threadnums);

	//关闭线程池
	void Close();

	//停止所有线程
	void StopAll();

	//调度任务执行
	void DispatchTask(CAbstractTask *task);

	//所有线程执行的任务
	void ThreadTask(CThread *pthread);

	//线程池是否在运行
	bool IsRun() { return is_init_; }
private:
	//线程池是否已经初始化
	bool is_init_ = false;
	//本线程池线程数量
	int thread_num_ = 0;
	//存放所有的线程对象
	std::vector<CThread *> threads_;
	//存放所有要执行的任务列表
	std::list<CAbstractTask *> tasks_;
	//任务列表互斥量
	std::mutex mux_;
	//条件变量
	std::condition_variable cv_;
	//当前线程池是否退出
	bool is_stop_ = false;
};

//专门用这个工厂创建线程池
class COMMON_EXPORT CThreadPoolFactory
{
public:
	//创建一个线程池
	static CThreadPool *Create();

	//等待所有线程退出
	static void Wait();
};

#endif
