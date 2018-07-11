#include "StdAfx.h"
#include "ReaderImpl.h"
#include <algorithm>
#include <Windows.h>
#include <winscard.h>
#pragma comment(lib,"winscard.lib")

// --------------- CApdu ----------------------
CApdu::CApdu(unsigned char* pIn, unsigned int dwIn)
{
	arrIn_.assign(pIn, pIn+dwIn);
}

unsigned long SW(const CApdu& apdu)
{
	unsigned int usize = apdu.arrOut_.size();
	if ( usize < 2 )
		return 0xFFFF;
	
	return (apdu.arrOut_[usize-2]*0x100+apdu.arrOut_[usize-1] );
}

void RSP(const CApdu& apdu, unsigned char* pOut, unsigned long& dwOut)
{
	dwOut = apdu.arrOut_.size()-2;
	std::copy(apdu.arrOut_.begin(), apdu.arrOut_.begin()+dwOut, pOut);
}

bool CheckSW_Norm(unsigned long dwsw)
{
	return dwsw == 0x9000||( (dwsw & 0x6C00) == 0x6100) || ( (dwsw & 0x6C00) == 0x6C00);
}

bool CheckSW_Abso(unsigned long dwsw)
{
	return dwsw == 0x9000;
}

// --------------- CReaderImpl ----------------

CReaderImpl::CReaderImpl(CReaderConT* pConT):pConT_(pConT)
{

}


void CReaderImpl::SetReader(const char* readername)
{
	sreadername_ = readername;
}

bool CReaderImpl::Open()
{
	Close();

	if (sreadername_.size() == 0 )
		return false;

	LONG lRet = SCardConnect(pConT_->hConText(), sreadername_.c_str(), SCARD_SHARE_SHARED,
		SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
		(ULONG*)&hHandle_, &dwProtocol_);
	if (SCARD_S_SUCCESS != lRet){
		return false;
	}

	if (hHandle_ == 0) {
		return false;
	}
	
	return true;
}

bool CReaderImpl::Close()
{
	long lRet = 0;
	if ( hHandle_  )
	{
		lRet = SCardDisconnect(hHandle_,SCARD_LEAVE_CARD);
		if (SCARD_S_SUCCESS != lRet){
			return false;
		}
	}

	hHandle_= 0;
	return true;
}

//CardStatus(SCARD_STATE_PRESENT)
//SCARD_STATE_EMPTY   - 无卡
//SCARD_STATE_PRESENT - 有卡

//       TY         SDI
//无卡 0x00010222 0x00020012
//有卡 0x00010422 --
//上电 0x00010122 0x00030122
unsigned long CReaderImpl::CardStatus(unsigned long dwStatus)
{
	long lRet = SCARD_S_SUCCESS;

	SCARD_READERSTATE rs[2];
	memset(&rs[1], 0, sizeof(SCARD_READERSTATE));
	memset(&rs[1], 0, sizeof(SCARD_READERSTATE));
	rs[0].szReader = (char*)sreadername_.c_str();
	rs[0].dwCurrentState = dwStatus;
	rs[0].cbAtr = 36;

	lRet = SCardGetStatusChange(pConT_->hConText(),1000, rs ,1);
	if(SCARD_S_SUCCESS==lRet)
	{
		return rs[0].dwEventState;
	}

	return 0;
}

bool CardStatus_Exist(unsigned long dwStatus)
{
	return ( dwStatus & SCARD_STATE_PRESENT ) && ( dwStatus & 0x00000F00 );
}

bool CardStatus_UnPower(unsigned long dwStatus)
{
	return ( dwStatus & SCARD_STATE_PRESENT ) && ( dwStatus & SCARD_STATE_UNPOWERED );
}

bool CardStatus_Valid(unsigned long dwStatus)
{
	return ( dwStatus & SCARD_STATE_PRESENT ) && ( dwStatus & SCARD_STATE_INUSE );
}

bool CReaderImpl::Reset(CApdu& rst)
{
	if ( !Open() )
		return false;

	long lRet = 0;

	unsigned char pbReset[255];
	unsigned long dwReset = 255;
	unsigned long dwState;
	lRet = SCardState(hHandle_,&dwState,&dwProtocol_,pbReset,&dwReset);
	if (lRet != SCARD_S_SUCCESS)
		return false;

	rst.arrOut_.assign(pbReset, pbReset+dwReset);
	return true;
}

