#include "stdafx.h"
#include "CardFileType.h"
#include "CommonFunc.h"
#include "ExceptionEx.hpp"
#include "ReaderUtil.h"


extern CReaderUtil rdutil;
//遍历两层来分解
void generalVecItem(vector<itemType*>& vecIter, string src, int iDepth)
{
	//OutputDebugString(format("%s\n", src.c_str()).c_str());
	//解析后每项为"01,cn,10","40,B-TLV(57_an_03^58_cn_03^59_an_28),34","-,b,55"
	string sSplitStr;
	if ( iDepth > 0 )
		sSplitStr = "^";
	else
		sSplitStr = "|";
	vector<string> vecItemAllString =  Split(src, sSplitStr);
	vector<string>::iterator iter = vecItemAllString.begin();

	while(iter!=vecItemAllString.end())
	{
		if ( iDepth > 0 )
			sSplitStr = "_";
		else
			sSplitStr = ",";
		vector<string> vecItemString = Split(*iter, sSplitStr);
		//第一项为id ,第二项为type,第三项为长度
		itemType* pitemtype = new itemType;
		pitemtype->id = vecItemString[0];
		pitemtype->len = ToHex(vecItemString[2]);
		pitemtype->type = vecItemString[1];
		if ( pitemtype->type.length() > 3 )  //为B-TLV()
		{
			string sbtlv = pitemtype->type.substr(6,  pitemtype->type.length() - 7 );
			generalVecItem(pitemtype->pvecBtlvItems, sbtlv, iDepth+1);
			pitemtype->type = "B-TLV";
			pitemtype->itemnum = ToInt(vecItemString[3]);
			pitemtype->istartpos = ToInt(vecItemString[4]);
		}
		vecIter.push_back(pitemtype);
		iter++;
	}
}

itemType::~itemType()
{
	vector<itemType*>::iterator iter = pvecBtlvItems.begin();
	for ( ;iter!=pvecBtlvItems.end();)
	{
		delete *iter;
		iter=pvecBtlvItems.erase(iter);
	}
}

FileType::FileType(const char* sName, int iType,const char* sFormat, int iRecodenum)
{

	sfilename = sName;
	itype = iType;
	stypeformat = sFormat;
	generalVecItem(pvecItems, sFormat, 0);
	irecodenum = iRecodenum;
}

FileType::~FileType()
{
	vector<itemType*>::iterator iter = pvecItems.begin();
	for ( ;iter!=pvecItems.end();)
	{
		delete *iter;
		iter=pvecItems.erase(iter);
	}
}


