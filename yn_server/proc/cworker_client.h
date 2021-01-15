#ifndef __CWORKER_CLIENT_H__
#define __CWORKER_CLIENT_H__
#ifdef _WIN32
#include "./../platform/cworker_client_impl_iocp.h"
#else
#include "./../cworker_client_impl_epoll.h"
#endif

#ifdef _WIN32
class CWorkerClient : public CWorkerClientImplIOCP
#else
class CWorkerClient : public CWorkerClientImplEpoll
#endif
{
public:

};

#endif