bool CReaderImpl::SendCmd(CApdu& apdu)
{
	SCARD_IO_REQUEST  ScardIoRequest;
	int nStatus;
	DWORD RespLen;
	ScardIoRequest.dwProtocol = dwProtocol_;
	ScardIoRequest.cbPciLength = sizeof(ScardIoRequest);
	unsigned char lpRes[260];
	RespLen=260;
	CApdu adpuw(&(apdu.arrIn_.at(0)), apdu.arrIn_.size());

	DWORD lRet = SCardTransmit(hHandle_, &ScardIoRequest, 
		&(adpuw.arrIn_.at(0)), adpuw.arrIn_.size(), NULL, 
		lpRes, &RespLen);

	if ((lRet == SCARD_S_SUCCESS) && (RespLen >1))
	{
		apdu.arrOut_.assign(lpRes, lpRes+RespLen);
		return true;
	}
	else
	{
		return false;
		//apdu.arrOut_.clear();
		//apdu.arrOut_.push_back(0x84);
		//apdu.arrOut_.push_back(0x04);
	}

	//return CheckSW_Norm(SW(apdu));
}

bool CReaderImpl::SendCmdAuto(CApdu& apdu)
{
	CApdu adpuw( &(apdu.arrIn_.at(0)), apdu.arrIn_.size() );
	unsigned long dwSW;

	while(1)
	{
		if ( !SendCmd(adpuw) )
			return false;

		apdu.arrOut_ = adpuw.arrOut_;
		dwSW = SW(apdu);
		if ( dwSW == 0x9000 )
		{
			break;
		}
		else if ( dwSW >> 8 == 0x61)
		{
			adpuw.arrIn_.clear();
			adpuw.arrIn_.assign(5, 0x00);
			adpuw.arrIn_[1] = 0xC0;
			adpuw.arrIn_[4] = dwSW &0x00FF;

			continue;
		}
		else if ( dwSW >> 8 == 0x6C )
		{
			adpuw.arrIn_[4] = dwSW&0x00FF;
			continue;
		}

		return false;
	}
	
	//return CheckSW_Norm(SW(apdu));
	return true;
}


//---------------  CReaderConT -------------------

CReaderConT::CReaderConT(long hScardHandle)
{
	hContext_ = hScardHandle;
}


CReaderConT::~CReaderConT()
{
	while ( vecImpl_.size() )
	{
		delete vecImpl_.at(0);
		vecImpl_.erase(vecImpl_.begin());
	}
}

bool CReaderConT::EstablishContext()
{
	SCARDCONTEXT hContext;
	DWORD lRet = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	if (lRet != SCARD_S_SUCCESS){
		hContext_ = 0;
		return false;
	}
	hContext_ = hContext;
	return true;
}

bool CReaderConT::ReaderNameVec(std::vector<std::string>& vecReaderName)
{
	unsigned long dwReadersLength;
	unsigned long lRet;
	lRet = SCardListReaders(hContext_, NULL, NULL, &dwReadersLength);
	if (lRet != SCARD_S_SUCCESS){
		return false;
	}

	char* szListReader = new char[dwReadersLength + 100];
	memset(szListReader, 0, dwReadersLength + 100);
	lRet = SCardListReaders(hContext_, NULL, szListReader, &dwReadersLength);
	if (lRet != SCARD_S_SUCCESS){
		delete[] szListReader;
		return false;
	}

	char* pPos = szListReader;
	while ((*pPos) != 0)
	{
		vecReaderName.push_back(std::string(pPos));
		pPos += ( strlen(pPos) + 1 );
	}

	delete[]szListReader;
	return vecReaderName.size() > 0;
}

bool CReaderConT::ReleaseContext()
{
	DWORD lRet = SCardReleaseContext(hContext_);
	if (lRet != SCARD_S_SUCCESS){
		return false;
	}
	return true;
}

CReaderImpl* CReaderConT::GetReaderImpl(const char* readername)
{
	CReaderImpl* pImpl = new CReaderImpl(this);
	pImpl->SetReader(readername);
	vecImpl_.push_back(pImpl);
	return pImpl;
}

CReaderConT CReaderConT::t_(0);

CReaderConT* CReaderConT::GetInstance()
{
	return &CReaderConT::t_;
}
