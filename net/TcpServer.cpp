#include"TcpServer.h"
#include"packDef.h"
#include<process.h>
#include"TcpServerMediator.h"
#include<INetMediator.h>

TcpServer::TcpServer(INetMediator* pMediator) :m_isStop(false), m_sockListen(INVALID_SOCKET)
{
    m_pMediator = pMediator;
}

TcpServer::~TcpServer()
{
    UnInitNet();
}

//初始化网络，加载库、创建套接字、绑定IP地址
bool TcpServer::InitNet()
{
    //1、加载库---WSAtartup
    WORD wVersionRequested = { 0 };
    WSADATA wsaData = { 0 };
    int err;
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        //* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup();
        return false;
    }
    //2、创建套接字
    m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sockListen == INVALID_SOCKET)
    {
        //qDebug()<<"error";
        WSACleanup();
        return false;
    }
    //3、选地址-绑定Ip地址
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(_DEF_TCP_PORT);
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    err = bind(m_sockListen, (sockaddr*)&server, sizeof(server));
    if (err == SOCKET_ERROR) {
        closesocket(m_sockListen);
        WSACleanup();
        return false;
    }
    else {
        printf("connect succeded\n");
    }
    //4、监听--listen
    if (listen(m_sockListen, 10) == SOCKET_ERROR) {
        printf("listen error:%d\n", WSAGetLastError());
        closesocket(m_sockListen);
        WSACleanup();
        return false;
    }
    else {
        printf("listen succeeded\n");
    }
    //5、创建线程--接受连接
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, this, 0, 0);
    if (handle) {
        m_hThreadHandleList.push_back(handle);
    }
    return true;
}

//发送数据
bool TcpServer::SendData(long lSendIp, char* buf, int nLen)
{
    //1、参数校验
    if (!buf || nLen <= 0) {
        return false;
    }
    //2、先发包大小
    if (send(lSendIp, (char*)&nLen, sizeof(int), 0) <= 0)
    {
        return false;
    }
    //3、再发包内容
    if (send(lSendIp, buf, nLen, 0) <= 0) {
        return false;
    }
    return true;
}
//接收数据
void TcpServer::RecvData()
{
    //1、休眠，因为创建线程以后立刻就执行线程，但是把线程id和套接字存入map需要一点时间
    Sleep(100);
    //2、从map中取出当前线程对应的socket
    SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];
    if (!sock || sock == INVALID_SOCKET)return;
    //3、开始接收数据
    int nRecvNum = 0;
    int nPackSize = 0;
    int offset = 0;
    while (!m_isStop)
    {
        //接收包大小
        nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
        if (nRecvNum > 0) {
            //接收包内容
            char* buf = new char[nPackSize];
            while (nPackSize) {
                nRecvNum = recv(sock, buf + offset, nPackSize, 0);
                if (nRecvNum > 0) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //接收完一个完整的包，offset清零，数据发送给中介者类
            m_pMediator->DealData(sock, buf, offset);
            offset = 0;
        }
    }
}

void TcpServer::UnInitNet()
{
    //1、退出线程
    m_isStop = true;
    for (auto ite = m_hThreadHandleList.begin(); ite != m_hThreadHandleList.end();)
    {
        if (*ite)
        {
            if (WaitForSingleObject(*ite, 100) == WAIT_TIMEOUT)
            {
                TerminateThread(*ite, -1);
            }
            CloseHandle(*ite);
            *ite = nullptr;
        }
        ite = m_hThreadHandleList.erase(ite);
    }
    //2、关闭socket
    if (m_sockListen && m_sockListen != INVALID_SOCKET) {
        closesocket(m_sockListen);
    }
    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
        if (ite->second && ite->second != INVALID_SOCKET)
        {
            closesocket(ite->second);
        }
        ite = m_mapThreadIdToSocket.erase(ite);
    }
    //3、卸载库
    WSACleanup();
}

unsigned int _stdcall TcpServer::RecvThread1(void* lpvoid){
    TcpServer* pThis = (TcpServer*)lpvoid;
    pThis->RecvData();
    return 0;
}

unsigned int _stdcall TcpServer::AcceptThread(void* lpvoid){
    TcpServer* pThis = (TcpServer*)lpvoid;
    sockaddr_in clientAddr;
    int addrSize = sizeof(clientAddr);
    SOCKET sockClient = INVALID_SOCKET;
    while (!pThis->m_isStop) {
        //1、接受连接
        sockClient = accept(pThis->m_sockListen, (sockaddr*)&clientAddr, &addrSize);
        cout << "ip:" << inet_ntoa(clientAddr.sin_addr) << "connect" << endl;
        //2、给每个客户端创建一个接收数据的线程
        unsigned int nThreadId = 0;
        HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread1, pThis, 0, &nThreadId);
        if (handle) {
            //保存线程句柄
            pThis->m_hThreadHandleList.push_back(handle);
            //保存每个客户端的socket
            pThis->m_mapThreadIdToSocket[nThreadId] = sockClient;
        }
    }
    return 0;
}