CCardFileType::CCardFileType(void)
{
	//vecFileType_v1.push_back(new FileType("SSSEEF05",0,"01,cn,10|02,an,01|03,an,04|04,cn,0C|05,cn,04|06,cn,04|07,an,09"));
	vecFileType_v1.push_back(new FileType("SSSEEF05",0,"01,b,10|02,an,01|03,an,04|04,cn,0C|05,cn,04|06,cn,04|07,an,09"));
	vecFileType_v1.push_back(new FileType("SSSEEF06",0,"08,an,12|09,an,1E|0A,an,01|0B,cn,01|0C,cn,03|0D,cn,04"));
	vecFileType_v1.push_back(new FileType("SSSEEF07",2,"-,b,300"));

	vecFileType_v1.push_back(new FileType("DF01EF05",0,"20,an,01|21,an,3E|22,an,12"));
	vecFileType_v1.push_back(new FileType("DF01EF06",0,"25,an,3E|26,an,12|27,cn,03|28,an,0F"));
	vecFileType_v1.push_back(new FileType("DF01EF07",0,"29,an,01"));
	vecFileType_v1.push_back(new FileType("DF01EF08",0,"2B,an,01"));
	vecFileType_v1.push_back(new FileType("DF01EF09",0,"2E,an,3E|2F,an,08|30,an,09|31,an,01|32,an,03"));
	vecFileType_v1.push_back(new FileType("DF01EF0A",0,"37,dec,04|38,dec,04|39,cn,04"));
	
	vecFileType_v1.push_back(new FileType("DF02EF05",0,"40,B-TLV(57_an_03^58_cn_03^59_an_28),34,1,0|41,B-TLV(57_an_03^58_cn_03^59_an_28),34,1,1|42,cn,03|43,cn,1D|44,cn,1D|5A,cn,07|5B,cn,01|5C,an,09|5D,cn,04"));
	vecFileType_v1.push_back(new FileType("DF02EF06",0,"4B,cn,04|4C,cn,04|60,cn,04|4D,cn,04|4F,cn,04|50,cn,04|3A,an,07"));
	vecFileType_v1.push_back(new FileType("DF02EF07",1,"-,an,07|-,cn,04|-,cn,04|-,an,46", 4));
	vecFileType_v1.push_back(new FileType("DF02EF08",0,"52,cn,0A|53,an,3E|94,an,08|54,cn,04"));
	vecFileType_v1.push_back(new FileType("DF02EF09",0,"55,cn,0A|56,an,3E|95,an,08|96,cn,04|97,cn,04"));

	vecFileType_v1.push_back(new FileType("DF03EF05",0,"60,cn,04|98,cn,04|61,dec,04|62,cn,04|63,an,03|64,an,03|65,dec,03|66,dec,03"));
	vecFileType_v1.push_back(new FileType("DF03EF06",0,"67,cn,04|68,an,3E|69,an,3E|6A,an,10|6B,an,3C"));
	vecFileType_v1.push_back(new FileType("DF03EF07",0,"6C,cn,04|6D,dec,04|6E,cn,04|6F,cn,04|70,an,03|71,dec,04|72,dec,04|73,cn,04"));

	vecFileType_v1.push_back(new FileType("DF04EF05",0,"80,cn,04|81,an,3E|82,an,08|83,an,09|84,an,3E|85,an,08|86,an,09|87,an,3E|88,an,08|89,an,09|8A,an,0F|8B,an,0A|8C,an,01|8F,an,01"));
	vecFileType_v1.push_back(new FileType("DF04EF06",0,"90,dec,04|92,dec,04|93,dec,04"));
	vecFileType_v1.push_back(new FileType("DF04EF08",1,"-,b,02|-,an,01|-,cn,06|-,cn,07|-,hex,04|-,hex,04|-,hex,04",10));

	//vecFileType_v2.push_back(new FileType("SSSEEF05",0,"01,cn,10|02,an,01|03,an,04|04,cn,0C|05,cn,04|06,cn,04|07,an,09"));
	vecFileType_v2.push_back(new FileType("SSSEEF05",0,"01,b,10|02,an,01|03,an,04|04,cn,0C|05,cn,04|06,cn,04|07,an,09"));
	vecFileType_v2.push_back(new FileType("SSSEEF06",0,"08,an,12|09,an,1E|4E,an,14|0A,an,01|0B,cn,01|0C,cn,03|0D,cn,04"));
	vecFileType_v2.push_back(new FileType("SSSEEF07",2,"-,b,400"));
	vecFileType_v2.push_back(new FileType("SSSEEF08",2,"-,b,1000"));

	vecFileType_v2.push_back(new FileType("DF01EF05",0,"20,an,02|21,an,50|0E,cn,03"));
	vecFileType_v2.push_back(new FileType("DF01EF06",0,"23,an,50|24,cn,03|28,an,0F|2C,an,32|2D,an,0F"));
	vecFileType_v2.push_back(new FileType("DF01EF07",0,"29,an,01|2A,cn,01"));
	vecFileType_v2.push_back(new FileType("DF01EF08",0,"2B,an,01"));
	vecFileType_v2.push_back(new FileType("DF01EF09",0,"2E,an,46|30,an,09|32,an,02"));
	vecFileType_v2.push_back(new FileType("DF01EF0A",0,"37,an,03|38,an,02|39,cn,04"));
	vecFileType_v2.push_back(new FileType("DF01EF15",0,"2A,cn,01|40,B-TLV(57_an_03^58_cn_03^59_an_28),34,3,1"));
	vecFileType_v2.push_back(new FileType("DF01EF16",2,"-,b,128"));
	vecFileType_v2.push_back(new FileType("DF01EF17",2,"-,b,128"));
	vecFileType_v2.push_back(new FileType("DF01EF18",2,"-,b,128"));
	vecFileType_v2.push_back(new FileType("DF01EF19",2,"-,b,128"));
	vecFileType_v2.push_back(new FileType("DF01EF1A",2,"-,b,128"));

	vecFileType_v2.push_back(new FileType("DF02EF05",0,"42,an,03|41,an,03|43,B-TLV(5A_an_07^5B_an_01^5C_an_46^5D_cn_04^5E_an_10),6C,2,2|44,B-TLV(33_an_03^34_an_46^35_cn_04^36_an_10),65,2,4"));
	vecFileType_v2.push_back(new FileType("DF02EF06",0,"4C,cn,04|4B,an,05|4D,cn,03|60,cn,04|4F,an,06|50,cn,03|3A,an,07"));
	vecFileType_v2.push_back(new FileType("DF02EF07",1,"-,an,07|-,cn,04|-,cn,04|-,an,46", 4));
	vecFileType_v2.push_back(new FileType("DF02EF09",0,"55,an,10|56,an,46|96,cn,04|97,cn,03"));
	vecFileType_v2.push_back(new FileType("DF02EF15",0,"99,B-TLV(10_an_03^11_cn_04^12_cn_03),10,4,0|0F,cn,04|1F,cn,03"));
	vecFileType_v2.push_back(new FileType("DF02EF16",1,"-,an,03|-,cn,04|-,cn,04|-,cn,04|-,cn,03",4));

	vecFileType_v2.push_back(new FileType("DF03EF05",0,"61,cn,03|60,cn,04|98,cn,04|62,cn,04|63,an,03|64,an,03|65,an,03|66,an,03"));
	vecFileType_v2.push_back(new FileType("DF03EF06",0,"45,an,14|46,an,02|47,an,02|48,an,01|49,an,01|67,cn,04|6B,an,3C|4A,B-TLV(13_an_02^14_an_01^15_an_14^16_cn_04^17_an_3C),61,3,7"));
	vecFileType_v2.push_back(new FileType("DF03EF07",0,"70,an,03|71,cn,03|6E,cn,04|6C,cn,04|6F,cn,04|73,cn,04"));
	vecFileType_v2.push_back(new FileType("DF03EF15",0,"7A,cn,03|7B,cn,04|7C,cn,04"));
	vecFileType_v2.push_back(new FileType("DF03EF16",0,"51,cn,03|5F,an,01"));
	vecFileType_v2.push_back(new FileType("DF03EF17",0,"3B,an,14|3C,an,46|3D,an,01|3E,cn,04|3F,an,3C"));
	vecFileType_v2.push_back(new FileType("DF03EF18",0,"74,an,03|75,cn,03|76,cn,01|77,cn,04|78,cn,04"));
	vecFileType_v2.push_back(new FileType("DF03EF19",0,"79,B-TLV(18_an_03^19_cn_03^1A_cn_04^1B_cn_04^1C_dec_04^1D_an_03^1E_cn_04),27,4,0|75,cn,03|76,cn,01|77,cn,04|78,cn,04"));

	vecFileType_v2.push_back(new FileType("DF04EF05",0,"81,an,08|84,cn,03|87,an,11|8C,cn,01|80,cn,04|8B,an,1D|8A,an,0F|83,an,09|86,an,09|89,an,09|7D,an,09|7E,an,09|7F,an,09|8D,an,09|8E,an,09|8F,an,01"));
	vecFileType_v2.push_back(new FileType("DF04EF06",0,"90,an,01|92,dec,04|93,an,02"));
	vecFileType_v2.push_back(new FileType("DF04EF08",1,"-,b,02|-,an,01|-,cn,06|-,cn,07|-,hex,04|-,hex,04|-,hex,04",30));
	vecFileType_v2.push_back(new FileType("DF04EF15",1,"-,b,02|-,an,01",8));

	vecFileType_v2.push_back(new FileType("DF07EF05",0,"F0,an,02|F1,an,01|F2,cn,04|F3,an,03|F4,cn,04"));
	vecFileType_v2.push_back(new FileType("DF07EF06",0,"F5,an,03|F6,cn,04|F7,an,46"));
	vecFileType_v2.push_back(new FileType("DF07EF07",0,"F8,cn,04|F9,an,01"));
	
	icardversion = 2;
}

