#ifndef __SAFETHREADPOOL_H
#define __SAFETHREADPOOL_H

#include "DBBuffer.h"
#include <concurrent_queue.h>
#include <atomic>
using namespace Concurrency;
using namespace std;
using namespace db;


//线程安全内存回收池  用来推送与回收buff 长期运行时减少new的次数 从而提高效率
class SafeThreadPool
{
private:
	concurrent_queue<DBBuffer*> buffs;
	int BufferSize;
	int MaxCount;
	atomic<int> CurCount;
public:
	SafeThreadPool() {};
	~SafeThreadPool()
	{
		while (!buffs.empty())
		{
			DBBuffer* buff = NULL;
			buffs.try_pop(buff);
			if (buff == NULL)delete buff;
			
		}
	}

	inline void Init(int num, int size)
	{
		BufferSize = size;
		MaxCount = num;
		CurCount = 0;
	}
	inline int GetCount()
	{
		return CurCount;
	}

	inline DBBuffer* Pop()
	{
		DBBuffer* buff = nullptr;
		if (buffs.empty())//如果buffs已经空了 就单独new个出来
		{
			buff = new DBBuffer(BufferSize);
			buff->Clear();
		}
		else
		{
			buffs.try_pop(buff);
			if (buff == nullptr)
			{
				buff = new DBBuffer(BufferSize);
				buff->Clear();
			}
			else
			{
				CurCount--;
			}
		}
		return buff;
	}

	inline void push(DBBuffer* buff)
	{
		if (buff == nullptr)return;
		if (CurCount > MaxCount)
		{
			delete buff;
			return;
		}
		buff->Clear();
		buffs.push(buff);
		CurCount++;

	
	}


};



#endif // !__SAFETHREADPOOL_H

