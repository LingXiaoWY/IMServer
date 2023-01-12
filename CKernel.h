#pragma once
#include"INetMediator.h"
#include"CMysql.h"
#include<iostream>
#include"TcpServerMediator.h"
#include"packDef.h"
using namespace std;

class CKernel;//������CKernel�����
typedef void (CKernel::* FUN) (long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel(void);
	~CKernel(void);
	//���������
	bool startServer();
	//�رշ����
	void closeServer();
	void dealData(long lSendIp, char* buf, int nLen);
	static CKernel* pKernel;
private:
	//��ʼ��Э��ӳ���
	void setProtocolMap();
	//����ע������
	void dealRegisterRq(long lSendIp, char* buf, int nLen);
	//�����¼����
	void dealLoginRq(long lSendIp, char* buf, int nLen);
	FUN m_netProtocolMap[_DEF_PROTOCAL_COUNT];
	CMySql m_sql;
	INetMediator* m_pMediator;
};

