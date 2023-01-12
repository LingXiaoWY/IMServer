#include "TcpClientMediator.h"
#include"TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	//new �������
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator()//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//������
bool TcpClientMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

//�ر�����
void TcpClientMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//��������:ͬʱ����tcp udp
bool TcpClientMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		return false;
	}
	return true;
}
//��������
#include<iostream>
void TcpClientMediator::DealData(long lSendlp, char* buf, int nLen)
{
	//TODO:�����ݴ���kernel��
	std::cout << buf << std::endl;
}