CCardFileType::~CCardFileType(void)
{
	vector<FileType*>::iterator iter = vecFileType_v2.begin();
	while (iter != vecFileType_v2.end())
	{
		delete *iter;
		iter++;
	}

	iter = vecFileType_v1.begin();
	while (iter != vecFileType_v1.end())
	{
		delete *iter;
		iter++;
	}
}

itemType* CCardFileType::GetTypeByID(string filename, string id)
{
	vector<FileType*>* pfts = GetVecFileType();
	vector<FileType*>::iterator iter = pfts->begin();
	while ( iter != pfts->end() )
	{
		if ( filename == (*iter)->sfilename )
			return (*iter)->GetTypeByID(id);
		iter++;
	}

	throw new ExceptionEx( SCD_ERR_CARD_INPUTILLEGAL , format("文件%s 标示%s 未找到", filename.c_str(), id.c_str()));
	return NULL;
}

CCardFileType CCardFileType::static_scardfiletype;

CCardFileType* CCardFileType::GetInistance()
{
	return &static_scardfiletype;
}

CCardFileType* CCardFileType::SetCardVersion(int icv)
{
	icardversion = icv;
	return this;
}


vector<FileType*>* CCardFileType::GetVecFileType()
{
	if ( icardversion == 1)
	{
		return &vecFileType_v1;
	}
	else
	{
		return  &vecFileType_v2;
	}
}

