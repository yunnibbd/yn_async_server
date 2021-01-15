#ifndef __CABSTRACT_TASK_H__
#define __CABSTRACT_TASK_H__

//所有在线程池中要执行的任务都要继承这个类
class CAbstractTask
{
public:
	virtual void Init() = 0;
};

#endif
