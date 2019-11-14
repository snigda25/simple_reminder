#include "Reminder_APP.h"
#include "Status_List_Reg.h"
#include "Dialog_Date.h"
#include "resource.h"


//global data
HINSTANCE hInstance;
HANDLE hProcess;
HWND hStatusBar;
HMENU hMenu,hSystemTrayMenu,hSystemMenu;
HWND hListViewControl;
NOTIFYICONDATA IconData={sizeof(NOTIFYICONDATA)};
HKEY hApplicationKey;
struct CLICKEDITEMDATA ClickedItemData;
TCHAR	*szApplicationPathnName;
TCHAR	szAppName[] = L"Reminder Application by Kavitesh Singh" ;
TCHAR	szRegistryStartApplicationPath[]=L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
int		ApplicationStartUpStatus=0;
int RightClickMenuStatus=0;

//Functions
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK TrayNotification(HWND hwnd, WPARAM wParam, LPARAM lParam);
VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
void CreateTrayIcon(const HWND hwnd);
void FillClickedDatainDialogBox(const HWND hDlg);
void CheckApplicationStartupWithWindows(const HWND hwnd,const  HMENU hMenu);
BOOL CenterWindow(HWND hwnd);
int CALLBACK SortingCodeListViewControl(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)  ;

//entry point
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HWND         hwnd,hwndPrevInstance ;
     MSG          msg ;
     WNDCLASS     wndclass ;

	 ::hInstance=hInstance;

	 //to see if program is running. if yes it activates it and exits the second instance
	 hwndPrevInstance=FindWindow(szAppName,NULL);
	 if(hwndPrevInstance)
	 {
	   //if eliminated it gets default value from winmain.This initialization will set window
	   // to normal and active positon
		iCmdShow=SW_SHOWNORMAL;
		ShowWindow(hwndPrevInstance,SW_SHOWNORMAL);
		return 0;
	 }

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_ICON1)) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"),
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }
	 InitCommonControls();

     hwnd = CreateWindow (szAppName,                  // window class name
                          TEXT ("The Reminder Application"), // window caption
                          WS_OVERLAPPED|WS_SYSMENU |WS_THICKFRAME |WS_VISIBLE|WS_MINIMIZEBOX  ,			// window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters

     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return (UINT)msg.wParam ;
}

