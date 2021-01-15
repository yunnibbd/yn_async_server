#include "./misc/cmemory_pool.h"
#include "./misc/cconfig.h"
#include <iostream>
#include <thread>
#include "./proc/cmaster_server.h"
#ifdef _WIN32
#include <Windows.h>
#include <signal.h>
#pragma comment(lib, "ws2_32.lib")
#endif
using namespace std;

int main()
{
#ifdef _WIN32
	WSAData wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
#endif
	//初始化配置文件
	CConfig *p_config = CConfig::Get();
	p_config->Load("yn.conf");
	//获得要绑定的ip和端口
	char *ip = p_config->GetString("BindnIP");
	if (!ip) ip = (char*)"127.0.0.1";
	int port = p_config->GetIntDefault("ListenPort", 8000);
	//创建server
	CMasterServer server;
	//设置server port
	server.set_server_port(port);
	//设置server ip
	server.set_server_ip(ip);
	//初始化server
	server.Init();
	//开启server
	server.Start();
	while (true)
		CThread::Sleep(1000);
	return 0;
}
