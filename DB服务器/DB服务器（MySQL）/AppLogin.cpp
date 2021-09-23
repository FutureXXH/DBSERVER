#include "AppLogin.h"
#include "DBManager.h"

namespace app
{

	//登录
	void onLogin_1000()
	{
		std::string account = "xu";
		std::string password = "6666";

		auto mem = Findmember(account);
		if (mem == nullptr)
		{
			std::cout << "登录查找为null" << std::endl;
			return;
		}

		if (strcmp(password.c_str(), mem->password) == 0)
		{
			std::cout << "登录成功" << std::endl;
			std::cout << __DBManager << std::endl;

			mem->Lastlogintime = time(NULL);
			auto db = __DBManager->DBAccount;
			auto buff = db->PopBuffer();
			buff->begin(1000);
			buff->s(mem->ID);


			db->PushToWorkThread(buff);




			return;
			
		}

		std::cout << "登录失败" << std::endl;


	}

	//注册
	void onRegister_1001()
	{
		std::string account = "xu2";
		std::string password = "6666";
		auto mem = Findmember(account);
		if (mem != nullptr)
		{
			std::cout << "账号已存在" << std::endl;
			return;
		}

		auto db = __DBManager->DBAccount;
		auto buff = db->PopBuffer();
		buff->begin(1001);
		buff->s((char*)account.c_str(),20);
		buff->s((char*)password.c_str(), 20);

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
		buff->r(memid);
		cout << "db1000 successful" << memid << endl;
	}

	void db_1001(DBBuffer* buff)
	{
		int memid;
		char account[20];
		char password[20];
		memset(account, 0, 20);
		memset(password, 0, 20);
		buff->r(memid);
		buff->r(account);
		buff->r(password);
		//插入内存中
		app::USER_MEMBER_BASE* mem = new app::USER_MEMBER_BASE();
		mem->ID = memid;
		memcpy(mem->username, account, 20);
		memcpy(mem->password, password, 20);
		mem->Createtime = time(NULL);
		mem->Lastlogintime = time(NULL);

		__AccountsID.insert(make_pair(memid,mem));

		cout << "db1001 successful" << memid  <<" " << account  <<" " << password << endl;
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