//main callback funtion
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
	 default:
		 return DefWindowProc (hwnd, message, wParam, lParam) ;

	case WM_CREATE:
			//display window in center of desktop
			CenterWindow(hwnd);
			//open process to get application name
			hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE,GetCurrentProcessId());
			//Get directory from where exe has started and reserve 100 TCHARs for AppName
			szApplicationPathnName=(TCHAR*)malloc(sizeof(TCHAR)*(GetCurrentDirectory(0,NULL)+100));
			//get the full path including the application Name
			//last paramete require size of buffer in bytes
			GetModuleFileNameEx(hProcess,NULL,szApplicationPathnName,sizeof(TCHAR)*(GetCurrentDirectory(0,NULL)+100));
			//display menu bar
			hMenu=MenuDisplay(hwnd);
			//check start application at windows start up after menu is created
			//so that option can be checked/unchecked
			CheckApplicationStartupWithWindows(hwnd, hMenu);
			//timer for status bar time update
			SetTimer(hwnd,ID_TIMER_1,1000,TimerProc);
			//timer for every minute registry comparison with current time
			SetTimer(hwnd,ID_TIMER_2,59000,TimerProc);
			//create tray icon
			CreateTrayIcon(hwnd);
			//create status bar and store the handle
			hStatusBar = CreateStatusBar(hwnd);
			//create listview control and store the handle
			hListViewControl = CreateListViewBox(::hInstance,hwnd);
			//main registry key in which furthe data will be stored
			hApplicationKey=CreateApplicationKey();
			//list view is updated after scanning registry for previously
			//stored data
			UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
			return 0 ;

	case WM_CLOSE:
		//if close button pressed it sends application to system tray
		ShowWindow(hwnd,SW_HIDE);
		return 0;

	 case WM_DESTROY:
		 //kill timer
		 KillTimer(hwnd,ID_TIMER_1);
		 KillTimer(hwnd,ID_TIMER_2);
		 //to close application we do all processing here.
		 //to remove systemtray icon
		 Shell_NotifyIcon(NIM_DELETE,&IconData);
		 //close registry key
		 RegCloseKey(hApplicationKey);
		 //to free application name
		 free(szApplicationPathnName);
		 szApplicationPathnName=NULL;
		 CloseHandle(hProcess);
		 DestroyWindow(hwnd);
		 PostQuitMessage (0) ;
		 return 0 ;

	 case WM_COMMAND:
		 {
			 switch(LOWORD(wParam))
			 {
			 default:
				 return DefWindowProc (hwnd, message, wParam, lParam) ;
				 //menu items
			 case  IDM_SYS_ADD_REMINDER:
				 ClickedItemData.DatainsideListBox=0;
				 ZeroMemory(ClickedItemData.szDate,sizeof(ClickedItemData.szDate));
				 ZeroMemory(ClickedItemData.szReminder,sizeof(ClickedItemData.szReminder));
				 ZeroMemory(ClickedItemData.szTime,sizeof(ClickedItemData.szTime));
				 DialogBoxW(::hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DialogProc);
				 return 0;
			 case IDM_SYS_EDIT_REMINDER:
				 DialogBoxW(::hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DialogProc);
				 return 0;
			 case IDM_SYS_DEL_REMINDER:
				DeleteDatafromRegistry(hApplicationKey,ClickedItemData.szDate,ClickedItemData.szTime);
				UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
				ClickedItemData.DatainsideListBox=0;
				ZeroMemory(ClickedItemData.szDate,sizeof(ClickedItemData.szDate));
				ZeroMemory(ClickedItemData.szReminder,sizeof(ClickedItemData.szReminder));
				ZeroMemory(ClickedItemData.szTime,sizeof(ClickedItemData.szTime));
				return 0;
			 case IDM_SYS_DELETE_ALL_REMINDER:
				 {
					 TCHAR szDate[20]=L"";
					 TCHAR szTime[20]=L"";
					 LVITEM LvItem = {sizeof(LVITEM)};
					 if(IDOK==MessageBox(hwnd,L"Are you sure you want to delete all the reminders",L"Delete all Reminders",MB_OKCANCEL|MB_DEFBUTTON2))
					 {
						while(SendMessage(hListViewControl,(UINT) LVM_GETITEMCOUNT,0,0))
						{
							//get the item and subitem to be deleted
							LvItem.mask=LVIF_TEXT;
							LvItem.iItem=0;
							LvItem.cchTextMax=19;
							//for date
							LvItem.iSubItem=0;
							LvItem.pszText=szDate;
							SendMessage(hListViewControl,(UINT)LVM_GETITEM,0,(LPARAM)&LvItem);
							//for time
							LvItem.iSubItem=1;
							LvItem.pszText=szTime;
							SendMessage(hListViewControl,(UINT)LVM_GETITEM,0,(LPARAM)&LvItem);
							DeleteDatafromRegistry(hApplicationKey,szDate,szTime);
							UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
						}
						return 0;
					 }
					 else
					 return 0;
				 }
			 case IDM_APP_EXIT  :
				 SendMessage(hwnd,WM_DESTROY,0,0);
				 return 0;
			 case IDM_SYS_APP_SHOW:
				 ShowWindow(hwnd,SW_SHOWNORMAL);
				 return 0;
			 case IDM_SYS_APP_HIDE:
				 ShowWindow(hwnd,SW_HIDE);
				 return 0;
			 case IDM_APP_ABOUT:
				 MessageBox(hwnd,L"Reminder Application created\n                    by\n          Kavitesh Singh",L"About Application",0);
				 return 0;
			 case IDM_APP_STARTUP:
				 {
					 HMENU hOptionMenu;
					 HKEY hKey;
					 LONG lResult=-1;
					 //open registry key where startup info is stored
					 lResult=RegOpenKeyEx(HKEY_CURRENT_USER, szRegistryStartApplicationPath,0,KEY_ALL_ACCESS,&hKey);
					 if( ERROR_SUCCESS!=lResult)
					 {
						 MessageBox(hwnd,L"Error getting the startup information",L"StartUp Error",0);
						 return 0;
					 }
					 //if already startup enable then disable it
					 if(ApplicationStartUpStatus)
					 {
						 //now delete the application name value key from it.
						 lResult = RegDeleteValue(hKey,szAppName);
						 if( ERROR_SUCCESS!=lResult)
						 {
							 RegCloseKey(hKey);
							 MessageBox(hwnd,L"Error getting the startup information",L"StartUp Error",0);
							 return 0;
						 }
						 //the menu is at second position,zero based count
						 hOptionMenu=GetSubMenu(hMenu,1);
						 //uncheck menu option in drop down is zero based and first one hence 0 for 2nd arguement
						 lResult=CheckMenuItem(hOptionMenu,1,MF_BYPOSITION|MF_UNCHECKED);
						 //ApplicationStartUpStatus variable
						 ApplicationStartUpStatus=0;
					 }
					 else
					 {
						 //if startup status is not set then set it and create registry
						 //entry and check the menu option
						 lResult=RegSetValueEx(hKey,szAppName,0,REG_SZ,(BYTE*)szApplicationPathnName,(lstrlen(szApplicationPathnName)+1)*sizeof(TCHAR));
						 if( lResult != ERROR_SUCCESS )
						 {
							 RegCloseKey(hKey);
							 MessageBox(NULL,L"Error in Creating Registry Key for Reminder",L"Registry Error",0);
							 return FALSE;
						 }
						 //the menu is at second position,zero based count
						 hOptionMenu=GetSubMenu(hMenu,1);
						 //uncheck menu option in drop down is zero based and first one hence 0 for 2nd arguement
						 lResult=CheckMenuItem(hOptionMenu,1,MF_BYPOSITION|MF_CHECKED);
						 //ApplicationStartUpStatus variable
						 ApplicationStartUpStatus=1;
					 }
					 DrawMenuBar(hwnd);
					 RegCloseKey(hKey);
				 }
				 return 0;
			 }
		 }

	 case WM_SIZE :
		 {
			 static int cyVScroll;
			 RECT rect;
			 GetClientRect(hwnd, &rect);
			 cyVScroll = GetSystemMetrics(SM_CYVSCROLL);
			 MoveWindow (
				 hStatusBar,rect.left,
				 (rect.bottom)-cyVScroll,	//Above horizontal scroll bar
				 rect.right,
				 cyVScroll,
				 TRUE) ;
			  return 0;
		 }
	//system tray notification message
	 case KAVI_TRAY_MESSAGE:
		 //callback funtion for tray function
		 TrayNotification(hwnd, wParam, lParam);
		 return 0;

	//to send application to system tray when minimize button is pressed
	 case WM_ACTIVATE:
		 if(HIWORD(wParam)!=0&&LOWORD(wParam)==WA_INACTIVE)
			ShowWindow(hwnd,SW_HIDE);
		 return 0;

	  case WM_NOTIFY:
		 {
			 NMITEMACTIVATE *nmhdr=NULL;
			 nmhdr=(NMITEMACTIVATE *)lParam;
			 int itemclicked;
			 LRESULT i;
			 LVHITTESTINFO pinfo;
			 LVITEM LvItem={sizeof(LVITEM)};
			 if(hListViewControl==nmhdr->hdr.hwndFrom)
			 {
				 switch(nmhdr->hdr.code)
				 {
				 case NM_DBLCLK:
					 //when double clicked item info comes in NMITEMACTIVATE structure
					 //itemclicked is zero based index
					 //if its clicked on subitem then we need to send LVM_SUBITEMHITTEST message
					 //with mouse coordiantes filled in point structure of LVHITTESTINFO
						 pinfo.pt.x=nmhdr->ptAction.x;
						 pinfo.pt.y=nmhdr->ptAction.y;
						 i = SendMessage(hListViewControl,(UINT)  LVM_SUBITEMHITTEST ,0,(LPARAM) &pinfo );
						 itemclicked = pinfo.iItem;
						 if(-1==itemclicked)
						 {
							 ClickedItemData.DatainsideListBox=0;	//no item on list box
						 }
						 else
						 {
							 ClickedItemData.DatainsideListBox=1;
							 LvItem.mask=LVIF_TEXT;
							 LvItem.iItem=itemclicked;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szDate);
							 LvItem.pszText=ClickedItemData.szDate;
							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
							 LvItem.iSubItem=1;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szTime);
							 LvItem.pszText=ClickedItemData.szTime;
 							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
							 LvItem.iSubItem=2;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szReminder);
							 LvItem.pszText=ClickedItemData.szReminder;
							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
						 }
					 //here create a dialog box
					 DialogBoxW(::hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DialogProc);
					 return 0;
				 case NM_CLICK:
					 //Beep(100,500);
					 return 0;
				case NM_RCLICK:
					 //when double clicked item info comes in NMITEMACTIVATE structure
					 //itemclicked is zero based index
					 //if its clicked on subitem then we need to send LVM_SUBITEMHITTEST message
					 //with mouse coordiantes filled in point structure of LVHITTESTINFO
						 pinfo.pt.x=nmhdr->ptAction.x;
						 pinfo.pt.y=nmhdr->ptAction.y;
						 i = SendMessage(hListViewControl,(UINT)  LVM_SUBITEMHITTEST ,0,(LPARAM) &pinfo );
						 itemclicked = pinfo.iItem;
						 if(-1==itemclicked)
						 {
							 ClickedItemData.DatainsideListBox=0;	//no item on list box
							 RightClickMenuStatus=0;
						 }
						 else
						 {
							 ClickedItemData.DatainsideListBox=1;
							 LvItem.mask=LVIF_TEXT;
							 LvItem.iItem=itemclicked;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szDate);
							 LvItem.pszText=ClickedItemData.szDate;
							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
							 LvItem.iSubItem=1;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szTime);
							 LvItem.pszText=ClickedItemData.szTime;
 							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
							 LvItem.iSubItem=2;
							 LvItem.cchTextMax=sizeof(ClickedItemData.szReminder);
							 LvItem.pszText=ClickedItemData.szReminder;
							 i=SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&LvItem);
							 RightClickMenuStatus=1;
						 }
						 RightClickMenuonListView(hwnd);
					 return 0;

				 default:
					 return DefWindowProc (hwnd, message, wParam, lParam) ;
				 }
			 }
			 else
				 return DefWindowProc (hwnd, message, wParam, lParam) ;
		 }
	 }
}


