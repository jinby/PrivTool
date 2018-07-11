#include "StdAfx.h"
#include "Lock.h" 
//����һ������������� 

Mutex::Mutex() 
{ 
    m_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if ( m_mutex == INVALID_HANDLE_VALUE)
	{
		TRACE_LINE("�����������ʧ�ܣ�������Ϣ��0x%08x��",GetLastError() );
	}
	else 
	{
		TRACE_LINE("�����������ɹ�");
	}
}

//���ٻ�������ͷ���Դ 
Mutex::~Mutex() 
{
	if ( m_mutex != INVALID_HANDLE_VALUE )
	{
		::CloseHandle(m_mutex); 
	}
}

//ȷ��ӵ�л��������̶߳Ա�������Դ�Ķ��Է��� 

void Mutex::Lock() const 
{ 
    DWORD d = WaitForSingleObject(m_mutex, INFINITE); 
}

//�ͷŵ�ǰ�߳�ӵ�еĻ��������ʹ�����߳̿���ӵ�л�����󣬶Ա�������Դ���з��� 
void Mutex::Unlock() const 
{ 	
    ::ReleaseMutex(m_mutex); 
} 


//����C++���ԣ������Զ����� 
CLock::CLock(const IMyLock& m) : m_lock(m) 
{ 
    m_lock.Lock();
	TRACE_LINE("�����ɹ���");
} 

//����C++���ԣ������Զ����� 
CLock::~CLock() 
{ 	
    m_lock.Unlock();
	TRACE_LINE("�����ɹ���");
} 


