
#ifndef __DBConnector_H
#define __DBConnector_H

#include <queue>
#include "SafeThreadPool.h"
#include "DBBuffer.h"
#include <condition_variable>
#include <functional>
#include <atomic>
#include "MySQLConnector.h"


namespace db
{
	typedef std::function<void(DBBuffer*)> CallBack;
	//连接数据库的配置文件
	struct DBXML
	{
		int port;
		char ip[20];
		char username[20];
		char userpass[20];
		char dbname[20];

		DBXML() { memset(this, 0, sizeof(this)); }
	};
	//线程的种类  用户读线程1  用户写线程2  专门处理账号线程3
	enum ThreadType {
		THREAD_USERREAD = 1,
	    THREAD_USERWRITE = 2,
		ACCOUNT = 3
	};
	//一个线程 一个DBconnector类
	class DBConnector
	{
	public:
		DBConnector(DBXML* cfg);
		~DBConnector();

		int StartRun(int id, CallBack fun, CallBack bfun = nullptr);
		void ThreadRun(); //线程运行的开始函数  
		void PushToWorkThread(DBBuffer* buffer); //推送到工作线程  主线程执行该函数
		DBBuffer* PopBuffer(); //获取一个buffer

		inline MysqlConnector* GetMysqlConnector() { return mysql; }
		inline int GetThreadID() { return ThreadID; }
		inline int GetWorkCount()const { return WorkCount; }

	private:
		DBXML* config;
		MysqlConnector* mysql;

		int ThreadID;
		CallBack Callback; //处理数据回调函数
		CallBack beginback;

		std::mutex Mutex;
		std::queue<DBBuffer*> Queue;
		std::queue<DBBuffer*> tempQueue;
		std::condition_variable ConditionVar;

		SafeThreadPool BuffsPool;
		std::atomic<int> WorkCount; 
	};
}

#endif // !__DBConnector_H

