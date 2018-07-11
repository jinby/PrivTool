#pragma once
#include <string>
using namespace std;
class CFingerBoard
{
public:
	CFingerBoard(void);
	~CFingerBoard(void);
	
	void Open();
	void Close();
	string getPin();
	string getNewPin();
	string findCom();
	string openCom();

public:
	string _scomname;
	HANDLE _hcom;
};

BOOL FullReadFile(HANDLE hFile, /* handle of file to read */ LPBYTE lpBuffer, /* pointer to buffer that receives data */ DWORD nNumberOfBytesToRead, /* number of bytes to read */ LPOVERLAPPED lpOverlapped, int nTryCount = 3) /* pointer to structure for data */;
BOOL FullWriteFile(HANDLE hFile, /* handle to file to write to */ LPBYTE lpBuffer, /* pointer to data to write to file */ DWORD nNumberOfBytesToWrite, /* number of bytes to write */ LPOVERLAPPED lpOverlapped) /* pointer to structure for overlapped I/O */;
void SetupCom(HANDLE hcom);
void ClearCom(HANDLE hcom);
void SendCom(HANDLE hcom, PBYTE pbInput, int iInput);
void RecvCom(HANDLE hcom, PBYTE pbOutput, int iOutput);
void RecvComSF(HANDLE hcom, PBYTE pbOutput, int& iOutput);
bool isStartByte(BYTE bOutput);
bool isFinishByte(BYTE bOutput);
string getMidPin(PBYTE pbInput, int iInput);
void resetpin(HANDLE hcom);
string getpin(HANDLE hcom);