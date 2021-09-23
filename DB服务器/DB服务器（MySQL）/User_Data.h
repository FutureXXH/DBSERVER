#ifndef __USER_DATA
#define __USER_DATA
#include <iostream>
#include <map>
#include <string>
namespace app
{
#pragma pack(push,packing)
#pragma pack(1)
	struct USER_MEMBER_BASE
	{
		int ID;
		char username[20];
		char password[20];
	    int8_t state;
		int Createtime;
		int Lastlogintime;



	};

#pragma pack(pop,packing)


	extern std::map<std::string, USER_MEMBER_BASE*>  __AccountsName;
	extern std::map<int, USER_MEMBER_BASE*> __AccountsID;
	


	extern USER_MEMBER_BASE* Findmember(std::string name);
	extern USER_MEMBER_BASE* Findmember(int id);


}


#endif // !__USER_DATA;

