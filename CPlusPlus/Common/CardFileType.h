#pragma once

#include <string>
#include <vector>
using namespace std;

struct itemType{
public:
	string id;
	int len;
	string type;
	int itemnum; //B-TLV有效
	int istartpos; //B-TLV有效
	vector<itemType*> pvecBtlvItems;
	~itemType();
};

class FileType{
public:
	const char* sfilename;
	int itype; // 0-变长,1-记录,2-定长
	const char* stypeformat;
	int irecodenum;
	
	vector<itemType*> pvecItems;
	
	//FileType(const char* sName, int iType,const char* sFormat);
	FileType(const char* sName, int iType,const char* sFormat, int iRecodenum = 0);
	~FileType();
	itemType* GetTypeByID(string id);
	bool FindId(string id);
};

class CCardFileType
{
public:
	CCardFileType(void);
	~CCardFileType(void);

	itemType* GetTypeByID(string filename, string id);
	static CCardFileType static_scardfiletype;
	static CCardFileType* GetInistance();
	vector<FileType*>* GetVecFileType();
	CCardFileType* SetCardVersion(int icv);
	bool FindId(string id);

public:
	vector<FileType*> vecFileType_v1;
	vector<FileType*> vecFileType_v2;

	int icardversion;
	
};


FileType* GetFileTypeByID(string filename);

string readByType(itemType* pitemtype, string src);
string readByTypeBtlv(vector<itemType*>& vecitemTypes, string src);
std::string GeneralReadByID(FileType* type, string src, string id);

string removeFirstItem(vector<string>& vecSrc);
string writeByType(itemType* pitemtype, vector<string>& vecSrc);
string writeByTypeBtlv(vector<itemType*>& vecitemTypes,vector<string>& vecitemData);
std::string GeneralWriteByID(FileType* type, vector<string>& vecSrc, string id);