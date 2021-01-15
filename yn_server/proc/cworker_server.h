#ifndef __CWORKER_SERVER_H__
#define __CWORKER_SERVER_H__
#ifdef _WIN32
#include "./../platform/cworker_server_impl_iocp.h"
#else
#include "./../platform/cworker_server_impl_epoll.h"
#endif

//通配windows和linux的消息处理服务器
#ifdef _WIN32
class CWorkerServer : public CWorkerServerImplIocp
#else
class CWorkerServer : public CWorkerServerImplEpoll
#endif
{

};

#endif
