#pragma once
#include <string>
#include <vector>
using namespace std;
std::string format(const char *fmt,...);
bool vectorContain(std::vector<std::string>& vecSrc, const char* key);

void HextoStrH(unsigned char *src, unsigned char *dst, unsigned long Src_Len, int iFlag = 0);
void StrtoHexH(unsigned char *src, unsigned char *dst, unsigned long Src_Len);

std::vector<std::string> Tokenize( std::string src, std::string tok, bool trim=false, std::string null_subst="");
std::vector<std::string> Split( std::string src, std::string delimit, std::string null_subst="", bool bremovelastchar = false);
unsigned char* stringtohex(string src, unsigned long* plen); 
string hextostring(unsigned char* srch, unsigned long len);

string Scm_ToAsc(string src);
string Scm_ToHex(string src);
string Scm_strmidH(string src, int s, int e);
string Scm_Left(string src, int s);
string Scm_Right(string src, int s);
string Scm_xor(string src);
int Scm_strlenH(string src);
unsigned int ToInt(string src);
int ToHex(string src);
bool isHead(string src, const char* funcheader);

string stringtostringH(string src);
string stringHtostring(string src);

string r_cn(string src);
string r_b(string src);
string r_ans(string src);
string w_cn(string src, int len);
string w_ans(string src, int len);
string w_b(string src, int len);
string w_bb(string src, int len);
void Total(string& TotalSrc, string addsrc, string add);
string totlv(string stag, string sdata);
string PadFunc(string src, int iFlag, string sAlgmType = "01");
string timeC();
string w_dec(string src, int len = 0);
string w_hex(string src, int len = 0);
string r_hex(string src);
string r_dec(string src);
string& trim(string &s);
string Xor(string or1, string or2);
string randomHex(int iLen);

class ExceptionTrans
{
public:
	ExceptionTrans(const char* err){ errmsg = err;}
	const char* what(){return errmsg.c_str();}
private:
	string errmsg;
};