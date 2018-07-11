#include "StdAfx.h"
#include "ReaderImplStr.h"
#include "CommonFunc.h"

CApduPlus::CApduPlus(unsigned char* pIn, unsigned int dwIn) : CApdu(pIn, dwIn)
{
}

CApduPlus::CApduPlus(std::string sIn)
{
	unsigned long dwIn;
	unsigned char* pIn =  stringtohex(sIn, &dwIn);
	this->CApduPlus::CApduPlus(pIn, dwIn);
	delete[] pIn;
}

string& operator<<(string& sOut,const CApduPlus& cAdpuPlus)
{
	unsigned char abyRsp[255] = {0};
	unsigned long dwRsp = 255;
	RSP((const CApdu&)cAdpuPlus, abyRsp, dwRsp);
	
	sOut = hextostring(abyRsp, dwRsp);
	return sOut;
}

CReaderImplPlus::CReaderImplPlus(CReaderImpl* p) : p_ (p)
{
}

bool CReaderImplPlus::SendCmd(std::string sCmd, std::string& sRsp)
{
	TRACE_LINE("CMD => %s\n", sCmd.c_str());
	CApduPlus cApdu(sCmd);
	
	bool bRes = p_->SendCmdAuto(cApdu);
	sRsp << cApdu;

	TRACE_LINE("RSP <= %s%04X\n", sRsp.c_str(), SW(cApdu));
	return bRes;
}


