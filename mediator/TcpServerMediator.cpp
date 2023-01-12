#include "TcpServerMediator.h"
#include"TcpServer.h"
#include"../CKernel.h"
TcpServerMediator::TcpServerMediator()
{
	//new �������
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator()//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//������
bool TcpServerMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

//�ر�����
void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//��������:ͬʱ����tcp udp
bool TcpServerMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		return false;
	}
	return true;
}
//��������
#include<iostream>
void TcpServerMediator::DealData(long lSendlp, char* buf, int nLen)
{
	//TODO:�����ݴ���kernel��
	CKernel::pKernel->dealData(lSendlp, buf, nLen);
}
