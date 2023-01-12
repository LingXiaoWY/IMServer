#include "TcpClientMediator.h"
#include"TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	//new 网络对象
	m_pNet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator()//使用时，父类指针指向子类，使用虚析构
{
	if (m_pNet) {
		delete m_pNet;
		m_pNet = nullptr;
	}
}
//打开网络
bool TcpClientMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

//关闭网络
void TcpClientMediator::CloseNet()
{
	m_pNet->UnInitNet();
}
//发送数据:同时兼容tcp udp
bool TcpClientMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		return false;
	}
	return true;
}
//处理数据
#include<iostream>
void TcpClientMediator::DealData(long lSendlp, char* buf, int nLen)
{
	//TODO:把数据传给kernel类
	std::cout << buf << std::endl;
}