BOOL CALLBACK DialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	default:
		return FALSE;

	case WM_INITDIALOG:
		FillClickedDatainDialogBox(hDlg);
		return TRUE;

	case WM_COMMAND:
		{
			//buttons on the dialog box
			switch(LOWORD(wParam))
			{
			case IDC_SAVE_REMINDER:
				SaveDialogButton(hDlg,hApplicationKey);
				//reset the structure
				ClickedItemData.DatainsideListBox=0;
				ZeroMemory(ClickedItemData.szDate,sizeof(ClickedItemData.szDate));
				ZeroMemory(ClickedItemData.szReminder,sizeof(ClickedItemData.szReminder));
				ZeroMemory(ClickedItemData.szTime,sizeof(ClickedItemData.szTime));
				UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
				return TRUE;
			case IDC_DELETE_REMINDER:
				DeleteDialogButton(hDlg,hApplicationKey);
				UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
				//reset the structure
				ClickedItemData.DatainsideListBox=0;
				ZeroMemory(ClickedItemData.szDate,sizeof(ClickedItemData.szDate));
				ZeroMemory(ClickedItemData.szReminder,sizeof(ClickedItemData.szReminder));
				ZeroMemory(ClickedItemData.szTime,sizeof(ClickedItemData.szTime));
				return TRUE;
			case IDC_CANCEL:
				SendMessage(hDlg,WM_CLOSE,0,0);
				UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
				return FALSE;
			default:
				return FALSE;
			}
		}
	case WM_CLOSE:
		EndDialog(hDlg,0);
		return FALSE;
	}
}


