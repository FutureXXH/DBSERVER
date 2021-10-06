#ifndef __APPLOGIN_H
#define __APPLOGIN_H
#include "User_Data.h"
#include "DBManager.h"
#include "TCPserver.h"
#include <string>
#include "IDefine.h"

namespace app
{

	extern void onLogin_1000(char* LogAccount, char* LogPassword, SOCKET gameserversocket, SOCKET playersocket);
	extern void onRegister_2000(char* RegAccount, char* RegPassword, SOCKET gameserversocket, SOCKET playersocket);
	extern void on_3000();

	extern void OnDBCommand(void* buf);

}



#endif // !__APPLOGIN_H


