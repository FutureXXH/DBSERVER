#include "DBManager.h"

 namespace db
 { 
DBManager* __DBManager = nullptr;


DBManager::DBManager()
{
    DBAccount = nullptr;
    _GameDBXML = nullptr;
    _AccountXML = nullptr;

}

DBManager::~DBManager()
{
    if (_GameDBXML != nullptr) delete _GameDBXML;
    if (_AccountXML != nullptr) delete _AccountXML;
}

DBConnector* DBManager::GetDBSource(int type, int threadid)
{
    
    if (type == THREAD_USERREAD)
    {
        auto it = DBRead.begin();
        db::DBConnector* db = *it;
        if (db->GetWorkCount() == 0) return db;
        for (; it < DBRead.end(); it++)
        {
            db::DBConnector* tempdbp = *it;
            if (tempdbp->GetWorkCount() < db->GetWorkCount())
            {
                db = tempdbp;
            }

        }
        return db;
    }
    else if (type == THREAD_USERWRITE)
    {
        auto it = DBWrite.begin();
        db::DBConnector* db = *it;
        if (db->GetWorkCount() == 0) return db;
        for (; it < DBWrite.end(); it++)
        {
            db::DBConnector* tempdbp = *it;
            if (tempdbp->GetWorkCount() < db->GetWorkCount())
            {
                db = tempdbp;
            }

        }
        return db;
    }


    return nullptr;
}

DBBuffer* DBManager::Popbuff()
{
    auto buff = _PoolBuffs.Pop();
    buff->Clear();
    buff->SetDB(nullptr);
    return buff;
}

void DBManager::InitDB()
{

    //加载库
    mysql_library_init(0, NULL, NULL);

    //初始化回收处
    _PoolBuffs.Init(1000, 1024 * 32);
    //初始化xml

    //游戏数据库配置
    _GameDBXML = new DBXML();
    sprintf(_GameDBXML->dbname, "testuserdata");
    sprintf(_GameDBXML->ip, "127.0.0.1");
    _GameDBXML->port = 3306;
    sprintf(_GameDBXML->username, "test1");
    sprintf(_GameDBXML->userpass, "111111");

    //账号数据库配置
    _AccountXML = new DBXML();
    sprintf(_AccountXML->dbname, "testaccount");
    sprintf(_AccountXML->ip, "127.0.0.1");
    _AccountXML->port = 3306;
    sprintf(_AccountXML->username, "test1");
    sprintf(_AccountXML->userpass, "11111");

    auto readfun = std::bind(&DBManager::Thread_UserRead, this, std::placeholders::_1);
    auto writefun = std::bind(&DBManager::Thread_UserWirte, this, std::placeholders::_1);
    auto accountfun = std::bind(&DBManager::Thread_UserAccount, this, std::placeholders::_1);
    auto beginaccountfun = std::bind(&DBManager::Thread_beginAccount, this);

    //开始的读写线程id
    int r_id = 1000;
    int w_id = 2000;
    DBRead.reserve(3);
    DBWrite.reserve(3);
    for (int i = 0; i < 3; i++)
    {
        auto d = new DBConnector(_GameDBXML);
        DBRead.emplace_back(d);
        d->StartRun(r_id + i,readfun,nullptr);
    }
    for (int i = 0; i < 3; i++)
    {
        auto d = new DBConnector(_GameDBXML);
        DBWrite.emplace_back(d);
        d->StartRun(r_id + i, writefun, nullptr);
    }
    //账号线程
    DBAccount = new DBConnector(_AccountXML);
    DBAccount->StartRun(3000,accountfun, beginaccountfun);
}

void DBManager::update()
{
    while (!_logicBuffs.empty())
    {
        DBBuffer* buff = nullptr;
        _logicBuffs.try_pop(buff);
        if (buff == nullptr)break;

        //逻辑处理==========(主线程逻辑处理)
        app::OnDBCommand(buff);
        //逻辑处理完毕后====
        _PoolBuffs.push(buff);

    }



}

void db::DBManager::PushToMainThread(DBBuffer* buffer)
{
    if (buffer == nullptr) return;
    _logicBuffs.push(buffer);
}

}