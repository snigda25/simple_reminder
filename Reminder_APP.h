#ifndef __REMINDER_APP_H__
#define __REMINDER_APP_H__

#include<windows.h>
#include<commctrl.h>
#include<psapi.h>

//Resource IDs
//for time in Status bar
#define ID_TIMER_1 (WM_APP + 100)
//for searching registry after every minute
#define ID_TIMER_2 (WM_APP + 101)

#define MAX_REMINDER_DATA 1024

struct CLICKEDITEMDATA
{
	BOOL  DatainsideListBox;	//0-1 value
	TCHAR szDate[15];
	TCHAR szTime[15];
	TCHAR szReminder[MAX_REMINDER_DATA+1];
};






#endif