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
	//调用关闭服务
	closeServer();
}

void CKernel::setProtocolMap()
{
	memset(m_netProtocolMap, 0, sizeof(m_netProtocolMap));
	//绑定协议头和处理函数
	NetPckFunMap(_DEF_TCP_REGISTER_RQ) = &CKernel::dealRegisterRq;
	NetPckFunMap(_DEF_TCP_LOGIN_RQ) = &CKernel::dealLoginRq;
}


//处理注册请求
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen)
{
	cout << "dealRegisterRq" << endl;
	//1、拆包
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	STRU_TCP_REGISTER_RS rs;
	//2、校验昵称、电话号、密码(校验规则同客户端)
	//2.1、校验字符串不能为空，不能为空格
	string nameStr = rq->name;
	string telStr = rq->tel;
	string passwordStr = rq->password;
	if (strlen(rq->name)<= 0 || strlen(rq->tel) <= 0 || strlen(rq->password) <= 0) {
		rs.result = parameter_error;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		return;
	}
	//3、根据电话号码和昵称查询是否已经有这个人了
	char sqlBuf[1024] = "";
	list<string>resultList;
	sprintf(sqlBuf, "select id from t_user where tel = '%s:';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf,1, resultList))
	{
		cout << "查询数据库失败" << endl;
		rs.result = parameter_error;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		return;
	}
	if (resultList.size() > 0) {
		//4、如果根据电话号码有这个人，设置注册结果：用户已存在,注册失败
		rs.result = user_is_exist;
	}
	else {
		//5、如果没有这个人，再根据昵称查询一下是否有这个人了
		sprintf(sqlBuf, "select id from t_user where tel = '%s';", rq->name);
		if (!m_sql.SelectMySql(sqlBuf, 1, resultList))
		{
			cout << "查询数据库失败" << endl;
			rs.result = parameter_error;
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
			return;
		}
		if (resultList.size() > 0) {
			rs.result = user_is_exist;
		}
		else {
			rs.result = register_success;
			sprintf(sqlBuf, "insert into t_user (tel,password,name,icon,feeling) values ('%s','%s','%s',1,'啥也没写');", rq->tel, rq->password, rq->name);
			if (!m_sql.UpdateMySql(sqlBuf)) {
				cout << "更新数据库失败" << endl;
				rs.result = register_fail;
				m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));
				return;
			}
		}
	}
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//处理登录请求
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen)
{
	cout << "dealLoginRq" << endl;
	//1、拆包
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	//2、校验用户输入的数据--自己写、
	//3、根据tel查询用户信息（id、密码)
	list<string>resultList;
	char sqlBuf[1024] = "";
	sprintf(sqlBuf, "select id,password from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf,2,resultList)) {
		cout << "查询数据库失败" << endl;
		return;
	}
	STRU_TCP_LOGIN_RS rs;
	int id = 0;
	string passwordStr = "";
	if (resultList.size() == 0) {
		//4、如果查不到这个人，设置登录结果：用户不存在
		rs.result = user_not_exist;
	}
	else{
		//5、如果查到这个人，判断密码是否一致
		id = atoi(resultList.front().c_str());
		resultList.pop_front();
		passwordStr = resultList.front();
		resultList.pop_front();
		if (strcmp(passwordStr.c_str(), rq->password )!= 0) {
			//6、如果密码不一致，设置登录结果：登录失败，密码错误
			rs.result = password_error;
		}
		else {
			//7、如果密码一致，设置登录结果：登录成功
			rs.result = login_success;
		}
	}
	//8、放回给客户端登录结果
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}


bool CKernel::startServer()
{
	//1、连接数据库
	if (!m_sql.ConnectMySql((char*)"127.0.0.1", (char*)"root", (char*)"gwy2002812GT", (char*)"0913test")) {
		cout << "打开数据库失败" << endl;
		return false;
	}
	//2、打开网络
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->OpenNet())
	{
		cout << "打开网络失败" << endl;
		return false;
	}
	return true;
}

void CKernel::closeServer()
{
	//1、回收资源

	//2、断开数据库连接
	m_sql.DisConnect();
	//3、关闭网络
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = nullptr;
	}
}

//处理函数
void CKernel::dealData(long lSendIp, char* buf, int nLen)
{
	cout << "dealData" << endl;
	//1、取出协议头
	PackType nType = *(PackType*)buf;
	//根据协议头，判断走哪个处理流程
	//2、判断协议头是否在有效范围内
	if (nType >= _DEF_TCP_REGISTER_RQ && nType <= _DEF_TCP_OFFLINE) {
		//3、计算数组下标，获取对应的处理函数指针
		FUN pFun = NetPckFunMap(nType);
		//4、调用处理函数
		if (pFun) {
			(this->*pFun)(lSendIp, buf, nLen);
		}
	}
	//5、回收buf
	delete buf;
}

