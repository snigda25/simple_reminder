
#ifndef __STATUS_TRAY_H__
#define __STATUS_TRAY_H__

#include<windows.h>
#include<commctrl.h>



#define ID_STATUS_BAR	(WM_APP + 77)
#define KAVI_TRAY_MESSAGE	(WM_APP + 78)
//Menu Items
#define IDM_APP_EXIT					(WM_APP + 101)
#define IDM_SYS_APP_SHOW				(WM_APP + 102)
#define IDM_SYS_APP_HIDE				(WM_APP + 103)
#define IDM_APP_ABOUT					(WM_APP + 104)
#define IDM_APP_STARTUP					(WM_APP + 105)
#define IDM_SYS_ADD_REMINDER			(WM_APP + 106)
#define IDM_SYS_EDIT_REMINDER			(WM_APP + 107)
#define IDM_SYS_DEL_REMINDER			(WM_APP + 108)
#define IDM_SYS_DELETE_ALL_REMINDER		(WM_APP + 109)



#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383



//funtions
HWND CreateStatusBar(const HWND hwnd);
void DisplayClockDateStatusBar(const HWND hStatusBar);
HWND CreateListViewBox(const HINSTANCE hInstance,HWND hwnd);
int AddListViewRow(const HWND hControl, TCHAR *szItemText, TCHAR *szSubItemText,TCHAR *szSubItemText2);
HMENU MenuDisplay(const HWND hwnd);
HMENU SystemTrayMenu(const HWND hwnd);
HKEY CreateApplicationKey();
int AddDatatoRegistry(const HKEY hKey, TCHAR *szDate,TCHAR *szTime,TCHAR *szReminder);
int SearchRegistryString(const HKEY hKey,TCHAR *szSearchDate,TCHAR *szSearchTime,TCHAR *szReminderData);
int UpdateListViewfromRegistry(const HWND hListView,HKEY hKey);
int CALLBACK SortingCodeListViewControl(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)  ;
int CompareCurrentTimewithRegistryDataTimer(const HKEY hKey);
int DeleteEmptyKeysfromRegistry(const HKEY hKey);
HMENU RightClickMenuonListView(const HWND hwnd);




#endif