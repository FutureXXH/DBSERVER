


#ifndef __MYSQLCONNECTOR_H
#define __MYSQLCONNECTOR_H
#pragma warning(disable:4996)

#include "share/MySQL/mysql.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#pragma comment(lib,"libmysql.lib")



namespace db
{
	const int len = 32;
	class MysqlConnector
	{
	public:

		MysqlConnector();
		~MysqlConnector();
		//进行增删改查（输入mysql指令 返回0即成功）
		int ExceQuery(const std::string& pData);
		//获取当前查询的一行数据
		void GetQueryFields();
		//进行连接mysql
		bool ConnectMySql(const char* ip, const char* username, const char* passworld, const char* dbname, int port = 3306); 
		//清理mysql数据
		void Clear();    
		//连接与重连
		bool ReConnect();  
		//清理查询结果
		void ClearRes();  
		//读取下一行数据（如果查询有多行数据才成功， 否则返回-1）
		int QueryNext();  
		//清理查询
		void CloseQuery(); 
		//获取行数
		inline int GetQueryRowNum() { return rows; }  
		//获取列数
		inline int GetQueryFieldNum() { return fields; }


		inline MYSQL* GetMysql() { return mysql; }
		inline MYSQL_RES* GetMyRes() { return myres; }
		//获取错误代码
		inline int GetErrorCode()
		{
			if (isGetErrorCode)
			{
				return mysql_errno(mysql);
			}
			return -1000;
		}
		//获取错误信息
		const char* GetErrorStr()
		{
			return mysql_error(mysql);
		}


		//使用read来读取值
		void read(const char* key, uint8_t& value);
		void read(const char* key, int8_t& value);
		void read(const char* key, uint16_t& value);
		void read(const char* key, int16_t& value);
		void read(const char* key, uint32_t& value);
		void read(const char* key, int32_t& value);
		void read(const char* key, int64_t& value);
		void read(const char* key, std::string& value);
		void read(const char* key, char* value);
	private:
		MYSQL* mysql;
		MYSQL_RES* myres;

		int rows;
		int fields;

		char m_ip[len];
		char m_username[len];
		char m_userpassword[len];
		char m_DBname[len];
		uint16_t m_port;

		bool isconnect;
		bool isGetErrorCode;

		std::vector<std::string> filedVec;
		std::map<std::string, std::string> filedsValue;




	};


}
#endif // !__MYSQLCONNECTOR_H