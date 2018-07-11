#pragma once
#include "ReaderImpl.h"
#include <string>

class CApduPlus;
std::string& operator<<(std::string& sOut,const CApduPlus& cAdpuPlus);

class CApduPlus : public CApdu
{
public:
	CApduPlus(std::string sIn);
	CApduPlus(unsigned char* pIn, unsigned int dwIn);
	//friend std::string& operator<<(std::string& sOut,const CApduPlus& cAdpuPlus);
};

class CReaderImplPlus
{
public:
	CReaderImplPlus(CReaderImpl* p);
	bool SendCmd(std::string sCmd, std::string& sRsp);
private:
	CReaderImpl* p_;
};