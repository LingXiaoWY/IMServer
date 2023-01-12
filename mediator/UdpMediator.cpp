#include"UdpMediator.h"
#include"Udpnet.h"
UDPMediator::UDPMediator()
{
	//new �������
	m_pNet = new UdpNet(this);
}
UDPMediator::~UDPMediator()//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//������
bool UDPMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

//�ر�����
void UDPMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//��������:ͬʱ����tcp udp
bool UDPMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		return false;
	}
	return true;
}
//��������
#include<iostream>
void UDPMediator::DealData(long lSendlp, char* buf, int nLen)
{
	//TODO:�����ݴ���kernel��
	std::cout << buf << std::endl;
}
