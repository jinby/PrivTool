//FuncCall.h

#pragma once

//�ƶ����ڵ���Ļ�м�λ��
void __stdcall MoveWindowTo(CWnd * pWnd,long InWidth,long InHeight);
//��Ļ���������ָ��SetWidthAndHeight�ݣ�����ȡOnWidthAndHeight�ݣ����ƶ�����Ļ����
void __stdcall MoveWindowTo(CWnd * pWnd,UCHAR  SetWidthAndHeight,UCHAR OnWidthAndHeight);
//��ʼ���й�ȫ���ļ����ַ���
void __stdcall InilinExeFileInfo();
//�򿪴���Ŀ¼
void __stdcall CreatePath();
//��ȡͼ��߶ȺͿ��
CSize __stdcall GethIconSize(HICON InhIcon);
//���ô���͸����
void  __stdcall SetWindowAttributes(CWnd * pWnd,long Attributes);
//��鱾���ļ�MD5
long  __stdcall  CheckFileMd5();
//����ϵͳPE�ļ�������Ϣ
unsigned int AFX_CDECL ThreadSetLinkInfo(PVOID p);