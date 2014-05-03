#pragma once

//配置类

class CConfig
{
public:
	CConfig(void);
	~CConfig(void);
    void CheckConfigFile();
	UINT GetPort();
	UINT GetMaxConn();
	UINT GetSound();
private:
	void WriteDefaultInfo();  //写入配置信息
};
