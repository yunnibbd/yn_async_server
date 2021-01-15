#include "cthread_pool.h"
#include "cthread.h"
#include "./../cabstract_task.h"
#include <vector>
#include <list>
#include <iostream>
#include <mutex>
#include <condition_variable>
using namespace std;

//创建的线程池是否全部退出
bool is_stop_all = false;
//存放所有的线程的队列
static vector<CThread*> all_threads;

CThreadPool::CThreadPool()
{

}

//析构
CThreadPool::~CThreadPool()
{
	Close();
}

//所有线程OnRun时执行的函数
void CThreadPool::ThreadTask(CThread *pthread)
{
	while (pthread->IsRun())
	{
		unique_lock<mutex> tmplock(mux_);
		while (tasks_.empty() && !is_stop_)
			//线程睡眠，等待唤醒
			cv_.wait(tmplock);
		if (is_stop_all)
			break;
		//走到这里必然任务列表中有任务
		CAbstractTask *task = tasks_.front();
		tasks_.pop_front();
		//操作完马上解锁
		tmplock.unlock();
		//执行task
		task->Init();
	}
}

//初始化线程池，threadnums为线程数量
bool CThreadPool::Init(int threadnums)
{
	for (int i = 0; i < threadnums; ++i)
	{
		auto th = new CThread();
		threads_.push_back(th);
		th->Start(
			nullptr,
			[this](CThread *pthread) {
				ThreadTask(pthread);
			},
			nullptr
		);
	}
	is_init_ = true;
	thread_num_ = threads_.size();
	return true;
}

void CThreadPool::Close()
{
	if (!is_init_) return;
	StopAll();
	for (auto &th : threads_)
	{
		delete th;
	}
	threads_.clear();
	is_init_ = false;
}

//停止所有线程
void CThreadPool::StopAll()
{
	for (auto &th : threads_)
	{
		th->Exit();
	}
	is_stop_ = true;
}

//调度任务执行
void CThreadPool::DispatchTask(CAbstractTask *task)
{
	if (!is_init_) return;
	lock_guard<mutex> tmplock(mux_);
	tasks_.push_back(task);
	//激活一个线程来处理这个任务
	cv_.notify_one();
}

//创建并返回一个线程池
CThreadPool *CThreadPoolFactory::Create()
{
	return new CThreadPool();
}

//等待所有线程退出
void CThreadPoolFactory::Wait()
{
	while (!is_stop_all)
	{
		CThread::Sleep(100);
	}
}
