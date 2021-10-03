#ifndef __DBBUFFER_H
#define __DBBUFFER_H

#include <string>

#include <iostream>

namespace db
{
	class DBBuffer
	{
	public:
		int CurLength;
		int OldLength;
		int SendE;
		int ReceE;
		char* m_Buf; //buf 缓冲
		void* db;//数据库连接器
	
		DBBuffer(int size)
		{
			SendE = 0;
			ReceE = 0;
			OldLength = size;
			CurLength = size;
			m_Buf = new char[CurLength];
			memset(m_Buf, 0, CurLength);
			db = nullptr;
		}

		~DBBuffer()
		{
			if (m_Buf != NULL)
			{
				delete[] m_Buf;
			}
		}

		inline void* GetDB() { return db; }
		inline void SetDB(void* setDB) { db = setDB; }
		inline void Clear()
		{
			db = nullptr;
			SendE = 0;
			ReceE = 0;
			if (CurLength != OldLength) // 回到最初的大小
			{
				if (m_Buf != NULL) delete[] m_Buf;
				CurLength = OldLength;
				m_Buf = new char[OldLength];
			}
			memset(m_Buf, 0, OldLength);
		}
		inline void append(int size) //改变大小
		{
			if (size <= OldLength) return;//小于最初大小就不变
			if (m_Buf != NULL) delete[] m_Buf;
			CurLength = size;
			m_Buf = new char[size];
			memset(m_Buf, 0, CurLength);
		}
		//写入buf
		void begin(const uint16_t cmd);
		template<class T>
		void s(const T v);

		
		void s(void * v,const int len);

		//读取buf
		inline void  init_r() { ReceE = 0; }
		template<class T>
		void r(T &v);

		void r(void* v, const int len);

	};


	template<class T>
	 void DBBuffer::s(const T v)
	{
		int size = sizeof(T);
		if (SendE + size < CurLength)
		{
			//char* p = (char*)&v;
			memcpy(m_Buf + SendE, (char*)&v, size);
			/*for (int i = 0; i < size; i++)
			{
				m_Buf[SendE+i] = p[i];
			}*/
			SendE += size;
			//std::cout << "s:  " << SendE << "|" << size << "|" << CurLength  << "   "  << v << std::endl;
		}
	}

	template<class T>
	 void DBBuffer::r(T& v)
	{
		int size = sizeof(T);
		if (ReceE + size >= CurLength) return;
		memcpy(&v, m_Buf+ReceE, size);
		ReceE += size;

		//std::cout << "r:  " << ReceE << "|" << size << "|" << CurLength << "   " << v<< std::endl;
	}

}




#endif // !__DBBUFFER_H



