#include "StdAfx.h"
#include "CommonFunc.h"
#include <stdio.h>
#include <algorithm>
#include <time.h>
#include <stdarg.h>

void Total(string& TotalSrc, string addsrc, string add)
{
	TotalSrc.append(addsrc.c_str());
	TotalSrc.append(add);
}

std::string format(const char *fmt,...) 
{ 
	std::string strResult;
	va_list   args;   
	int   len;   
	char*  buffer = new char[10000];   
	va_start(args, fmt);   
	vsprintf(buffer, fmt, args);   
	strResult = buffer;
	delete[] buffer;
	return strResult; 
}

void HextoStrH(unsigned char *src, unsigned char *dst, unsigned long Src_Len, int iFlag)
{
	unsigned long i = 0;
	unsigned char tmp = 0;

	while (i < Src_Len)
	{
		tmp = *src;
		tmp = (tmp >> 4) & 0x0f;
		if (tmp <= 9)
			*dst++ = tmp + '0';
		else if (tmp >= 0x0A && tmp <= 0x0F)
		{
			if (1 == iFlag)
			{
				*dst++ = tmp - 10 + 'a';
			}
			else
			{
				*dst++ = tmp - 10 + 'A';
			}
		}

		//*dst++ = 0x00;

		tmp = *src++;
		tmp = tmp & 0x0f;
		if (tmp <= 9)
			*dst++ = tmp + '0';
		else if (tmp >= 0x0A && tmp <= 0x0F)
		{
			if (1 == iFlag)
			{
				*dst++ = tmp - 10 + 'a';
			}
			else
			{
				*dst++ = tmp - 10 + 'A';
			}
		}

		//*dst++ = 0x00;
		i++;
	}
}

void StrtoHexH(unsigned char *src, unsigned char *dst, unsigned long Src_Len)
{
	unsigned long i = 0;
	unsigned char high = 0;
	unsigned char low = 0;

	while (i < Src_Len)
	{
		high = src[i];
		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;

		low = src[i + 1];
		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;

		dst[i / 2] = high << 4 | low;

		i += 2;
	}
}

typedef std::basic_string<char>::size_type S_T;
static const S_T npos = -1;

std::vector<std::string> Tokenize( std::string src, std::string tok, bool trim/*=false*/, std::string null_subst/*=""*/ )
{
	if( src.empty() || tok.empty() ) 
		throw new ExceptionTrans("tokenize: empty std::string\0");
	//	throw "tokenize: empty std::string\0";  
	
	std::vector<std::string> v;  
	S_T pre_index = 0, index = 0, len = 0;  
	while( (index = src.find_first_of(tok, pre_index)) !=npos )  
	{  
		if( (len = index-pre_index)!=0 )  
			v.push_back(src.substr(pre_index, len));  
		else if(trim==false)  
			v.push_back(null_subst);  
		pre_index = index+1;  
	}  
	std::string endstr = src.substr(pre_index);  
	if( trim==false ) v.push_back( endstr.empty()?null_subst:endstr );  
	else if( !endstr.empty() ) 
		v.push_back(endstr);  
	return v;
}

std::vector<std::string> Split( std::string src, std::string delimit, std::string null_subst/*=""*/, bool bremovelastchar /*= false*/)
{
	std::vector<std::string> v;
	if (src.empty() || delimit.empty())
		//throw new ExceptionTrans("split:empty std::string\0");
		//throw "split:empty std::string\0";    
		return v;


	if ( bremovelastchar )
	{
		if ( src[src.size()-1] != delimit[0])
			throw new ExceptionTrans( format("缺少分隔符 数据:%s",src.c_str()).c_str() ) ;

		src.erase(src.end()-1);
	}

	
	S_T deli_len = delimit.size();  
	long index = npos, last_search_position = 0;  
	while( (index=src.find(delimit, last_search_position))!=npos )  
	{  
		if(index==last_search_position)  
			v.push_back(null_subst);  
		else  
			v.push_back( src.substr(last_search_position, index-last_search_position) );  
		last_search_position = index + deli_len;  
	}  
	std::string last_one = src.substr(last_search_position);  
	v.push_back( last_one.empty()? null_subst:last_one );  
	return v;
}


unsigned char* stringtohex(string src, unsigned long* plen)
{
	unsigned char* psrc = new unsigned char[src.size() + 1];
	unsigned char* pdct = new unsigned char[src.size() / 2];
	memset((void*)psrc, 0, src.size() + 1);
	strcpy((char*)psrc, src.c_str());
	StrtoHexH(psrc, pdct, src.size());
	*plen = src.size() / 2;
	delete[] psrc;
	return pdct;
}

