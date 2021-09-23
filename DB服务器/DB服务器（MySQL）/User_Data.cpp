#include "User_Data.h"

namespace app
{

	 std::map<std::string, USER_MEMBER_BASE*>  __AccountsName;
	 std::map<int, USER_MEMBER_BASE*> __AccountsID;



	USER_MEMBER_BASE* Findmember(std::string name)
	{
		auto it = __AccountsName.find(name);
		if (it != __AccountsName.end()) return it->second;


		return nullptr;
	}

	USER_MEMBER_BASE* Findmember(int id)
	{

		auto it = __AccountsID.find(id);
		if (it != __AccountsID.end()) return it->second;

		return nullptr;
	}



}

