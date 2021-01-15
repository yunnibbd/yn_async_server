#ifndef __CSERVER_H__
#define __CSERVER_H__
#include "./../export_api.h"
#include "./../error_code.h"
#include "./../cabstract_task.h"
#include "./../platform/ciocp.h"
#include "./../misc/cthread.h"
#include "./../misc/ctimer.h"
#include <set>
#include <vector>
#include <time.h>

class CClient;
struct CBaseMsg;
class CThreadPool;

//所有服务端的基础抽象类
class COMMON_EXPORT CAbstractServer : public CAbstractTask
{
public:
	//初始化事件
	virtual void Init() {}

	//客户端加入事件
	virtual void OnJoin(CClient *pclient) = 0;	

	//客户端离开事件
	virtual void OnLeave(CClient *pclient) = 0;

	//服务端接收消息事件
	virtual void OnRecv(CClient *pclient) {}

	//处理消息回调
	virtual void OnMsg(CClient *pclient, CBaseMsg *msg) {}

	//处理二进制回调
	virtual void OnBin(CClient *pclient, char *data, int data_len) {}

	//处理json回调
	virtual void OnJson(CClient *pclient, char *data, int data_len) {}

	void set_server_port(unsigned short port) { server_port_ = port; }
	void set_server_ip(const char *ip);
protected:
	//本服务端持有的socket
	int sockfd_ = SOCK_NOT_INIT;
	//本服务端的port
	unsigned short server_port_ = SERVER_PORT_NOT_INIT;
	//本服务端的ip
	char server_ip_[16] = { 0 };
};

#endif
