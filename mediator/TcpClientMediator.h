#pragma once
#pragma
#include"INetMediator.h"

class TcpClientMediator :public INetMediator
{
public:
	TcpClientMediator();
	~TcpClientMediator();
	//������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen);

};

