#pragma once
#include<WinSock2.h>
#include<iostream>
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
class INetMediator;

class INet {
public:
	INet() {}
	virtual ~INet() {}//����ָ��ָ���������ʹ������������������
	//��ʼ������
	virtual bool InitNet() = 0;
	//�ر�����
	virtual void UnInitNet() = 0;
	//��������
	virtual bool SendData(long lSendIp, char* buf, int nLen) = 0;
protected:
	//��������
	virtual void RecvData() = 0;
	INetMediator* m_pMediator = nullptr;//�н��߽ӿ�ָ��
};