bool CCardFileType::FindId(string id)
{
	vector<FileType*>::iterator iter = GetVecFileType()->begin();
	while (iter != GetVecFileType()->end())
	{
		if ( (*iter)->FindId(id) )
			return true;
		iter++;
	}
	return false;
}

itemType* FileType::GetTypeByID(string id)
{
	vector<itemType*>::iterator iter = pvecItems.begin();
	while (iter != pvecItems.end())
	{
		if ( (*iter)->id == id )
			return *iter;
		iter++;
	}

	throw new ExceptionEx( SCD_ERR_CARD_INPUTILLEGAL , format("标示%s 未找到", id.c_str()));
	return NULL;
}

bool FileType::FindId(string id)
{
	vector<itemType*>::iterator iter = pvecItems.begin();
	while (iter != pvecItems.end())
	{
		if ( (*iter)->id == id )
			return true;
		iter++;
	}

	return false;
}

FileType* GetFileTypeByID(string filename)
{
	vector<FileType*>::iterator iter = CCardFileType::GetInistance()->GetVecFileType()->begin();
	while (iter != CCardFileType::GetInistance()->GetVecFileType()->end())
	{
		if ( (*iter)->sfilename == filename )
			return *iter;
		iter++;
	}

	throw new ExceptionEx( SCD_ERR_CARD_INPUTNOFILEORTAG , format("文件%s 未找到", filename.c_str()));
	return NULL;
}

/////////////////////////// 读数据后处理 //////////////////////////////
//根据格式读B-TLV
string readByTypeBtlv(vector<itemType*>& vecitemTypes, string src)
{
	string sres;
	vector<itemType*>::iterator iter = vecitemTypes.begin();
	int ipos = 0;
	int iLen = 0;
	string srcLast = src;

	while ( iter!= vecitemTypes.end())
	{
		int isrcpos = 0;
		if ( (*iter)->id != "-")
			isrcpos = 4;
		
		iLen = (*iter)->len;
		if ( srcLast.size() < (iLen*2+isrcpos) )
			throw new ExceptionEx(SCD_ERR_INFO_VERITY, format("解析%s的Tag%s数据时长度错误",rdutil._pfiletype->sfilename, (*iter)->id.c_str() ));

		src = srcLast.substr(0, isrcpos + iLen*2);
		srcLast = srcLast.substr(isrcpos + iLen*2);
		Total(sres,readByType( *iter, src ), "^");
		ipos += isrcpos + iLen*2;
		iter++;
	}
	return sres;
}

