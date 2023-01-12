#pragma once
#include"INet.h"

class TcpClient :public INet
{
public:
	TcpClient(INetMediator* pMediator);
	~TcpClient();
	//初始化网络
	bool InitNet();
	//关闭网络
	void UnInitNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
	//接收数据
	void RecvData();
protected:
	static unsigned int _stdcall RecvThread2(void* lpvoid);
	SOCKET m_sock;
	HANDLE m_hThreadHandle;
	bool m_isStop;
};