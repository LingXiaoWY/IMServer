#pragma once
#include"INetMediator.h"
#include"CMysql.h"
#include<iostream>
#include"TcpServerMediator.h"
#include"packDef.h"
using namespace std;

class CKernel;//声明有CKernel这个类
typedef void (CKernel::* FUN) (long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel(void);
	~CKernel(void);
	//开启服务端
	bool startServer();
	//关闭服务端
	void closeServer();
	void dealData(long lSendIp, char* buf, int nLen);
	static CKernel* pKernel;
private:
	//初始化协议映射表
	void setProtocolMap();
	//处理注册请求
	void dealRegisterRq(long lSendIp, char* buf, int nLen);
	//处理登录请求
	void dealLoginRq(long lSendIp, char* buf, int nLen);
	FUN m_netProtocolMap[_DEF_PROTOCAL_COUNT];
	CMySql m_sql;
	INetMediator* m_pMediator;
};