string readByType(itemType* pitemtype, string src)
{
	int isrcpos  = 0;
	if ( pitemtype->id != "-")
	{	
		isrcpos = 4;
		string srcHead = src.substr(0,2);
		if ( srcHead.compare( pitemtype->id ) != 0 )
		{
			throw new ExceptionEx(SCD_ERR_INFO_VERITY, format("解析%s的Tag%s数据时标签错误",rdutil._pfiletype->sfilename, pitemtype->id.c_str() ));
		}
	}

	if ( pitemtype->type == "an")
	{
		return r_ans(src.substr(isrcpos));
	}
	else if ( pitemtype->type == "b")
	{
		return r_b(src.substr(isrcpos));
	}
	else if ( pitemtype->type == "cn")
	{
		return r_cn(src.substr(isrcpos));
	}
	else if ( pitemtype->type == "dec")
	{
		return r_dec(src.substr(isrcpos));
	}
	else if ( pitemtype->type == "hex")
	{
		return r_hex(src.substr(isrcpos));
	}
	else // B-TLV
	{
		return readByTypeBtlv(pitemtype->pvecBtlvItems, src.substr(isrcpos));
	}
	return "";
}

std::string GeneralReadByID(FileType* type, string src, string id)
{
	//如果是循环记录文件
	if ( type->itype == 1 )
	{
		return readByTypeBtlv(type->pvecItems , src);
	}

	//如果是变长记录文件
	if ( type->itype == 0 )
	{
		itemType* pitemtype = type->GetTypeByID(id);
		return readByType(pitemtype, src);
	}

	return "";
}

/////////////////////////// 写数据后处理 //////////////////////////////
string removeFirstItem(vector<string>& vecSrc)
{
	if ( vecSrc.size() == 0 )
	{
		//允许写空值
		//TRACE_LINE(format("文件%s输入的数据源过少\n",rdutil._pfiletype->sfilename).c_str());
		return "";
	}
	
	string sres = vecSrc[0];
	vecSrc.erase(vecSrc.begin());
	return sres;
}

string writeByType(itemType* pitemtype, vector<string>& vecSrc)
{
	string id = "";
	if ( pitemtype->id != "-")
		id = pitemtype->id;

	try {
		if ( pitemtype->type == "an")
		{
			return totlv(id, w_ans(removeFirstItem(vecSrc),pitemtype->len));
		}
		else if ( pitemtype->type == "b")
		{
			return  totlv(id,w_b(removeFirstItem(vecSrc), pitemtype->len));
		}
		else if ( pitemtype->type == "cn")
		{
			return  totlv(id,w_cn(removeFirstItem(vecSrc), pitemtype->len));
		}
		else if ( pitemtype->type == "dec")
		{
			return  totlv(id,w_dec(removeFirstItem(vecSrc), pitemtype->len));
		}
		else if ( pitemtype->type == "hex")
		{
			return  totlv(id,w_hex(removeFirstItem(vecSrc), pitemtype->len));
		}
		else // B-TLV
		{
			vector<string> vecDatas = Split(removeFirstItem(vecSrc), "^","", true);
			return  totlv(id,writeByTypeBtlv(pitemtype->pvecBtlvItems, vecDatas) );
		}
	}
	catch(ExceptionTrans* e)
	{
		delete e;
		throw new ExceptionEx(SCD_ERR_CARD_INPUTILLEGAL, format("输入%s的Tag%s数据时长度错误",rdutil._pfiletype->sfilename, pitemtype->id.c_str() ));
	}
	return "";
}

//根据格式写B-TLV
string writeByTypeBtlv(vector<itemType*>& vecitemTypes,vector<string>& vecitemData)
{
	string sres;
	vector<itemType*>::iterator iter = vecitemTypes.begin();
	while ( iter!= vecitemTypes.end())
	{
		sres += writeByType( *iter, vecitemData);		
		iter++;
	}
	return sres;
}


std::string GeneralWriteByID(FileType* type, vector<string>& vecSrc, string id)
{
	//如果是循环记录文件
	if ( type->itype == 1 )
	{
		vector<string> vecitemData = Split( removeFirstItem(vecSrc), "^", "", true);
		return writeByTypeBtlv(type->pvecItems , vecitemData );
	}

	//如果是变长记录文件
	if ( type->itype == 0 )
	{
		itemType* pitemtype = type->GetTypeByID(id);
		return writeByType(pitemtype, vecSrc);
	}

	//如果是变长记录文件
	if ( type->itype == 2 )
	{
		//return writeByType(type->pvecItems[0], vecSrc);
		return w_bb(removeFirstItem(vecSrc), type->pvecItems[0]->len);
	}

	return "";
}




