#if !defined(AFX_GLOBALINC_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)
#define AFX_GLOBALINC_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_

typedef ULONG SCARDHANDLE;

void tri_des(BYTE *dat, BYTE *key1, BYTE *key2, BYTE mode);
bool AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);
CString DESCBC(CString &dkey,CString &jmdata,CString &ivs,int mode);
unsigned int strHexTostrASCII(unsigned char*pAscHex,CString strHex);
bool StrToHex(CString str,unsigned char*hex, int & hexlen);
int ByteToInt(unsigned char *bb, int len);
void HexToStr(unsigned char * hex,int hexlen, CString & str);
unsigned int strASCIITostrHex(unsigned char*pAscii,CString& strHex);
void desjs(unsigned char *firstkey,unsigned char *tempkey,unsigned char *reskey);
void desjm(unsigned char *firstkey,unsigned char *tempkey,unsigned char *reskey);

void HexToAscii(unsigned char * pHex, unsigned char * pASSCHex, int nLen);
void HexToAscii(unsigned char * pHex, CString& sHex, int nLen);
void des(BYTE *dat, BYTE *key1, BYTE mode);
void DesJS(BYTE *dat, BYTE *key, BYTE mode, int datalen, int keylen);

bool AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);
void HexToAscii(unsigned char * pHex, unsigned char * pASSCHex, int nLen);
#endif



