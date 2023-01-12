#pragma once
#include"INet.h"
#include<process.h>
#include<list>
#include<map>
using namespace std;
class TcpServer :public INet
{
public:
    TcpServer(INetMediator* pMediator);
    ~TcpServer();
    //初始化网络
    bool InitNet();
    //关闭网络
    void UnInitNet();
    //发送数据
    bool SendData(long lSendIp, char* buf, int nLen);
    //接收数据
    void RecvData();
    static unsigned int _stdcall RecvThread1(void* lpvoid);
    static unsigned int _stdcall AcceptThread(void* lpvoid);
    SOCKET m_sockListen;
    list<HANDLE>m_hThreadHandleList;
    map<unsigned int, SOCKET>m_mapThreadIdToSocket;
    bool m_isStop;
};

