#include "cabstract_client.h"
#include "./../cbase_msg.h"
#include "./../misc/clog.h"
#include "./../utils/ctools.h"
#include "./../misc/cbuffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#define MAX_SEND_PKG 2048

CAbstractClient::CAbstractClient(int sockfd, int send_buf_size, int recv_buf_size)
	: sockfd_(sockfd), send_buf_size_(send_buf_size), recv_buf_size_(recv_buf_size)
{

}

//析构
CAbstractClient::~CAbstractClient()
{
	is_init_ = false;
	Destory();
}

//获得本客户端的socket
int CAbstractClient::sockfd()
{
	return sockfd_;
}

//初始化事件
void CAbstractClient::Init()
{
	p_send_buf_ = new CBuffer(send_buf_size_);
	p_recv_buf_ = new CBuffer(recv_buf_size_);
	is_init_ = true;
}

//销毁资源
void CAbstractClient::Destory()
{
	if (sockfd_ != SOCK_NOT_INIT)
	{
		CTools::CloseSocket(sockfd_);
		sockfd_ = SOCK_NOT_INIT;
	}
	if (p_send_buf_)
	{
		delete p_send_buf_;
		p_send_buf_ = nullptr;
	}
	if (p_recv_buf_)
	{
		delete p_recv_buf_;
		p_recv_buf_ = nullptr;
	}
}

//只关闭socket
void CAbstractClient::CloseSocket()
{
	if (sockfd_ != SOCK_NOT_INIT)
	{
		CTools::CloseSocket(sockfd_);
		sockfd_ = SOCK_NOT_INIT;
	}
}

//发回数据
int CAbstractClient::SendData(CBaseMsg *msg)
{
	return SendData((const char*)msg, msg->data_length_);
}

//使用websocket的方式发送数据
int CAbstractClient::WSSendData(char *data, int data_len)
{
	char *send_buffer = nullptr;

	int header = 1; //0x81
	if (data_len <= 125)
		header++;
	else if (data_len <= 0xffff)
		header += 3;
	else
		header += 9;
	if (header + data_len > MAX_SEND_PKG)
		send_buffer = new char[header + data_len];
	else
		send_buffer = new char[MAX_SEND_PKG];

	unsigned int send_len;
	//固定的头
	send_buffer[0] = 0x81;
	if (data_len <= 125)
	{
		//最高bit为0
		send_buffer[1] = data_len;
		send_len = 2;
	}
	else if (data_len <= 0xffff)
	{
		send_buffer[1] = 126;
		send_buffer[2] = (data_len & 0x000000ff);
		send_buffer[3] = ((data_len & 0x0000ff00) >> 8);
		send_len = 4;
	}
	else
	{
		send_buffer[2] = (data_len & 0x000000ff);
		send_buffer[3] = ((data_len & 0x0000ff00) >> 8);
		send_buffer[4] = ((data_len & 0x00ff0000) >> 16);
		send_buffer[5] = ((data_len & 0xff000000) >> 24);

		send_buffer[6] = 0;
		send_buffer[7] = 0;
		send_buffer[8] = 0;
		send_buffer[9] = 0;
		send_len = 10;
	}
	memcpy(send_buffer + send_len, data, data_len);
	send_len += data_len;
	int ret = SendData(send_buffer, send_len);

	delete[] send_buffer;
	return ret;
}

//发回数据
int CAbstractClient::SendData(const char *data, int data_len)
{
	if (p_send_buf_->Push(data, data_len))
		//只是将数据放入发送缓冲区，在下次检测可以发送的时候再发送
		return data_len;
	return 0;
}

//立即发送发送缓冲区的数据
int CAbstractClient::SendDataReal()
{
	return p_send_buf_->Write2Socket(sockfd_);
}

//接收数据
int CAbstractClient::RecvData()
{
	return p_recv_buf_->Read4Socket(sockfd_);
}

//发送缓冲区是否有数据需要发送
bool CAbstractClient::NeedWrite()
{
	return p_send_buf_->HasData();
}

//接收缓冲区是否有数据需要处理
bool CAbstractClient::NeedRead()
{
	return p_recv_buf_->HasData();
}

#ifdef USE_IOCP //如果使用了IOCP模式
IO_DATA_BASE *CAbstractClient::MakeRecvIOData()
{
	if (is_post_recv_)
		return nullptr;
	is_post_recv_ = true;
	return p_recv_buf_->MakeRecvIOData(sockfd_);
}

//告知缓冲区本次接收到数据的长度
void CAbstractClient::Recv4Iocp(int nRecv)
{
	if (!is_post_recv_)
		LOG_DEBUG("CAbstractClient recv4Iocp, is_post_recv_=false");
	is_post_recv_ = false;
	p_recv_buf_->Read4Iocp(nRecv);
}

IO_DATA_BASE *CAbstractClient::MakeSendIOData()
{
	if (is_post_send_)
		return nullptr;
	is_post_send_ = true;
	return p_send_buf_->MakeSendIOData(sockfd_);
}

//告知本次要发送的数据长度
void CAbstractClient::Send2Iocp(int nSend)
{
	if (!is_post_send_)
		LOG_DEBUG("CAbstractClient send2Iocp, is_post_send_=false");
	is_post_send_ = false;
	p_send_buf_->Write2Iocp(nSend);
}

//是否投递了接收或者发送数据的任务
bool CAbstractClient::IsPostIoAction()
{
	return is_post_recv_ || is_post_send_;
}
#endif //#ifdef USE_IOCP
