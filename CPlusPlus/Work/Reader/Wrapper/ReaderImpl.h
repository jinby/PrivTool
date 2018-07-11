#pragma once
#include <string>
#include <vector>

struct CApdu
{
	std::vector<unsigned char> arrIn_;
	std::vector<unsigned char> arrOut_;
	CApdu(){};
	CApdu(unsigned char* pIn, unsigned int dwIn);
};

unsigned long SW(const CApdu& apdu);
void RSP(const CApdu& apdu, unsigned char* pOut, unsigned long& dwOut);
bool CheckSW_Norm(unsigned long dwsw);
bool CheckSW_Abso(unsigned long dwsw);

bool CardStatus_Exist(unsigned long dwStatus);
bool CardStatus_UnPower(unsigned long dwStatus);
bool CardStatus_Valid(unsigned long dwStatus);

class CReaderConT;
class CReaderImpl
{
public:
	CReaderImpl(CReaderConT* pConT);
public:
	void SetReader(const char* readername);
	bool Open();
	bool Close();
	unsigned long CardStatus(unsigned long dwStatus = 0);

	bool Reset(CApdu& rst);
	bool SendCmd(CApdu& apdu);
	bool SendCmdAuto(CApdu& apdu);
private:
	CReaderConT* pConT_;
	std::string sreadername_;
	long hHandle_;
	unsigned long dwProtocol_;
};

class CReaderConT
{
public:
	CReaderConT(long hScardHandle = 0);
	~CReaderConT();

public:
	void ResetContext(long hContext);
	bool EstablishContext();
	bool ReleaseContext();
	bool ReaderNameVec(std::vector<std::string>& vecReaderName);
	long hConText(){return hContext_;}
public: 

	CReaderImpl* GetReaderImpl(const char* readername = 0);
private:
	long hContext_;
	std::vector<CReaderImpl*> vecImpl_;
public:
	static CReaderConT* GetInstance();
	static CReaderConT t_;
};

