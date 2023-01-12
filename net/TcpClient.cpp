#include"TcpClient.h"
#include"packDef.h"
#include<process.h>
#include"TcpClientMediator.h"
#include<INetMediator.h>

TcpClient::TcpClient(INetMediator* pMediator) :m_isStop(false),m_sock(INVALID_SOCKET),m_hThreadHandle(NULL)
{
    m_pMediator = pMediator;
}

TcpClient::~TcpClient()
{
    UnInitNet();
}

//初始化网络，加载库、创建套接字、绑定IP地址
bool TcpClient::InitNet()
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
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_sock == INVALID_SOCKET)
    {
        //qDebug()<<"error";
        WSACleanup();
        return false;
    }
    //3、选地址-绑定Ip地址
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_TCP_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);
    err = connect(m_sock, (sockaddr*)&addr, sizeof(addr));
    if (err == SOCKET_ERROR) {

        closesocket(m_sock);
        return false;
    }
    else {
        printf("connect succeded\n");
    }
    m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread2, this, 0, NULL);
    return true;
}

//发送数据
bool TcpClient::SendData(long lSendIp, char* buf, int nLen)
{
    //1、参数校验
    if (!buf || nLen <= 0) {
        return false;
    }
    //2、先发包大小
    if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0)
    {
        cout << WSAGetLastError() << endl;
        return false;
    }
    //3、再发包内容
    if (send(m_sock, buf, nLen, 0) <= 0) {
        return false;
    }
    return true;
}
//接收数据
void TcpClient::RecvData()
{
    int nRecvNum = 0;
    int nPackSize = 0;
    int offset = 0;
    while (!m_isStop)
    {
        //接收包大小
        nRecvNum = recv(m_sock,(char*)&nPackSize,sizeof(int),0 );
        if (nRecvNum > 0) {
            //接收包内容
            char* buf = new char[nPackSize];
            while (nPackSize) {
                nRecvNum = recv(m_sock, buf + offset, nPackSize, 0);
                if (nRecvNum > 0) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //接收完一个完整的包，offset清零，数据发送给中介者类
            m_pMediator->DealData(m_sock, buf, offset);
            offset = 0;
        }
    }
}

void TcpClient::UnInitNet()
{
    //1、退出线程
    m_isStop = true;
    if (WaitForSingleObject(m_hThreadHandle, 100) == WAIT_TIMEOUT) {
        TerminateThread(m_hThreadHandle, -1);
    }
    CloseHandle(m_hThreadHandle);
    //2、关闭socket
    if (m_sock && m_sock!=INVALID_SOCKET)
    {
        closesocket(m_sock);
    }
    WSACleanup();
}

unsigned int _stdcall TcpClient::RecvThread2(void* lpvoid) {
    TcpClient* pThis = (TcpClient*)lpvoid;
    pThis->RecvData();
    return 0;
}