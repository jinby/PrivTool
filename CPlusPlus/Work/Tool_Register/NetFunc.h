#include <NB30.H>
#pragma comment(lib, "Netapi32.lib")

#include "CmdAPIzhy.h"
#include "glb.h"

typedef struct _ASTAT_   
{   
	ADAPTER_STATUS  adapt;   
	NAME_BUFFER     NameBuff[30];   
} ASTAT, *PASTAT;   

ASTAT Adapter;   
int  EncryptString(CString m_key, CString strData, CString& strEncrypt) ;

CString getmac_one(int lana_num)   
{   
	char  text[256];   
	NCB   ncb;   
	UCHAR uRetCode;   
    
	memset(&ncb, 0, sizeof(ncb));   
	ncb.ncb_command  = NCBRESET;   
	ncb.ncb_lana_num = lana_num;           
    
	uRetCode = Netbios(&ncb);   
	//sprintf(text, "The NCBRESET return code is: 0x%x\n", uRetCode);   
	//AfxMessageBox(text,0,0);   
    
	memset(&ncb, 0, sizeof(ncb));   
	ncb.ncb_command  = NCBASTAT;   
	ncb.ncb_lana_num = lana_num;       //指定网卡号   
    
	strcpy((char*)ncb.ncb_callname,"*");   
	ncb.ncb_buffer = (unsigned char *)&Adapter;   
    
	//指定返回的信息存放的变量   
	ncb.ncb_length = sizeof(Adapter);   
    
	//接着，可以发送NCBASTAT命令以获取网卡的信息   
	//debug     
	uRetCode = Netbios(&ncb);   
	sprintf(text,"The NCBASTAT return code is: 0x%x\r\n", uRetCode);   
	//AfxMessageBox(text,0,0);   
    
	if (uRetCode == 0)   
	{   
		//把网卡MAC地址格式化成常用的16进制形式，如0010A4E45802   
		sprintf(text,"%02X%02X%02X%02X%02X%02X",   
				Adapter.adapt.adapter_address[0],   
				Adapter.adapt.adapter_address[1],   
				Adapter.adapt.adapter_address[2],   
				Adapter.adapt.adapter_address[3],   
				Adapter.adapt.adapter_address[4],   
				Adapter.adapt.adapter_address[5] );   
		
		//AfxMessageBox(text,0,0); 
		return text;
	}
	return "";
}   

BOOL GetComuterNetInfo(CStringArray &comptrMac)
{
	comptrMac.RemoveAll();
	return zhyapi_GetComputerNETMAC(comptrMac);

	NCB       ncb;   
	UCHAR     uRetCode;   
	LANA_ENUM lana_enum;   
    
	memset(&ncb, 0, sizeof(ncb));   
	ncb.ncb_command = NCBENUM;   
    
	ncb.ncb_buffer  = (unsigned char*)&lana_enum;   
	ncb.ncb_length  = sizeof(lana_enum);   
    
	//向网卡发送NCBENUM命令，以获取当前机器的网卡信息，如有多少个网卡、每张网卡的编号等   
	uRetCode = Netbios(&ncb);   
	//printf(   "The NCBENUM return code is: 0x%x\n", uRetCode);   
	if   (uRetCode == 0)   
	{   
		//char   text[256];   
		//sprintf(   text,"Ethernet Count is: %d\n\n", lana_enum.length);   
		//AfxMessageBox(text,0,0);   
		
		//对每一张网卡，以其网卡编号为输入编号，获取其MAC地址 
// 		srand( (unsigned)time( NULL ) );

		for (int i = 0; i < lana_enum.length; i++)   
		{   
			CString strMAC = getmac_one(lana_enum.lana[i]);  
			comptrMac.Add(strMAC);
		}   
	} 

	return TRUE;

}

//遍历PC机网卡MAC码，加密
BOOL GenerateMac( CString &strCompMac)
{
	strCompMac = "";

	CStringArray comptrMac;
	GetComuterNetInfo(comptrMac);
	CString sMacEncrypt;
	for (int i = 0; i < comptrMac.GetSize(); i++)   
	{   
		CString strMAC = comptrMac[i];  
		
		//加密
		CString sKey = "1F84B2ADF8EB82647A037E2040627D81";
		EncryptString(sKey, strMAC, sMacEncrypt);

		strCompMac += sMacEncrypt;
		if (i < (comptrMac.GetSize()-1))
			strCompMac += ";";
	}   

	return TRUE;
}