string hextostring(unsigned char* srch, unsigned long len)
{
	char*  pChar = new char[len * 2 + 1];
	memset((void*)pChar, 0, len * 2 + 1);
	HextoStrH(srch, (unsigned char*)pChar, len);
	string srt; //srt.assign((char*)pChar,len*2);
	srt = pChar;
	delete[] pChar;
	return srt;
}

string Scm_ToAsc(string src)
{
	char* psrc = new char[src.size() + 1];
	strcpy(psrc, src.c_str());
	string psrcs = hextostring((unsigned char*)psrc, src.size());
	delete[] psrc;
	return psrcs;
}

string Scm_ToHex(string src)
{
	unsigned long dwlen;
	unsigned char* psrc = stringtohex(src, &dwlen);
	char* pStrH = new char[dwlen + 1];
	memset(pStrH, 0, dwlen + 1);
	memcpy(pStrH, psrc, dwlen);
	string srt; //srt.assign((char*)pStrH, dwlen);
	srt = pStrH;
	delete[] psrc;
	delete[] pStrH;
	return srt;
}

string Scm_strmidH(string src, int s, int e)
{
	return src.substr(s * 2, e * 2);
}

string Scm_Left(string src, int s)
{
	return src.substr(0, s * 2);
}

string Scm_Right(string src, int s)
{
	return src.substr(src.size() - s * 2);
}

string Scm_xor(string src)
{
	unsigned long dwlen;
	unsigned char* psrcpoint = stringtohex(src, &dwlen);

	for (int i = 0; i < 8; i++)
		psrcpoint[i] = psrcpoint[i] ^ psrcpoint[8 + i];
	
	string sres = hextostring(psrcpoint, 8);
	delete[] psrcpoint;
	return sres;
}

string Xor(string or1, string or2)
{
	unsigned long dwlen1;
	unsigned long dwlen2;
	unsigned char* psrcpoint1 = stringtohex(or1, &dwlen1);
	unsigned char* psrcpoint2 = stringtohex(or2, &dwlen2);

	for (int i = 0; i < dwlen1; i++)
		psrcpoint1[i] = psrcpoint1[i] ^ psrcpoint2[i];

	string sres = hextostring(psrcpoint1, dwlen1);
	delete[] psrcpoint1;
	delete[] psrcpoint2;
	return sres;
}

unsigned int ToInt(string src)
{
	return strtol(src.c_str(), 0, 10);
}

int ToHex(string src)
{
	return strtol(src.c_str(), 0, 16);
}

bool isHead(string src, const char* funcheader)
{
	if (strncmp(src.c_str(), funcheader, strlen(funcheader)) == 0)
		return true;
	return false;
}

bool vectorContain(std::vector<std::string>& vecSrc, const char* key)
{
	std::vector<std::string>::iterator it = find(vecSrc.begin(), vecSrc.end(), key);
	return it != vecSrc.end();
}

string stringtostringH(string src)
{
	if( !src.size() )
		return "";
	char* psrc = new char[src.size() + 1];
	strcpy(psrc, src.c_str());
	string sres = hextostring((unsigned char*)psrc, src.size());
	delete[] psrc;
	return sres;
}

string stringHtostring(string src)
{
	if( !src.size() )
		return "";

	unsigned long dwlen;
	unsigned char* psrc = stringtohex(src, &dwlen);
	char* pStrH = new char[dwlen + 1];
	memset(pStrH, 0, dwlen + 1);
	memcpy(pStrH, psrc, dwlen);
	string srt; //srt.assign((char*)pStrH, dwlen);;
	srt = pStrH;
	delete[] psrc; delete[] pStrH;
	return srt;
}

string r_cn(string src)
{
	while (src.size() && src[src.size() - 1] == 'F')
		src.erase(src.size() - 1);
	return src;
}

string r_b(string src)
{
	return src;
}

string r_ans(string src)
{
	src = stringHtostring(src);
	return src;
}

string w_cn(string src, int len)
{
	if (src.size() > len*2 )
		throw new ExceptionTrans("");

	int addlen = len*2 - src.size();
	string addstr; addstr.assign(addlen,'F');
	src.append(addstr);
	return src;
}

string w_ans(string src, int len)
{
	src = stringtostringH(src);
	if (src.size() > len*2 )
		throw new ExceptionTrans("");

	int addlen = len*2 - src.size();

	string addstr; addstr.assign(addlen, '0');
	src.append(addstr);
	return src;
}

string w_b(string src, int len)
{
	if (src.size() % 2)
		src = "0" + src;

	if (src.size() > len*2 )
		throw new ExceptionTrans("");

	int addlen = len * 2 - src.size();

	string addstr; addstr.assign(addlen, 'F');
	src.append(addstr);
	return src;
}

