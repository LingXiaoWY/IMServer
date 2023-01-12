#include "TcpServerMediator.h"
#include"TcpServer.h"
#include"../CKernel.h"
TcpServerMediator::TcpServerMediator()
{
	//new 网络对象
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator()//使用时，父类指针指向子类，使用虚析构
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//打开网络
bool TcpServerMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

//关闭网络
void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//发送数据:同时兼容tcp udp
bool TcpServerMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		return false;
	}
	return true;
}
//处理数据
#include<iostream>
void TcpServerMediator::DealData(long lSendlp, char* buf, int nLen)
{
	//TODO:把数据传给kernel类
	CKernel::pKernel->dealData(lSendlp, buf, nLen);
}
