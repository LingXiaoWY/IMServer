#pragma once
#pragma
#include"INetMediator.h"

class TcpServerMediator :public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
	//打开网络
	bool OpenNet();
	//关闭网络
	void CloseNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
	//处理数据
	void DealData(long lSendIp, char* buf, int nLen);

};

