#pragma once
#include"INet.h"

class TcpClient :public INet
{
public:
	TcpClient(INetMediator* pMediator);
	~TcpClient();
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UnInitNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void RecvData();
protected:
	static unsigned int _stdcall RecvThread2(void* lpvoid);
	SOCKET m_sock;
	HANDLE m_hThreadHandle;
	bool m_isStop;
};