#include "cthread.h"
#include <thread>
#include <chrono>
using namespace std;

//开启本线程
void CThread::Start(Call on_create, Call on_run, Call on_destory)
{
	std::lock_guard<std::mutex> tmpmux(mux_);
	if (!is_run_)
	{
		is_run_ = true;
		if (on_create)
			on_create_ = on_create;
		if (on_run)
			on_run_ = on_run;
		if (on_destory)
			on_destory_ = on_destory;
		// 将线程启动放在后面，确保传进线程的本对象的三个事件都被准确赋值
		std::thread t(&CThread::Main, this);
		t.detach();
	}
}

//停止本线程
void CThread::Stop()
{	std::lock_guard<std::mutex> tmpmux(mux_);
	if (is_run_)
	{
		//如果本线程运行标志位是在运行，那么置为false
		is_run_ = false;
		//如果本线程循环还在运作，但是线程对象已经销毁的时候会出现问题
		//这里阻塞本线程，线程循环停下来的时候再唤醒
		Wait();
	}
}

//本线程退出，直接退出线程，但是没有信号量的阻塞
void CThread::Exit()
{
	std::lock_guard<std::mutex> tmpmux(mux_);
	if (is_run_)
	{
		//如果本线程运行标志位是在运行，那么置为false
		is_run_ = false;
	}
}

//本线程休眠一段时间(单位毫秒)
void CThread::Sleep(int msec)
{
	if (msec > 0)
	{
		chrono::milliseconds t(msec);
		this_thread::sleep_for(t);
	}
}

//本线程是否在运作
bool CThread::IsRun()
{
	return is_run_;
}

//本线程进入休眠状态
void CThread::Wait()
{
	sem_.wait();
}

//唤醒本线程
void CThread::Wakeup()
{
	sem_.wakeup();
}

//本线程入口函数
void CThread::Main()
{
	// 如果注册了线程启动事件，就执行
	if (on_create_)
		on_create_(this);
	if (on_run_)
		on_run_(this);
	if (on_destory_)
		on_destory_(this);
	//线程循环结束，唤醒此线程
	Wakeup();
	//本线程运行标志标记为false
	is_run_ = false;
}
