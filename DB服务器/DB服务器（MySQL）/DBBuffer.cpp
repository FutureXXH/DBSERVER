#include "DBBuffer.h"

void db::DBBuffer::begin(const uint16_t cmd)
{
	
	char* p = (char*)&cmd;
	m_Buf[0] = p[0];
	m_Buf[1] = p[1];
	SendE = 2;
}

void db::DBBuffer::s(void* v, const int len)
{
	if (SendE + len < CurLength)
	{
		memcpy_s(&m_Buf[SendE], len, v, len);
		SendE += len;
	}
}

void db::DBBuffer::r(void* v, const int len)
{
	if (ReceE + len >= CurLength)return;
	memcpy(v, &m_Buf[ReceE], len);
	ReceE += len;
}

