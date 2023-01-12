#pragma once
#include<WinSock2.h>
#include<iostream>
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
class INetMediator;

class INet {
public:
	INet() {}
	virtual ~INet() {}//父类指针指向子类对象，使用虚析构，回收子类
	//初始化网络
	virtual bool InitNet() = 0;
	//关闭网络
	virtual void UnInitNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIp, char* buf, int nLen) = 0;
protected:
	//接收数据
	virtual void RecvData() = 0;
	INetMediator* m_pMediator = nullptr;//中介者接口指针
};