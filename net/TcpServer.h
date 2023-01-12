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
    //��ʼ������
    bool InitNet();
    //�ر�����
    void UnInitNet();
    //��������
    bool SendData(long lSendIp, char* buf, int nLen);
    //��������
    void RecvData();
    static unsigned int _stdcall RecvThread1(void* lpvoid);
    static unsigned int _stdcall AcceptThread(void* lpvoid);
    SOCKET m_sockListen;
    list<HANDLE>m_hThreadHandleList;
    map<unsigned int, SOCKET>m_mapThreadIdToSocket;
    bool m_isStop;
};

