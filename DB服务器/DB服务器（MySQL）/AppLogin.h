#ifndef __APPLOGIN_H
#define __APPLOGIN_H
#include "User_Data.h"
#include "DBManager.h"
#include <string>

namespace app
{

	extern void onLogin_1000();
	extern void onRegister_1001();
	extern void on_3000();

	extern void OnDBCommand(void* buf);

}



#endif // !__APPLOGIN_H


