#ifndef __CBASE_MSG_H__
#define __CBASE_MSG_H__
#include "export_api.h"

enum CMD
{
	CMD_PING, //心跳命令
};

struct COMMON_EXPORT CBaseMsg
{
	CBaseMsg(CMD cmd)
	{
		data_length_ = sizeof(CBaseMsg);
		cmd_ = cmd;
	}
	int data_length_ = 0;
	CMD cmd_;
	char username[20] = { 0 };
};

#endif
