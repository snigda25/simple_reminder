#ifndef __DIALOG_DATE_H__
#define __DIALOG_DATE_H__

#include<windows.h>
#include<commctrl.h>

#define MAX_REMINDER_IN_EDIT_BOX		1024


//functions
void DateStringtoDateComponents(TCHAR *szDate,short *day, short *month, short *year);
void DateComponentstoDateString(const short day,const short month,const short year,TCHAR *szDateString);
void TimeStringtoTimeComponents(TCHAR *szTime,short *hour, short *minute, short *AMP);
void TimeComponentstoTimeString(const short hour,const short minute,const short AMP,TCHAR *szTimeString);
int DeleteDatafromRegistry(const HKEY hKey,TCHAR *szDate,TCHAR *szTime);
int FillDefaultDatainDialogBox(const HWND hDlg);
int SaveDialogButton(const HWND hDlg,const HKEY hKey);
int DeleteDialogButton(const HWND hDlg,const HKEY hKey);

#endif