//create tray icon for the main window
void CreateTrayIcon(const HWND hwnd)
{
	HICON hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	IconData.hIcon=hIcon;
	IconData.hWnd=hwnd;
	lstrcpy(IconData.szTip,L"Reminder Application by Kavitesh Singh");
	IconData.uCallbackMessage=KAVI_TRAY_MESSAGE;
	IconData.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	Shell_NotifyIcon(NIM_ADD,&IconData);
}


//the call back function for the messages generated by the system tray icon
LRESULT CALLBACK TrayNotification(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	switch(lParam)
	{
		//for single left/right click on the system icon and pop menu is triggered
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		lstrcpy(IconData.szTip,L"");
		Shell_NotifyIcon(NIM_MODIFY,&IconData);
		hSystemTrayMenu=SystemTrayMenu(hwnd);
		lstrcpy(IconData.szTip,L"Reminder Application by Kavitesh Singh");
		Shell_NotifyIcon(NIM_MODIFY,&IconData);
		return 0;
		//for double left mouse click activate the main window
	case WM_LBUTTONDBLCLK:
		ShowWindow(hwnd,SW_SHOWNORMAL);
		return 0;
	}
	return 0;
}


//once clicked on listview then fill the dialog with
//either current time and disable delete button or
//fill with the data stored in CLICKEDITEMDATA struct
void FillClickedDatainDialogBox(const HWND hDlg)
{
	short hour=0,minute=0, AMP=0;
	short day=0, month=0, year=0;
	TCHAR szYear[7]=L"";
	//to initialize dialog box controls and fill with current time
	FillDefaultDatainDialogBox(hDlg);
	//ClickedItemData
	if(0==ClickedItemData.DatainsideListBox)
	{
		//data has been initialisedwith current time so just
		//diable delete button
		EnableWindow(GetDlgItem(hDlg,IDC_DELETE_REMINDER),FALSE);
     	return ;
	}
	else
	{
		EnableWindow(GetDlgItem(hDlg,IDC_DELETE_REMINDER),TRUE);
		TimeStringtoTimeComponents(ClickedItemData.szTime,&hour, &minute, &AMP);
		//for hour we need to subtract 1 for indexing
		SendDlgItemMessage(hDlg,IDC_TIME_HOURS,CB_SETCURSEL,(hour-1),0);
		SendDlgItemMessage(hDlg,IDC_TIME_MINUTES,CB_SETCURSEL,minute,0);
		SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_SETCURSEL,AMP,0);
		DateStringtoDateComponents(ClickedItemData.szDate,&day, &month, &year);
		SendDlgItemMessage(hDlg,IDC_DATE_DAY,CB_SETCURSEL,(day-1),0);
		SendDlgItemMessage(hDlg,IDC_DATE_MONTH,CB_SETCURSEL,(month-1),0);
		_itow(year,szYear,10);
		SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_SETCURSEL,SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_FINDSTRING,(WPARAM)-1,(LPARAM)szYear),0);
		SendDlgItemMessage(hDlg,IDC_REMINDER,WM_SETTEXT,0,(LPARAM)ClickedItemData.szReminder);
		return ;

	}
}



