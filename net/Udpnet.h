#pragma once
#include"INet.h"

class UdpNet :public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UnInitNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
protected:
	//��������
	void RecvData();
	static unsigned int _stdcall RecvThread(void* lpvoid);
	SOCKET m_sock;
	HANDLE m_hThreadHandle;
	bool m_isStop;
};