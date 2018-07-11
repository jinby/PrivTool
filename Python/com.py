# -*- coding:utf-8 -*-
import threading
import win32com.client
import pythoncom
import sys,os

#获取脚本文件的当前路径
def cur_file_dir():
    #获取脚本路径
    path = sys.path[0]
    #判断为脚本文件还是py2exe编译后的文件，如果是脚本文件，则返回的是脚本的目录，如果是py2exe编译后的文件，则返回的是编译后的文件路径
    if os.path.isdir(path):
        return path
    elif os.path.isfile(path):
        return os.path.dirname(path)

def OutputWord():
    pythoncom.CoInitialize()
    o = win32com.client.Dispatch("kwps.Application")
    o.Visible=True
    dirname = cur_file_dir()
    doc = o.Documents.open(dirname + r"\02.PBOC_UICS 应用个人化信息表v3.02.doc")
    table = doc.Tables(1)
    print(table.Columns.Count)
    print(table.Cell(3,2).Range.Text.decode("gbk", "ignore").encode("utf-8"))
#    for x in range(1,table.Rows.Count):
#        print(table.Rows(x).Cells.Count)
#        table2 = table.Rows(x).Cells(1).Tables
#       print(table2.Count)
    #for x in range(1,table.Rows.Count):
    #   print(table.Rows(1))
        #if table.Rows(x).Cells.Count == 2:
        #   str1 = table.cell(x,1).Range.Text
        #	str1 = str1[0:-2]
        #	str2 = table.cell(x,2).Range.Text
        #	str2 = str2[0:-2]
        #	print("%03d-%s=%s"%(x,str1,str2))
    o.Documents.Close()
    pythoncom.CoUninitialize()

if __name__=='__main__':
    t1 = threading.Thread(target=OutputWord)
    t1.start()

