#pragma once

#ifndef __DBMANAGER_H
#define __DBMANAGER_H
#pragma warning(disable:4996)
#include "DBBuffer.h"
#include "DBConnector.h"
#include "AppLogin.h"
#include "IDefine.h"
namespace db
{
	class DBManager
	{
	public:
		DBManager();
		~DBManager();

		DBXML* _GameDBXML;
		DBXML* _AccountXML;

		concurrent_queue<DBBuffer*> _logicBuffs;
		SafeThreadPool _PoolBuffs;

		std::vector<DBConnector*> DBRead; //读数据线程对象池
		std::vector<DBConnector*> DBWrite;//写数据线程对象池
		DBConnector* DBAccount; //账户线程对象
		//查找当前工作最轻的线程对象
		DBConnector* GetDBSource(int type, int threadid = 0);
		//获取一个buff
		DBBuffer* Popbuff();

		void InitDB();
		void update();
		void PushToMainThread(DBBuffer* buffer);

		void Thread_beginAccount();
		void Thread_UserRead(DBBuffer* buff);
		void Thread_UserWirte(DBBuffer* buff);
		void Thread_UserAccount(DBBuffer* buff);
	};



	extern DBManager* __DBManager;

}





#endif // !__DBMANAGER_H
