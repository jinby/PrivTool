//MyReg.h
#pragma once

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

//���ĳ�����Ƿ����
ULONG TestRegSubKey(HKEY StartKey,char * NewSubKeyName);
//��������
ULONG NewRegSubKey(HKEY StartKey,char * NewSubKeyName);
//�����Ӽ�
ULONG NewRegKeyValue(HKEY StartKey,char * SubKeyName,char * NewValueName,ULONG NewValueType);
//��ȡ�Ӽ����ݡ����������Ĭ���Ӽ�ֵ���Ӽ�����
ULONG ReadRegValueData(HKEY StartKey,char * SubKeyName,char * ValueName
					   ,PULONG OutValueType,PVOID InOutBuffer,PULONG InOutBufferLen);
//��������Ĭ��ֵ�ַ���
ULONG SetRegKeyStr(HKEY StartKey,char * SubKeyName,char * DataBuffer);
//�����Ӽ�����ֵ
ULONG SetRegKeyValueData(HKEY StartKey,char * SubKeyName,char * ValueName,ULONG NewValueType
						 ,PVOID DataBuffer,ULONG DataLen);
//��ѯ������Ŀ
ULONG NumRegSubKey(HKEY StartKey,char * SubKeyName);
//��ѯ�Ӽ���Ŀ
ULONG NumRegSubKeyValue(HKEY StartKey,char * SubKeyName);
//��ȡ����������
ULONG ReadRegSubKeyOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen);
//��ȡ�����Ӽ���
ULONG ReadRegSubKeyValueOrdName(HKEY StartKey,char * SubKeyName,ULONG Ord,char* OutName,PULONG OutNameLen);

