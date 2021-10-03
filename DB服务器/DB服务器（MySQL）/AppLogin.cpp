#include "AppLogin.h"
#include "DBManager.h"

namespace app
{

	//登录
	void onLogin_1000(char* LogAccount, char* LogPassword,SOCKET gameserversocket, SOCKET playersocket)
	{
		std::string account = LogAccount;
		std::string password = LogPassword;

		auto mem = Findmember(account);
		if (mem == nullptr)
		{
			std::cout << "登录查找为null" << std::endl;
			//无相关账号返回登录结果  10001 + -1 + 玩家socket+ 玩家索引
			char sendbuff[8];
			int type = -1;
			memcpy(sendbuff, (char*)&type, 4);
			memcpy(sendbuff + 4, (char*)&playersocket, 4);

			__TCPSERVER->Send(gameserversocket, 10001, sendbuff, 8);

			std::cout << "登录失败" << std::endl;
			return;
		}

		if (strcmp(password.c_str(), mem->password) == 0)
		{
			std::cout << "登录成功1"  << mem->ID<< "|" << gameserversocket<< "|" << playersocket << std::endl;


			mem->Lastlogintime = time(NULL);
			auto db = __DBManager->DBAccount;
			auto buff = db->PopBuffer();
			buff->begin(1000);
			buff->s(mem->ID);
			buff->s(gameserversocket);
			buff->s(playersocket);


	
		
		

			db->PushToWorkThread(buff);

			
			
			return;
			
		}
		//密码错误返回登录结果  10002 + -2 + 玩家socket
		char sendbuff[8];
		int type = -2;
		memcpy(sendbuff, (char*)&type, 4);
		memcpy(sendbuff + 4, (char*)&playersocket, 4);
		__TCPSERVER->Send(gameserversocket, 10002, sendbuff, 8);

		std::cout << "登录失败" << std::endl;


	}

	//注册
	void onRegister_1001(char* RegAccount, char* RegPassword, SOCKET gameserversocket, SOCKET playersocket)
	{
		std::string account = RegAccount;
		std::string password = RegPassword;
		auto mem = Findmember(account);
		if (mem != nullptr)
		{
			std::cout << "账号已存在" << std::endl;
			int errid = -1;
			char sendbuff[48];
			memcpy(sendbuff, (char*)&errid, 4);
			memcpy(sendbuff + 4, (char*)&account, 20);
			memcpy(sendbuff + 24, (char*)&password, 20);
			memcpy(sendbuff + 44, (char*)&playersocket, 4);
			__TCPSERVER->Send(gameserversocket, 10011, sendbuff, 48);
			return;
		}

		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->begin(1001);
		buff->s((char*)account.c_str(),20);
		buff->s((char*)password.c_str(), 20);
		buff->s(gameserversocket);
		buff->s(playersocket);
		db->PushToWorkThread(buff);
	}

	void on_3000()
	{
		int memid = 1;
		auto db = __DBManager->GetDBSource(THREAD_USERREAD);
		auto buff = db->PopBuffer();
		buff->begin(3000);
		buff->s(memid);

		db->PushToWorkThread(buff);

	}



	//===================主线程执行===============================

	void db_1000(DBBuffer* buff)
	{
		int memid;
		SOCKET serversocket;
		SOCKET playersocket;
		buff->r(memid);
		buff->r(serversocket);
		buff->r(playersocket);
		cout << "登录成功" << memid  << "|"  << serversocket << "|"<< playersocket << endl;


		//成功返回登录结果  10001 + 玩家id + 玩家socket
		char sendbuff[8];
		memcpy(sendbuff, (char*)&memid, 4);
		memcpy(sendbuff + 4, (char*)&playersocket, 4);
		__TCPSERVER->Send(serversocket, 10001, sendbuff, 8);

	    

	}

	void db_1001(DBBuffer* buff)
	{

		int memid;
		char account[20];
		char password[20];
		SOCKET gameserversocket;
		unsigned int playersocket;
		memset(account, 0, 20);
		memset(password, 0, 20);
		buff->r(memid);
		buff->r(account);
		buff->r(password);
		buff->r(gameserversocket);
		buff->r(playersocket);
		if (memid >= 0)  //注册成功操作
		{
			//插入内存中
			app::USER_MEMBER_BASE* mem = new app::USER_MEMBER_BASE();
			mem->ID = memid;
			memcpy(mem->username, account, 20);
			memcpy(mem->password, password, 20);
			mem->Createtime = time(NULL);
			mem->Lastlogintime = time(NULL);

			__AccountsID.insert(make_pair(memid, mem));

			cout << "db1001 successful" << memid << " " << account << " " << password << endl;
			cout << "正在返回注册成功信息" << memid << " " << account << " " << password << endl;

		}
		else  //注册失败操作
		{
			cout << "正在返回注册失败信息" << memid << " " << account << " " << password << endl;
		}
		char sendbuff[48];
		memcpy(sendbuff, (char*)&memid, 4);
		memcpy(sendbuff + 4, (char*)&account, 20);
		memcpy(sendbuff + 24, (char*)&password, 20);
		memcpy(sendbuff + 44, (char*)&playersocket, 4);
		__TCPSERVER->Send(gameserversocket, 10011, sendbuff, 48);

	}


	void OnDBCommand(void* buf)
	{
		DBBuffer* buff = (DBBuffer*)buf;
		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);

		switch (cmd)
		{
		case 1000:
			db_1000(buff);
			break;
		case 1001:
			db_1001(buff);
			break;
		default:
			break;
		}

	}
	

}

