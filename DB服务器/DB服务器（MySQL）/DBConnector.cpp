#include "DBConnector.h"

db::DBConnector::DBConnector(DBXML* cfg)
{
	mysql = nullptr;
	config = cfg;
}

db::DBConnector::~DBConnector()
{

}

int db::DBConnector::StartRun(int id, CallBack fun, CallBack bfun)
{
	if (mysql != nullptr)return -1;
	mysql = new MysqlConnector();

	
	ThreadID = id;
	Callback = fun;
	beginback = bfun;
	BuffsPool.Init(500, 1024 * 32);

	bool isconnectmysql = mysql->ConnectMySql(config->ip, config->username, config->userpass, config->dbname, config->port);
	if (!isconnectmysql)
	{
		std::cout << "线程id:" << ThreadID << "连接mysql失败" << std::endl;
		return -2;
	}

	std::thread workthread(&DBConnector::ThreadRun, this);
	workthread.detach();
	return 0;
}

void db::DBConnector::ThreadRun()
{
	std::cout << "线程id:" << ThreadID << "开始运行" << std::endl;
	if (beginback != nullptr) beginback(nullptr);
	while (true)
	{
		std::unique_lock<std::mutex> lock(Mutex);
		while (Queue.empty())
		{
			ConditionVar.wait(lock);//等待时释放mutex  唤醒时又锁上
			cout << "线程唤醒" << endl;
		}
		tempQueue.swap(Queue);
		lock.unlock();

		//开始处理队列数据
		while (!tempQueue.empty())
		{
			DBBuffer* buff = tempQueue.front();
			tempQueue.pop();
			cout << "处理数据" << endl;
			Callback(buff);
			WorkCount--;
			BuffsPool.push(buff);//用完后返回回收池 可以重复利用 避免new
		}
	}
	
	std::cout << "线程id:" << ThreadID << "退出" << std::endl;
}

void db::DBConnector::PushToWorkThread(DBBuffer* buffer)
{
	if (buffer == nullptr) return;
	std::unique_lock<std::mutex> lock(Mutex);
	Queue.push(buffer);
	WorkCount++;
	ConditionVar.notify_one(); //通知工作线程 处理数据啦
}

DBBuffer* db::DBConnector::PopBuffer()
{
	auto buff = BuffsPool.Pop();
	buff->Clear();
	buff->SetDB(this);
	return buff;
}