//the timer call back function and it returns void
VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
		switch(idEvent)
		 {
		 case ID_TIMER_1:
			 DisplayClockDateStatusBar(hStatusBar);
			 return ;
		 case ID_TIMER_2:
			 CompareCurrentTimewithRegistryDataTimer(hApplicationKey);
 			 UpdateListViewfromRegistry(hListViewControl,hApplicationKey);
			 return ;
		 default:
			 return ;
		 }
}


void CheckApplicationStartupWithWindows(const HWND hwnd,const HMENU hMenu)
{
	//HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
	HKEY hKey;
	LONG lResult=-1;
	DWORD nSubkeys=0;
	DWORD MaxSubKeyLen=0;
	DWORD nValues=0;
	DWORD MaxValueNameLen=0;
	DWORD nIndex=0;
	DWORD nValueName=0;
	TCHAR szValueName[MAX_VALUE_NAME]=L"";
	TCHAR szValueData[MAX_VALUE_NAME]=L"";
	DWORD nValueData=0;

	//the menu is at second position,zero based count
	HMENU hOptionMenu=GetSubMenu(hMenu,1);
	//By default the option will be unchecked, if found it will be checked
	//uncheck menu option in drop down is zero based and second one hence 1 for 2nd arguement
	lResult=CheckMenuItem(hOptionMenu,1,MF_BYPOSITION|MF_UNCHECKED);
	//ApplicationStartUpStatus variable
	ApplicationStartUpStatus=0;

	//open registry key where startup info is stored
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER, szRegistryStartApplicationPath,0,KEY_ALL_ACCESS,&hKey);
	if( ERROR_SUCCESS!=lResult)
	{
		MessageBox(hwnd,L"Error getting the startup information",L"StartUp Error",0);
		return ;
	}

	//Get the no of keys
	lResult= RegQueryInfoKey(hKey,NULL,NULL,NULL,&nSubkeys,&MaxSubKeyLen,NULL,&nValues,&MaxValueNameLen,
		NULL,NULL,NULL);
	if( lResult != ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		MessageBox(NULL,L"Error getting the startup information",L"StartUp Error",0);
		return ;
	}
	//to list all the value names in a subkey.
	if(nValues)
	{
		for(nIndex=0;nIndex<nValues;nIndex++)
		{
			//if ommited then only first key gets printed
			nValueName=MAX_VALUE_NAME;
			nValueData=MAX_VALUE_NAME;
			ZeroMemory(szValueName,sizeof(szValueName));
			ZeroMemory(szValueData,sizeof(szValueData));
			lResult=RegEnumValue(hKey,nIndex,szValueName,&nValueName,NULL,NULL,(BYTE*)szValueData,&nValueData);
			if (lResult == ERROR_SUCCESS)
			{
				lResult=lstrcmpi(szValueName,szAppName)&&lstrcmpi(szValueData,szRegistryStartApplicationPath);
				if(0==lResult)
				{
				//check menu option in drop down is zero based and first one hence 0 for 2nd arguement
				lResult=CheckMenuItem(hOptionMenu,1,MF_BYPOSITION|MF_CHECKED);
				//ApplicationStartUpStatus variable state
				ApplicationStartUpStatus=1;
				RegCloseKey(hKey);
				return ;
				}
			}
		}
	}
	RegCloseKey(hKey);
	return ;


}



BOOL CenterWindow(HWND hwnd)
{
    HWND hwndParent;
    RECT rect, rectP;
    int width, height;
    int screenwidth, screenheight;
    int x, y;

    //make the window relative to its desktop
    hwndParent = GetDesktopWindow();

    GetWindowRect(hwnd, &rect);
    GetWindowRect(hwndParent, &rectP);

    width  = rect.right  - rect.left;
    height = rect.bottom - rect.top;

    x = ((rectP.right-rectP.left) -  width) / 2 + rectP.left;
    y = ((rectP.bottom-rectP.top) - height) / 2 + rectP.top;

    screenwidth  = GetSystemMetrics(SM_CXSCREEN);
    screenheight = GetSystemMetrics(SM_CYSCREEN);

    //make sure that the dialog box never moves outside of
    //the screen
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(x + width  > screenwidth)  x = screenwidth  - width;
    if(y + height > screenheight) y = screenheight - height;

    MoveWindow(hwnd, x, y, width, height, FALSE);

    return TRUE;
}