#include "CKernel.h"
#define NetPckFunMap(a) m_netProtocolMap[a-_DEF_PROTOCAL_BASE-10]
CKernel* CKernel::pKernel = nullptr;
CKernel::CKernel(void)
{
	pKernel = this;
	setProtocolMap();
}
CKernel::~CKernel(void)
{
	//���ùرշ���
	closeServer();
}

void CKernel::setProtocolMap()
{
	memset(m_netProtocolMap, 0, sizeof(m_netProtocolMap));
	//��Э��ͷ�ʹ�����
	NetPckFunMap(_DEF_TCP_REGISTER_RQ) = &CKernel::dealRegisterRq;
	NetPckFunMap(_DEF_TCP_LOGIN_RQ) = &CKernel::dealLoginRq;
}


//����ע������
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen)
{
	cout << "dealRegisterRq" << endl;
	//1�����
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	STRU_TCP_REGISTER_RS rs;
	//2��У���ǳơ��绰�š�����(У�����ͬ�ͻ���)
	//2.1��У���ַ�������Ϊ�գ�����Ϊ�ո�
	string nameStr = rq->name;
	string telStr = rq->tel;
	string passwordStr = rq->password;
	if (strlen(rq->name)<= 0 || strlen(rq->tel) <= 0 || strlen(rq->password) <= 0) {
		rs.result = parameter_error;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		return;
	}
	//3�����ݵ绰������ǳƲ�ѯ�Ƿ��Ѿ����������
	char sqlBuf[1024] = "";
	list<string>resultList;
	sprintf(sqlBuf, "select id from t_user where tel = '%s:';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf,1, resultList))
	{
		cout << "��ѯ���ݿ�ʧ��" << endl;
		rs.result = parameter_error;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		return;
	}
	if (resultList.size() > 0) {
		//4��������ݵ绰����������ˣ�����ע�������û��Ѵ���,ע��ʧ��
		rs.result = user_is_exist;
	}
	else {
		//5�����û������ˣ��ٸ����ǳƲ�ѯһ���Ƿ����������
		sprintf(sqlBuf, "select id from t_user where tel = '%s';", rq->name);
		if (!m_sql.SelectMySql(sqlBuf, 1, resultList))
		{
			cout << "��ѯ���ݿ�ʧ��" << endl;
			rs.result = parameter_error;
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
			return;
		}
		if (resultList.size() > 0) {
			rs.result = user_is_exist;
		}
		else {
			rs.result = register_success;
			sprintf(sqlBuf, "insert into t_user (tel,password,name,icon,feeling) values ('%s','%s','%s',1,'ɶҲûд');", rq->tel, rq->password, rq->name);
			if (!m_sql.UpdateMySql(sqlBuf)) {
				cout << "�������ݿ�ʧ��" << endl;
				rs.result = register_fail;
				m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));
				return;
			}
		}
	}
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//�����¼����
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen)
{
	cout << "dealLoginRq" << endl;
	//1�����
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	//2��У���û����������--�Լ�д��
	//3������tel��ѯ�û���Ϣ��id������)
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf,2,resultList)) {
		cout << "��ѯ���ݿ�ʧ��" << endl;
		return;
	}
	STRU_TCP_LOGIN_RS rs;
	int id = 0;
	string passwordStr = "";
	if (resultList.size() == 0) {
		//4������鲻������ˣ����õ�¼������û�������
		rs.result = user_not_exist;
	}
	else{
		//5������鵽����ˣ��ж������Ƿ�һ��
		id = atoi(resultList.front().c_str());
		resultList.pop_front();
		passwordStr = resultList.front();
		resultList.pop_front();
		if (strcmp(passwordStr.c_str(), rq->password )!= 0) {
			//6��������벻һ�£����õ�¼�������¼ʧ�ܣ��������
			rs.result = password_error;
		}
		else {
			//7���������һ�£����õ�¼�������¼�ɹ�
			rs.result = login_success;
		}
	}
	//8���Żظ��ͻ��˵�¼���
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}


bool CKernel::startServer()
{
	//1���������ݿ�
	if (!m_sql.ConnectMySql((char*)"127.0.0.1", (char*)"root", (char*)"gwy2002812GT", (char*)"0913test")) {
		cout << "�����ݿ�ʧ��" << endl;
		return false;
	}
	//2��������
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->OpenNet())
	{
		cout << "������ʧ��" << endl;
		return false;
	}
	return true;
}

void CKernel::closeServer()
{
	//1��������Դ

	//2���Ͽ����ݿ�����
	m_sql.DisConnect();
	//3���ر�����
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//������
void CKernel::dealData(long lSendIp, char* buf, int nLen)
{
	cout << "dealData" << endl;
	//1��ȡ��Э��ͷ
	PackType nType = *(PackType*)buf;
	//����Э��ͷ���ж����ĸ���������
	//2���ж�Э��ͷ�Ƿ�����Ч��Χ��
	if (nType >= _DEF_TCP_REGISTER_RQ && nType <= _DEF_TCP_OFFLINE) {
		//3�����������±꣬��ȡ��Ӧ�Ĵ�����ָ��
		FUN pFun = NetPckFunMap(nType);
		//4�����ô�����
		if (pFun) {
			(this->*pFun)(lSendIp, buf, nLen);
		}
	}
	//5������buf
	delete buf;
}

