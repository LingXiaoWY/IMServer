#pragma
#include"INetMediator.h"

class UDPMediator :public INetMediator
{
public:
	UDPMediator();
	~UDPMediator();
	//������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen);

};