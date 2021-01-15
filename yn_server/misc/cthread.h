#ifndef __CTHREAD_H__
#define __CTHREAD_H__

#include "csemaphore.h"
#include "./../export_api.h"
#include <thread>
#include <functional>

//封装的线程类
class COMMON_EXPORT CThread
{	
public:
	typedef std::function<void(CThread *)> Call;

	//开始本线程
	void Start(Call on_create = nullptr, Call on_run = nullptr, Call on_destory = nullptr);

	//停止本线程
	void Stop();

	//本线程退出
	void Exit();

	//本线程是否在运作
	bool IsRun();

	//本线程进入休眠状态
	void Wait();

	//唤醒本线程
	void Wakeup();

	//本线程休眠一段时间(单位毫秒)
	static void Sleep(int msec = 0);
protected:
	//本线程入口函数
	void Main();

private:
	//线程是否在运行
	bool is_run_ = false;
	//线程的互斥量
	std::mutex mux_;
	//控制线程的休眠
	CSemaphore sem_;
	//线程创建时执行
	Call on_create_ = nullptr;
	//线程运转时执行
	Call on_run_ = nullptr;
	//线程销毁时执行
	Call on_destory_ = nullptr;
};

#endif
