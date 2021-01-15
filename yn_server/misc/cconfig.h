#ifndef __CCONFIG_H__
#define __CCONFIG_H__
#include "./../export_api.h"
#include <vector>

//每一项配置
struct ConfigItem
{
	char key[50] = { 0 };
	char val[50] = { 0 };
};

//单例配置类
class COMMON_EXPORT CConfig
{
public:
	~CConfig();

	static CConfig *Get()
	{
		static CConfig *ins = new CConfig;
		return ins;
	}

	//读取配置文件
	bool Load(const char *filename);

	//删除所有配置项
	void DelAllConfig();

	//获取配置项
	char *GetString(const char *key);

	//获取配置项，可以传入默认值
	int GetIntDefault(const char *key, int def);

	//测试打印所有配置项
	void ShowAllConfig();
private:
	//私有化构造函数
	CConfig(){}
	//存储所有的配置项
	std::vector<ConfigItem *> configs_;
	//是否是第一次Load标志位
	bool is_first_load_ = false;
};

#endif
