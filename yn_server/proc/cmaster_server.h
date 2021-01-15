#ifndef __CMASTER_SERVER_H__
#define __CMASTER_SERVER_H__
#ifdef _WIN32
#include "./../platform/cmaster_server_impl_iocp.h"
#else
#include "./../platform/cmaster_server_impl_epoll.h"
#endif

//适配windows和linux的接收客户端服务器
#ifdef _WIN32
class CMasterServer : public CMasterServerImplIOCP
#else
class CMasterServer : public CMatserServerImplEpoll
#endif
{

};

#endif