int DataEncrypt(BYTE *data, WORD dlen, BYTE *keyl, BYTE *keyr, BYTE keylen)
{
	//BYTE length;  
	BYTE ptr[2560];//gmz 256->2560
	int len;

	//length = dlen;
	WORD len1=dlen;
	BYTE temp[2],temp2[2];
	memcpy(temp,(BYTE*)&len1,2);
	temp2[0]=temp[1];
	temp2[1]=temp[0];
	memcpy(ptr,temp2,2);
	memcpy(ptr+2, data, dlen);//gmz modify
	dlen+=2;//前面加上了长度gmz
	len = dlen % 8;
	//将数据补齐80 00 00 ... ...
		ptr[dlen] = 0x80;
		if(len)   
			len = 7 - len;	
		else
			len = 7;
		memset(ptr+dlen+1, 0, len);
	if(keylen == 0x10){
		for(int i = 0; i<1+dlen/8; i++)
			tri_des(ptr + i*8, keyl, keyr, 0);
	}
	else
		if(keylen == 8){
			for(int i = 0; i<1+dlen/8; i++)
				des(ptr + i*8, keyl, 0);
		}

	memcpy(data, ptr, dlen+2+len);
	//data[dlen+1+len] = '\0';
	
	return dlen+2+len;
}

//加密字符串数据
int  EncryptString(CString m_key, CString strData, CString& strEncrypt) 
{
	int m_des1or3 = 0;
	CString m_keyleft, m_keyright, keyl, keyr;
	BYTE keyleft[8] = {0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51};
	BYTE keyright[8] = {0x56,0x56,0x56,0x56,0x56,0x56,0x56,0x56};

	m_key.TrimLeft();
	m_key.TrimRight();
	if(m_key.GetLength()!=16
		&& m_key.GetLength()!=32)
	{
		return 1;
	}
	if(m_key.GetLength()==16)//des
	{
		m_des1or3=0;
		m_keyleft=m_key;
	}
	else           //3des
	{
		m_des1or3=1;
		m_keyleft=m_key.Left(16);
		m_keyright=m_key.Right(16);
	}

	unsigned char sourcehex[255], sourceascii[255];

	int tmplen, i = 0, /*j = 0,*/ length = 0;
	if(m_des1or3)  //3des
	{
		keyl = m_keyleft;
		keyr = m_keyright;
		//str = keyl;
		if(!keyl.IsEmpty() && !keyr.IsEmpty())
		{
			length = keyl.GetLength();
			for(i=0; i<length; i++)  
				sourcehex[i] = keyl[i];
			AsciiToHex(sourcehex, keyleft, length);
			length = keyr.GetLength();
			for(i=0; i<length; i++)  
				sourcehex[i] = keyr[i];
			AsciiToHex(sourcehex, keyright, length);
		}
		else
		{
			if(keyl.IsEmpty())   keyl = keyleft;
			if(keyr.IsEmpty())   keyr = keyright;
		}
	}
	else   //单des
	{
		keyl = m_keyleft;
		if(!keyl.IsEmpty()){
			length = keyl.GetLength();
			for(i=0; i<length; i++)  
				sourcehex[i] = keyl[i];
			AsciiToHex(sourcehex, keyleft, length);
		}
		else 
			keyl = keyleft;
	}

	//进行加密
	memset(sourceascii, 0x00, sizeof(sourceascii));
	memset(sourcehex, 0x00, sizeof(sourcehex));
	strcpy((char*)sourceascii, strData.GetBufferSetLength(strData.GetLength()));
	CString str;
	str = sourceascii;
	length = str.GetLength();
	tmplen = AsciiToHex(sourceascii, sourcehex, length);

	if(m_des1or3)
		tmplen = DataEncrypt(sourcehex, length/2, keyleft, keyright, 16);
	else
		tmplen = DataEncrypt(sourcehex, length/2, keyleft, keyright, 8);
	if (tmplen == 0)
	{
		return FALSE;
	}
	
	//将source转换为字符串
	HexToAscii(sourcehex, sourceascii, tmplen);	
	//sourceascii[tmplen] = '\0';
	sourceascii[tmplen*2] = '\0';

	strEncrypt = sourceascii;

	return TRUE;		
}


