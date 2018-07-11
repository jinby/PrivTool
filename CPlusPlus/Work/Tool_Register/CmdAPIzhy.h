/*---------------------------------------------------------------------------
**版权所有 (C)2011, 武汉天喻信息产业股份有限公司。
**
**文件名称：CmdAPIzhy.h
**文件标识：
**内容摘要：简要描述本文件的内容，包括宏定义，常量定义，结构定义等内容
**其它说明：为实现对命令行命令的使用而封装
**当前版本：
**作 	者：zhangheyong
**完成日期：2011:9:2   15:47
**
**备注：

**修改记录1:
** 修改日期：
** 版 本 号：
** 修 改 人：
** 修改内容：
**修改记录2：
**---------------------------------------------------------------------------*/
#ifndef CmdAPIzhy_h__
#define CmdAPIzhy_h__

//获取计算机物理MAC地址 
BOOL zhyapi_GetComputerNETMAC(CStringArray &strNETMAC);

//根据getmac命令行中的反馈值中分解出MAC值
BOOL zhyapi_ParseNetMAC(const CString strBuf, CStringArray &strNETMAC);


#endif // CmdAPIzhy_h__