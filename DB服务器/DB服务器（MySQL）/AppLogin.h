#ifndef __APPLOGIN_H
#define __APPLOGIN_H
#include "User_Data.h"
#include "DBManager.h"
#include "TCPserver.h"
#include <string>

namespace app
{

	extern void onLogin_1000(char* LogAccount, char* LogPassword, SOCKET gameserversocket, unsigned int playersocket, int index);
	extern void onRegister_1001(char* RegAccount, char* RegPassword, SOCKET gameserversocket, unsigned int playersocket, int index);
	extern void on_3000();

	extern void OnDBCommand(void* buf);

}



#endif // !__APPLOGIN_H


