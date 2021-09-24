#include "AppManager.h"


namespace app
{ 
   
    void update()
    {

        
        if (__DBManager == nullptr)return;
        __DBManager->update();
    }


   int run()
   { 
       //initMysql();
       SERVERPRINT_INFO << "正在开启服务器 " << endl;
       SERVERPRINT_INFO << "正在初始化DB " << endl;
       __DBManager = new DBManager();
       __DBManager->InitDB();

       std::this_thread::sleep_for(std::chrono::milliseconds(1000));

       __TCPSERVER = new Tcpserver();
       if (!__TCPSERVER->InitServer(6667))
       {
           SERVERPRINT_INFO << "初始化服务器失败 " << endl;
           return -1;
       }
       if (!__TCPSERVER->StartServer(2))
       {
           SERVERPRINT_INFO << "DB服务器开启失败 " << endl;
           return -2;
       }
       SERVERPRINT_INFO << "DB服务器开启成功 " << endl;
       
      // onLogin_1000();
       //onRegister_1001();
       //on_3000();

       
       while (true)
       {
           update();
           std::this_thread::sleep_for(std::chrono::milliseconds(10));
       }
        return 0;
    }






}