#pragma once
#include"INet.h"

class UdpNet :public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//初始化网络
	bool InitNet();
	//关闭网络
	void UnInitNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
protected:
	//接收数据
	void RecvData();
	static unsigned int _stdcall RecvThread(void* lpvoid);
	SOCKET m_sock;
	HANDLE m_hThreadHandle;
	bool m_isStop;
};