string w_bb(string src, int len)
{
	if (src.size() % 2)
		src = "0" + src;

	if (src.size() > len*2 )
		throw new ExceptionTrans("");

	return src;
}

string totlv(string stag, string sdata)
{
	string res;
	if ( stag.size() )
		res = format("%s%02X%s", stag.c_str(), sdata.size()/2, sdata.c_str());
	else
		res = sdata;

	return res;
}

string PadFunc(string src, int iFlag, string sAlgrim)
{
	int iblock;
	if ( sAlgrim == "01") //DES时填充为8
		iblock = 8;
	else
		iblock = 16;

	string res;
	int ilen = Scm_strlenH(src);
	int iPadlen = ilen % iblock;
	string sPaddata = "80000000000000000000000000000000";
	if (iPadlen != 0)
		res = src + Scm_strmidH(sPaddata, 0, iblock - iPadlen);
	else
		if (iFlag == 1)
			res = src + sPaddata;
		else
			res = src;
	return res;
}

int Scm_strlenH(string src)
{
	return src.size() / 2;
}

std::string timeC()
{
	time_t t = time(0); 
	char tmp[32]={NULL};
	strftime(tmp, sizeof(tmp), "%Y%m%d%H%M%S",localtime(&t)); 
	return string(tmp);
}

string w_dec(string src, int len)
{
	string addstr;

	if (src.size() != 0 )
	{
		char DECIMAL_POINT = '.'; // 欧洲用法为','
		size_t n = src.find(DECIMAL_POINT);
		int nAddZero;
		if (n != string::npos)//是否有小数点呢？
		{
			int nDecLen = src.size() - n - 1;
			if (nDecLen > 2)
			{
				nAddZero = 0;
				src.erase(n + 2);
			}
			else
				nAddZero = 2 - nDecLen;
			src.erase(n, 1);
		}
		else
			nAddZero = 2;

		addstr.assign(nAddZero, '0');
		src.append(addstr);
	}
	
	if ( src.size() % 2)
	{
		src = "0"+ src;
	}

	if ( len != 0 )
	{
		int addlen = len * 2 - src.size();
		if (addlen > 0 )
			addstr.assign(addlen, '0');
		src =  addstr + src;
	}

	return src;
}


string w_hex(string src, int len)
{
	string addstr;

	if (src.size() != 0 )
	{
		char DECIMAL_POINT = '.'; // 欧洲用法为','
		size_t n = src.find(DECIMAL_POINT);
		int nAddZero;
		if (n != string::npos)//是否有小数点呢？
		{
			int nDecLen = src.size() - n - 1;
			if (nDecLen > 2)
			{
				nAddZero = 0;
				src.erase(n + 2);
			}
			else
				nAddZero = 2 - nDecLen;
			src.erase(n, 1);
		}
		else
			nAddZero = 2;

		addstr.assign(nAddZero, '0');
		src.append(addstr);
	}
	src = format("%X", ToInt(src));

	if ( src.size() % 2)
		src = "0"+ src;

	if ( len != 0 )
	{
		int addlen = len * 2 - src.size();
		if (addlen > 0 )
			addstr.assign(addlen, '0');
		src =  addstr + src;
	}
	return src;
}


string r_dec(string src)
{
	src = r_cn(src);
	src = format("%d",ToInt(src));

	if ( src.size() < 3 )
	{
		int addzero = 3-src.size();
		string addstr; 
		addstr.assign(addzero, '0');
		src = addstr + src;
	}		
	src.insert(src.size() - 2, ".");	
	return src;
}


string r_hex(string src)
{
	src = format("%d",ToHex(src));
	
	if ( src.size() < 3 )
	{
		int addzero = 3-src.size();
		string addstr; 
		addstr.assign(addzero, '0');
		src = addstr + src;
	}		
	src.insert(src.size() - 2, ".");	
	return src;
}

std::string& trim(std::string &s)   
{  
	if (s.empty())   
	{  
		return s;  
	}  
	s.erase(0,s.find_first_not_of(" "));  
	s.erase(s.find_last_not_of(" ") + 1);  
	return s;  
}  

std::string randomHex(int iLen)
{
	static unsigned int iseed = 0;
	srand((unsigned int)time(NULL) + iseed);
	iseed = (iseed+1) % 100;

	unsigned char* outString = new unsigned char[iLen];
	for(unsigned long l = 0; l < iLen; l++)
		outString[l] = (unsigned char)rand();

	string srandom = hextostring(outString, iLen);
	delete[] outString;
	return srandom;
}
