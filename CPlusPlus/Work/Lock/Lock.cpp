#include "StdAfx.h"
#include "Lock.h" 
//创建一个匿名互斥对象 

Mutex::Mutex() 
{ 
    m_mutex = ::CreateMutex(NULL, FALSE, NULL);
	if ( m_mutex == INVALID_HANDLE_VALUE)
	{
		TRACE_LINE("建立互斥对象失败，错误信息：0x%08x！",GetLastError() );
	}
	else 
	{
		TRACE_LINE("建立互斥对象成功");
	}
}

//销毁互斥对象，释放资源 
Mutex::~Mutex() 
{
	if ( m_mutex != INVALID_HANDLE_VALUE )
	{
		::CloseHandle(m_mutex); 
	}
}

//确保拥有互斥对象的线程对被保护资源的独自访问 

void Mutex::Lock() const 
{ 
    DWORD d = WaitForSingleObject(m_mutex, INFINITE); 
}

//释放当前线程拥有的互斥对象，以使其它线程可以拥有互斥对象，对被保护资源进行访问 
void Mutex::Unlock() const 
{ 	
    ::ReleaseMutex(m_mutex); 
} 


//利用C++特性，进行自动加锁 
CLock::CLock(const IMyLock& m) : m_lock(m) 
{ 
    m_lock.Lock();
	TRACE_LINE("加锁成功！");
} 

//利用C++特性，进行自动解锁 
CLock::~CLock() 
{ 	
    m_lock.Unlock();
	TRACE_LINE("解锁成功！");
} 


