#ifndef __CABSTRACT_CLIENT_H__
#define __CABSTRACT_CLIENT_H__
#include "./../error_code.h"
#include "./../cabstract_task.h"
#include "./../export_api.h"
#include "./../platform/ciocp.h"

struct CBaseMsg;
class CBuffer;

enum SocketType
{
	TCP_SOCKET_IO = 0,  //tcp socket
	WEB_SOCKET_IO = 1,  //websocket
};

enum ProtocalType
{
	MSG_PROTOCAL = 0,  //消息协议
	BIN_PROTOCAL = 1,  //二进制协议
	JSON_PROTOCAL = 2, //json协议
};

//client的基类, 其中只封装数据IO操作
class COMMON_EXPORT CAbstractClient : public CAbstractTask
{
public:
	//构造
	CAbstractClient(int sockfd = SOCK_NOT_INIT, 
		int send_buf_size = 4096, int recv_bu_size = 4096);

	//析构
	~CAbstractClient();

	//初始化事件
	virtual void Init() override;

	//连接成功事件
	virtual void OnConnected() {}

	//销毁资源
	void Destory();

	//获得本客户端的socket
	int sockfd();

	//只关闭socket
	void CloseSocket();

	//发回数据
	int SendData(CBaseMsg *msg);

	//发回数据
	int SendData(const char *data, int data_len);

	//使用websocket的方式发送数据
	int WSSendData(char *data, int data_len);

	//立即发送发送缓冲区的数据
	int SendDataReal();

	//接收数据
	int RecvData();

	//发送缓冲区是否有数据需要发送
	bool NeedWrite();

	//接收缓冲区是否有数据需要处理
	bool NeedRead();

	//获得接收缓冲区指针
	CBuffer *p_recv_buf() { return p_recv_buf_; }

	SocketType GetSocketType() { return socket_type_;  }
	void SetSocketType(SocketType type) { socket_type_ = type; }

	ProtocalType GetProtocalType() { return protocal_type_; }
	void SetProtocalType(ProtocalType type) { protocal_type_ = type; }

#ifdef USE_IOCP //如果使用了IOCP模式
	IO_DATA_BASE *MakeRecvIOData();

	//告知缓冲区本次接收到数据的长度
	void Recv4Iocp(int nRecv);

	IO_DATA_BASE *MakeSendIOData();

	//告知本次要发送的数据长度
	void Send2Iocp(int nSend);

	//是否投递了接收或者发送数据的任务
	bool IsPostIoAction();
#endif //#ifdef USE_IOCP

	//是否初始化标志位
	bool is_init_ = false;
	//本客户端对应的远端客户端的ip
	char ip_[32] = { 0 };
	//本客户端对应的远端客户端的端口
	unsigned short port_ = 0;
	//SocketType类型为WEB_SOCKET_IO下时候先进行了连接
	bool is_ws_connected = false;
protected:
	//接收缓冲区指针
	CBuffer *p_recv_buf_ = nullptr;
	//发送缓冲区指针
	CBuffer *p_send_buf_ = nullptr;
	//接受缓冲区大小
	int send_buf_size_ = 0;
	//发送缓冲区大小
	int recv_buf_size_ = 0;
	//本客户端持有的socket
	int sockfd_ = SOCK_NOT_INIT;
	//0 表示TCP socket, 1表示是 websocket
	SocketType socket_type_ = TCP_SOCKET_IO;
	//0 表示消息协议, 格式为包头加包尾(默认为消息协议)
	//1 表示二进制协议，size + 数据的模式
	//2 表示文本协议，以\r\n来分解收到的数据为一个包
	ProtocalType protocal_type_ = MSG_PROTOCAL;
#ifdef USE_IOCP //如果使用了IOCP模式
	//IOCP是否投递了请求
	bool is_post_recv_ = false;
	bool is_post_send_ = false;
#endif
};

#endif
