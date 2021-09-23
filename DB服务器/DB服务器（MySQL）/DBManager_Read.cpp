
#include "DBManager.h"
#include <sstream>
namespace db
{
	void do_3000(DBBuffer* buff, DBConnector* db)
	{
		int memid = 0;
		buff->r(memid);
		auto mysql = db->GetMysqlConnector();
		std::stringstream sql;
		sql << "select * from user_data where ID = " << memid << ";";

		int err = mysql->ExceQuery(sql.str());
		if (err != 0)
		{
			cout << "查询失败" << endl;
		}
		char nick[20];
		memset(nick, 0, 20);
		mysql->read("nick", nick);
		cout << "查询玩家nick ：" << nick << endl;
	}

	void DBManager::Thread_UserRead(DBBuffer* buff)
	{
		auto db = (DBConnector*)buff->GetDB();
		if (db == nullptr)
		{
			cout << "读线程错误" << endl;
			return;
		}

		uint16_t cmd;
		buff->init_r();
		buff->r(cmd);

		switch (cmd)
		{
		case 3000:
			do_3000(buff,db);

			break;
		default:
			break;
		}


	}


}