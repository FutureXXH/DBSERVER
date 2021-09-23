#include "MySQLConnector.h"
namespace db
{ 
db::MysqlConnector::MysqlConnector()
{
    mysql = NULL;
    myres = NULL;
    rows = 0;
    fields = 0;
    isconnect = false;
    isGetErrorCode = false;
}

db::MysqlConnector::~MysqlConnector()
{

    if (mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
    if (myres)
    {
        myres = NULL;
    }
}
//查询
int db::MysqlConnector::ExceQuery(const std::string& pData)
{

    if (pData.size() < 1)
    {
        return -1;
    }
    ClearRes();
    if (ReConnect() == false)
    {
        return - 2;
    }
    int ret = mysql_real_query(mysql, pData.c_str(), pData.size());
    //查询结果如果返回非0 则失败
    if (ret != 0)
    {
        std::cout << "解析指令出现问题:" << GetErrorStr() << std::endl;
        return ret;
    }
        
    if (pData.find("select") == -1 && pData.find("SELECT") == -1) return ret;
    
    myres = mysql_store_result(mysql);
    if (myres == nullptr)
    {
        std::cout << "查询错误" << std::endl;
        return -1;
    }
    fields = mysql_num_fields(myres);
    rows = static_cast<int>(mysql_affected_rows(mysql));
    GetQueryFields();
    std::cout << "查询成功" << std::endl;
    return 0;
}

void MysqlConnector::GetQueryFields()
{
    filedVec.clear();
    filedsValue.clear();
    //mysql 在执行查询一行数据时 结果是按一列一列来输出的   这里的列就是行数据  — -》 |
    MYSQL_ROW  curRow = mysql_fetch_row(myres);//获取当前行的全部值
    if (!curRow) return;
   
   

    MYSQL_FIELD* filed = NULL;
    int num = mysql_num_fields(myres);
    int i = 0;
    while (filed = mysql_fetch_field(myres))    //获取当前列的名称
    {
        auto tempres = curRow[i];
        if (tempres)
        {
            std::string sss(filed->name, filed->name_length);
            filedVec.push_back(sss); //添加列的名称
            filedsValue[sss] = tempres;
        }
        i++;
        if (i >= num) break;
    }


}

//读取下一行数据（如果查询有多行数据才成功， 否则返回-1）
int MysqlConnector::QueryNext()
{
    filedsValue.clear();
    if (myres == nullptr)
    {

        return -1;
    }
    MYSQL_ROW  curRow = mysql_fetch_row(myres);
    if (!curRow) return -2;

    int size = filedVec.size();
    for (int i = 0; i < size; i++)
    {
        auto tempres = curRow[i];
        if (tempres)
        {
            filedsValue[filedVec[i]] = tempres;
        }
    }

    return 0;
}

void MysqlConnector::CloseQuery()
{
    filedsValue.clear();
}


bool db::MysqlConnector::ConnectMySql(const char* ip, const char* username, const char* passworld, const char* dbname, int port)
{
    
    strcpy(m_ip, ip);
    strcpy(m_username, username);
    strcpy(m_userpassword, passworld);
    strcpy(m_DBname, dbname);
   
    m_port = port;
    
    Clear();
    if (ReConnect() == false)
    {
        return false;
    }
    return true;

  
}

void db::MysqlConnector::Clear()
{
    isconnect = false;
    if (mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
    if (myres)
    {
        myres = NULL;
    }



}
//连接与重连
bool db::MysqlConnector::ReConnect()
{
    if (isconnect)
    {
        //如果已经连接就ping一下确认
        isGetErrorCode = true;
        int error = mysql_ping(mysql);
        if (error == 0)
        {
            return true;
        }
        isGetErrorCode = false;
        std::cout << "ip可以连接 但没有成功连接上" << std::endl;
        return false;
    }
    //初次连接初始化
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        return false;
    }
    char value = 1;
    int outtime = 5;
    //设置重连与超时
    mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);
    mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &outtime);
    //格式
    mysql_options(mysql,  MYSQL_SET_CHARSET_NAME, "utf8");
    //开始连接
    auto m  = mysql_real_connect(mysql, m_ip, m_username, m_userpassword, m_DBname, m_port, NULL, CLIENT_MULTI_STATEMENTS);

    if (m = NULL)
    {
        std::cout << "连接错误:" << GetErrorStr() <<std::endl;
        isconnect = false;
        isGetErrorCode = false;
        return false;
    }
    std::cout << "ip连接成功 "<<  std::endl;
    //再次设置以便兼容各种版本mysql
    mysql_options(mysql, MYSQL_OPT_RECONNECT, &value);
    mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &outtime);
    isconnect = true;
    isGetErrorCode = true;
    return true;
}
//清理结果 
void db::MysqlConnector::ClearRes()
{
    bool isnext = (myres != NULL);
    if (myres)
    {
        mysql_free_result(myres);
        myres = NULL;
    }
    if (isnext)
    {
        //查询下一个结果集 如果非0 就是没有了
        while (mysql_next_result(mysql) == 0)
        {
            myres = mysql_store_result(mysql);
            if (myres)
            {
                mysql_free_result(myres);
                myres = NULL;
            }
        }
    }
    else
    {
        do
        {
            myres = mysql_store_result(mysql);
            if (myres)
            {
                mysql_free_result(myres);
                myres = NULL;
            }
        } while (mysql_next_result(mysql) == 0);
    }
    myres = NULL;
}











void MysqlConnector::read(const char* key, uint8_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (uint8_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, int8_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (int8_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, uint16_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (uint16_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, int16_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (int16_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, uint32_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (uint32_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, int32_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (int32_t)atoi(it->second.data());
    }
}

void MysqlConnector::read(const char* key, int64_t& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = (int64_t)_atoi64(it->second.data());
    }
}

void MysqlConnector::read(const char* key, std::string& value)
{
    auto it = filedsValue.find(key);
    if (it != filedsValue.end())
    {
        value = it->second.data();
    }
}

void MysqlConnector::read(const char* key, char* value)
{
    auto iter = filedsValue.find(key);
    if (iter != filedsValue.end())
    {
        strcpy(value, iter->second.data());
    }

}




}