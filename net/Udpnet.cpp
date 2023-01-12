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

//��ʼ�����磬���ؿ⡢�����׽��֡���IP��ַ
bool UdpNet::InitNet()
{
    //1�����ؿ�---WSAtartup
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
    //2�������׽���
    m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_sock == INVALID_SOCKET)
    {
        //qDebug()<<"error";
        WSACleanup();
        return false;
    }
    //�������߹㲥Ȩ��
    BOOL bval = TRUE;
    setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));
    //3��ѡ��ַ-��Ip��ַ
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
    //4�������߳� ��������
    //CreateThread��WinAPI,�˳�ʹ��ExitThread������
    //ʹ��CreateThread�������̣߳�������߳���ʹ����strcpy����C/C++����ʱ�⺯�����ᴴ���ڴ�飬�������߳��˳�ʱ�����գ��ᵼ���ڴ�й¶
    //ʹ��_beginthreadex�������̣߳��˳�ʱʹ��_endthreadex�������������ʵ��ExitThread������������չ��,
    //���������ʱ�⺯���������ڴ�飬�Ͳ�������ڴ�й©��
    m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    //qDebug()<<"success"<<endl;
    return true;
}

//��������
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
//��������
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
            //ͨ���н����࣬�����ݷ���kernel����д���
            if (m_pMediator) {
                m_pMediator->DealData(addrClient.sin_addr.S_un.S_addr, packbuf, nRecvNum);
                //TODO:����packBuf�ռ䣬��ʹ�����ݵĶ������
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
	//�˳��߳�
	//�˳���־λ���ó�true�����������˳��߳�
    m_isStop = true;
    //��100ms��û���˳���ǿ��ɱ���߳�
    if (m_hThreadHandle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadHandle, 100)) {
            TerminateThread(m_hThreadHandle,-1);
        }
        CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
    //�رվ��
	//�ر�socket
	if (m_sock && INVALID_SOCKET != m_sock)
	{
		closesocket(m_sock);
	}
	//ж�ؿ�
	WSACleanup();
}
