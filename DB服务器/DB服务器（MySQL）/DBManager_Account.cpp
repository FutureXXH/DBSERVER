#include "DBManager.h"
#include <sstream>
#include "User_Data.h"
#include <chrono>
using namespace std::chrono;


namespace db
{


void DBManager::Thread_beginAccount()
{

	auto start = std::chrono::steady_clock::now();

	auto mysql = DBAccount->GetMysqlConnector();
	stringstream sql;
	sql << "select * from testaccount.account;";
	int error = mysql->ExceQuery(sql.str());
	if (error != 0)
	{
		SERVERPRINT_ERROR << "[thread]" << DBAccount->GetThreadID() << "beginaccount error" << mysql->GetErrorCode() << mysql->GetErrorStr() << endl;

		return;
	}
	int row = mysql->GetQueryRowNum();
	for (int i = 0; i < row; i++)
	{
		app::USER_MEMBER_BASE *user = new app::USER_MEMBER_BASE();
		mysql->read("ID", user->ID);
		mysql->read("state", user->state);
		mysql->read("username", user->username);
		mysql->read("password", user->password);
		mysql->read("createtime", user->Createtime);
		mysql->read("logintime", user->Lastlogintime);
		app::__AccountsName.insert(std::make_pair(user->username, user));
		app::__AccountsID.insert(std::make_pair(user->ID, user));

		  
		cout << user->ID << " " << user->username << " " << user->password << endl;
		mysql->QueryNext();

	}
	auto current = std::chrono::steady_clock::now();
	auto duration = duration_cast<milliseconds>(current - start);
	SERVERPRINT_INFO << "加载用户数据完成,用户账户大小" << app::__AccountsID.size() << "消耗时间" << duration.count() << endl;

}

void updateLoignTime(DBBuffer* buff,DBConnector* db)
{
	//cout << "更新时间" << endl;
	int memid;
	SOCKET gameserversock;
	SOCKET playersock;
	buff->r(memid);
	buff->r(gameserversock);
	buff->r(playersock);

	std::cout << memid << "|" << gameserversock << "|" << playersock << std::endl;



	int curtime = time(NULL);
	stringstream sql;
	sql << "update testaccount.account set logintime = " << curtime << " where ID =" << memid << ";";
	auto mysql = db->GetMysqlConnector();
	int ret = mysql->ExceQuery(sql.str());
	if (ret != 0)
	{
		SERVERPRINT_WARNING << "loign 1000 failed:" << mysql->GetErrorStr() << " " << ret << endl;
		return;
	}
	//cout << "更新时间成功 " << memid << endl;

	auto buf2 = __DBManager->Popbuff();
	buf2->begin(1000);
	buf2->s(memid);
	buf2->s(gameserversock);
	buf2->s(playersock);
	__DBManager->PushToMainThread(buf2);
}


void RegAccount(DBBuffer* buff, DBConnector* db)
{
	
	SOCKET gameserversocket;
	unsigned int playersocket;
	int index;
	char account[20];
	char password[20];
	buff->r(account,20);
	buff->r(password, 20);
	buff->r(gameserversocket);
	buff->r(playersocket);
	buff->r(index);

	


	int curtime = time(NULL);
	int state = 1;
	stringstream sql;
	sql << "insert testaccount.account(username,password,createtime,logintime,state) value( " << "'" << account << "','" << password << "'," << curtime << "," << curtime << "," << state << ");";
	
	
	
	
	auto mysql = db->GetMysqlConnector();
	int ret = mysql->ExceQuery(sql.str());
	if (ret != 0)
	{
		cout << "reg 1001 failed:" << mysql->GetErrorStr() << " " << ret << endl;
		auto buf2 = __DBManager->Popbuff();
		int errid = -2;
		buf2->begin(1001);
		buf2->s(errid);
		buf2->s(account, 20);
		buf2->s(password, 20);
		buf2->s(gameserversocket);
		buf2->s(playersocket);
		buf2->s(index);
		__DBManager->PushToMainThread(buf2);
		return;
	}


	SERVERPRINT_INFO << "注册成功" << account << endl;

	int memid = mysql->GetMysql()->insert_id;

	auto buf2 = __DBManager->Popbuff();
	buf2->begin(1001);
	buf2->s(memid);
	buf2->s(account, 20);
	buf2->s(password, 20);
	buf2->s(gameserversocket);
	buf2->s(playersocket);
	buf2->s(index);
	__DBManager->PushToMainThread(buf2);

}

void DBManager::Thread_UserAccount(DBBuffer* buff)
{
	uint16_t cmd;
	buff->init_r();
	buff->r(cmd);
	cout << " recv " << cmd << endl;;
	switch (cmd)
	{
	case 1000:
		updateLoignTime(buff, DBAccount);
		break;
	case 1001:
		RegAccount(buff, DBAccount);
		break;

	default:
		break;
	}

}



}