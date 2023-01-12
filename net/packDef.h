#pragma once
#include<string>
using namespace std;
#define _DEF_UDP_PORT (12345)
#define _DEF_BUFFER_SIZE (4096)
#define _DEF_NAME_SIZE 60
#define _DEF_CONTENT_SIZE (1024)
#define _DEF_TCP_PORT (54321)
#define _DEF_TCP_SERVER_IP ("10.71.65.155")
#define _DEF_MAX_SIZE (100)
#define _DEF_CONTENT_MAX_SIZE (4096)
#define _DEF_PROTOCAL_COUNT (10)

//协议头
#define _DEF_PROTOCAL_BASE (1000)
//上线
#define _DEF_UDP_ONLINE_RQ (_DEF_PROTOCAL_BASE + 1)
#define _DEF_UDP_ONLINE_RS (_DEF_PROTOCAL_BASE + 2)
//聊天
#define _DEF_UDP_CHAT_RQ (_DEF_PROTOCAL_BASE + 3)
// 下载
#define _DEF_UDP_OFFLINE_RQ (_DEF_PROTOCAL_BASE+4)
//传文件

//注册
#define _DEF_TCP_REGISTER_RQ (_DEF_PROTOCAL_BASE+10)
#define _DEF_TCP_REGISTER_RS (_DEF_PROTOCAL_BASE+11)
//登录
#define _DEF_TCP_LOGIN_RQ (_DEF_PROTOCAL_BASE+12)
#define _DEF_TCP_LOGIN_RS (_DEF_PROTOCAL_BASE+13)
//好友信息
#define _DEF_TCP_FRIEND_INFO (_DEF_PROTOCAL_BASE+14)
//添加好友
#define _DEF_TCP_ADD_FRIEND_RQ (_DEF_PROTOCAL_BASE+15)
#define _DEF_TCP_ADD_FRIEND_RS (_DEF_PROTOCAL_BASE+16)
//聊天
#define _DEF_TCP_CHAT_RQ (_DEF_PROTOCAL_BASE+17)
#define _DEF_TCP_CHAT_RS (_DEF_PROTOCAL_BASE+18)
//下线
#define _DEF_TCP_OFFLINE (_DEF_PROTOCAL_BASE+19)
//返回结果
//注册请求的返回结果
#define register_success (0)
#define user_is_exist (1)
#define parameter_error (2)
#define register_fail (3)
//登录请求的返回结果
#define login_success (0)
#define password_error (1)
#define user_not_exist (3)
//添加好友的返回结果
#define add_success (0)
#define no_this_exist (1)
#define user_refuse (2)
#define user_offline (3)
//聊天信号的发送结果
#define send_success (0)
#define send_error (1)
//好友状态
#define status_online (0)
#define status_offline (1)

//上线请求&回复结构体，包括：协议头，名字
struct STRU_ONLINE{
	STRU_ONLINE() :nType(_DEF_UDP_ONLINE_RQ)
	{
		memset(szName, 0, _DEF_NAME_SIZE);
	}
	int nType;
	char szName[_DEF_NAME_SIZE];
};

//下线请求结构体，包括：协议头
struct STRU_OFFLINE_RQ {
	STRU_OFFLINE_RQ() :nType(_DEF_UDP_OFFLINE_RQ){}
	int nType;
};

//聊天请求结构体，包括：协议头，聊天内容
struct STRU_Communication_RQ {
	STRU_Communication_RQ() :nType(_DEF_UDP_CHAT_RQ)
	{
		memset(szContent, 0, _DEF_NAME_SIZE);
	}
	int nType;
	char szContent[_DEF_CONTENT_SIZE];
};

//TCP结构体
typedef int PackType;
// 注册请求，协议头、昵称、密码、手机号
typedef struct STRU_TCP_REGISTER_RQ {
	STRU_TCP_REGISTER_RQ() :type(_DEF_TCP_REGISTER_RQ) {
		memset(name, 0, _DEF_MAX_SIZE);
		memset(tel, 0, _DEF_MAX_SIZE);
		memset(password, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char name[_DEF_MAX_SIZE];
	char tel[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}STRU_TCP_REGISTER_RQ;
//注册回复、协议头、注册结束
typedef struct STRU_TCP_REGISTER_RS {
	STRU_TCP_REGISTER_RS() :type(_DEF_TCP_REGISTER_RS) , result(register_success){}
	PackType type;
	int result;
}STRU_TCP_REGISTER_RS;

// 登录请求，协议头、电话号码、密码
typedef struct STRU_TCP_LOGIN_RQ {
	STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ){
		memset(tel, 0, _DEF_MAX_SIZE);
		memset(password, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char tel[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}STRU_TCP_LOGIN_RQ;


// 登录回复，协议头、登录结果、用户id
typedef struct STRU_TCP_LOGIN_RS {
	STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), result(login_success), userid(0) {}
	PackType type;
	int result;
	int userid;
}STRU_TCP_LOGIN_RS;

// 好友信息，协议头、昵称、用户状态、签名、头像、用户ID
typedef struct STRU_TCP_FRIEND_INFO {
	STRU_TCP_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), state(status_online), iconId(0), userId(0){
		memset(name, 0, _DEF_MAX_SIZE);
		memset(feeling, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char name[_DEF_MAX_SIZE];
	char feeling[_DEF_MAX_SIZE];
	int state;
	int iconId;
	int userId;
}STRU_TCP_FRIEND_INFO;

// 添加好友请求，协议头、用户id、用户昵称、被添加用户的昵称
typedef struct STRU_TCP_ADD_FRIEND_RQ {
	STRU_TCP_ADD_FRIEND_RQ() :userId(0), type(_DEF_TCP_ADD_FRIEND_RQ)
	{
		memset(name, 0, _DEF_MAX_SIZE);
		memset(friendname, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	char name[_DEF_MAX_SIZE];
	char friendname[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RQ;
// 添加好友回复，协议头、添加结果、被添加用户的昵称、被添加用户的id
typedef struct STRU_TCP_ADD_FRIEND_RS {
	STRU_TCP_ADD_FRIEND_RS() :result(add_success),friendId(0), type(_DEF_TCP_ADD_FRIEND_RS)
	{
		memset(friendname, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int friendId;
	int result;
	char friendname[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RS;
//_DEF_CONTENT_MAX_SIZE
// 聊天请求，协议头、聊天内容、接收者的id、发送者的Id
typedef struct STRU_TCP_CHAT_RQ {
	STRU_TCP_CHAT_RQ():type(_DEF_TCP_CHAT_RQ),userId(0), friendId(0){
		memset(content, 0, sizeof(content));
	}
	PackType type;
	int userId;
	int friendId;
	char content[_DEF_CONTENT_MAX_SIZE];
}STRU_TCP_CHAT_RQ;

// 聊天回复，协议头、发送结果、接收者的id、发送者的id
typedef struct STRU_TCP_CHAT_RS {
	STRU_TCP_CHAT_RS() :type(_DEF_TCP_CHAT_RS), userId(0), friendId(0) , result(send_success){}
	PackType type;
	int userId;
	int friendId;
	int result;
}STRU_TCP_CHAT_RS;

// 下线请求，协议头、用户id
typedef struct STRU_TCP_OFFLINE {
	STRU_TCP_OFFLINE() :type(_DEF_TCP_OFFLINE), userId(0) {}
	PackType type;
	int userId;
}STRU_TCP_OFFLINE;