# -*- coding:utf-8 -*-
import win32com.client
#import chardet
o = win32com.client.Dispatch("KWPS.Application")
o.Visible=True
doc = o.Documents.open(r"C:\\Users\Administrator\Desktop\02.PBOC_UICS 应用个人化信息表v3.02.doc")
doc.Tables[0].Rows.First.cells[1].Range.Select


#print(doc.Tables[0].Rows.Count)
#print(doc.Tables[0].Rows.First)
#print(doc.Tables[0].Rows.Last)
#print(doc.Tables[0].Rows[0])
#print(doc.Tables[0].cell(1,2).Row.Range.Text) -可行

#doc.Tables[0].cell(1,2).Formula = '111'
#print(doc.Tables[0].cell(1,2))
#for a in range(0,10):
 #   print(doc.tables[0].Rows[a])
    #print(doc.tables[0].Rows[a].Cells[0].Range.Text)
 #   print(doc.Tables[0].Rows[1])
    #pass
