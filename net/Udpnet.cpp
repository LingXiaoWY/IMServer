#include"Udpnet.h"
#include"packDef.h"
#include<process.h>
#include"UdpMediator.h"
#include<INetMediator.h>

UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_hThreadHandle(NULL),m_isStop(false)
{
	m_pMediator = pMediator;
}

UdpNet::~UdpNet()
{
	UnInitNet();
}

//初始化网络，加载库、创建套接字、绑定IP地址
bool UdpNet::InitNet()
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
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        //qDebug()<<"error";
        WSACleanup();
        return false;
    }
    //申请有线广播权限
    BOOL bval = TRUE;
    setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));
    //3、选地址-绑定Ip地址
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_UDP_PORT);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
    if (err == SOCKET_ERROR) {
        
        closesocket(m_sock);
        return false;
    }
    else {
        printf("bind succeded\n");
    }
    //4、创建线程 接收数据
    //CreateThread是WinAPI,退出使用ExitThread函数。
    //使用CreateThread创建的线程，如果在线程中使用了strcpy这类C/C++运行时库函数，会创建内存块，但是在线程退出时不回收，会导致内存泄露
    //使用_beginthreadex创建的线程，退出时使用_endthreadex函数，这个函数实在ExitThread函数基础上拓展的,
    //会回收运行时库函数创建的内存块，就不会造成内存泄漏。
    m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    //qDebug()<<"success"<<endl;
    return true;
}

//发送数据
bool UdpNet::SendData(long lSendIp, char* buf, int nLen)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_DEF_UDP_PORT);
    addr.sin_addr.S_un.S_addr = lSendIp;
    if (sendto(m_sock, buf, nLen, 0, (SOCKADDR*)&addr, sizeof(addr))==SOCKET_ERROR)
    {
        return false;
    }
	return true;
}
//接收数据
void UdpNet::RecvData()
{
    char recvBuf[_DEF_BUFFER_SIZE] = "";
    sockaddr_in addrClient;
    int addrClientSize = sizeof(addrClient);
    m_isStop = false;
    int nRecvNum = 0;
    while (!m_isStop) {
        nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrClient, &addrClientSize);
        if (nRecvNum > 0) {
            char* packbuf = new char[nRecvNum];
            memcpy(packbuf,recvBuf,nRecvNum);
            //通过中介者类，把数据发给kernel类进行处理
            if (m_pMediator) {
                m_pMediator->DealData(addrClient.sin_addr.S_un.S_addr, packbuf, nRecvNum);
                //TODO:回收packBuf空间，由使用数据的对象决定
            }
        }
    }
}

unsigned int _stdcall UdpNet::RecvThread(void* lpvoid)
{
	UdpNet* pThis = (UdpNet*)lpvoid;
	pThis->RecvData();
	return 0;
}

void UdpNet::UnInitNet()
{
	//退出线程
	//退出标志位设置成true，尝试正常退出线程
    m_isStop = true;
    //等100ms，没有退出就强制杀死线程
    if (m_hThreadHandle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadHandle, 100)) {
            TerminateThread(m_hThreadHandle,-1);
        }
        CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
    //关闭句柄
	//关闭socket
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//卸载库
	WSACleanup();
}
