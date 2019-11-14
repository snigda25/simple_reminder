#include "Status_List_Reg.h"
#include "Dialog_Date.h"
#include "Reminder_APP.h"
#include "resource.h"


//Global Data
TCHAR ApplicationRegKey[]=L"Software\\Kavitesh Singh - Reminder Application";
extern HWND hListViewControl;
extern int	ApplicationStartUpStatus;
extern int RightClickMenuStatus;


//create Menu for the application without using resource
HMENU MenuDisplay(const HWND hwnd)
{
	HMENU hMenu;
	HMENU hMenuPopup;
	//main menu bar
	hMenu=CreateMenu();
	if(hMenu==NULL)
	{
		MessageBox(NULL,TEXT("Menu Build Error"),TEXT("Error"),0);
		return FALSE;
	}
	//first drop down menu
	hMenuPopup = CreatePopupMenu () ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_ADD_REMINDER, TEXT("Add New Reminder")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_APP_HIDE,		TEXT("H&ide Application")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_EXIT,			TEXT("E&xit Application")) ;
	AppendMenu (hMenuPopup, MF_SEPARATOR, 0, NULL) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_ABOUT,		TEXT("A&bout Application")) ;
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR)hMenuPopup,			TEXT("&File")) ;
	//second drop down menu
	hMenuPopup=CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_DELETE_ALL_REMINDER,	TEXT("Delete All Reminders")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_STARTUP,	TEXT("Start at Logon")) ;
	AppendMenu (hMenu, MF_POPUP, (UINT_PTR)hMenuPopup,		TEXT("&Options")) ;
	if(!SetMenu(hwnd,hMenu))
		return (HMENU)-1;
	else 
		return hMenu;
	
}

//create pop-up menu when clicked on tray icon
HMENU SystemTrayMenu(const HWND hwnd)
{
	HMENU hMenuPopup;
	POINT pt;

	//directly create a pop up which will be displayed on right click. 
	//no main menu is required
	hMenuPopup=CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_ADD_REMINDER, TEXT("&Add New Reminder")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_APP_SHOW,		TEXT("S&how Application")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_APP_HIDE,		TEXT("H&ide Application")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_STARTUP,		TEXT("Start at Logon")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_EXIT,			TEXT("E&xit Application")) ;
	AppendMenu (hMenuPopup, MF_SEPARATOR, 0, NULL) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_APP_ABOUT,		TEXT("A&bout Application")) ;
	//for check to be displayed on system tray menu also
		if(ApplicationStartUpStatus)
			CheckMenuItem(hMenuPopup,3,MF_BYPOSITION|MF_CHECKED);
		else
			CheckMenuItem(hMenuPopup,3,MF_BYPOSITION|MF_UNCHECKED);

	//capture the point where mouse click has happened and display menu there.
	GetCursorPos(&pt);
	SetForegroundWindow(hwnd);
	TrackPopupMenu(hMenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
	//on right click on icon the system menu of windows use to pop up
	//so had send a null msg to maintain control after showing menu.
	PostMessage(hwnd, WM_NULL, 0, 0);
	return hMenuPopup;
}

//create right click menu on list view control
HMENU RightClickMenuonListView(const HWND hwnd)
{	
	HMENU hMenuPopup;
	POINT pt;

	//directly create a pop up which will be displayed on right click. 
	//no main menu is required
	hMenuPopup=CreatePopupMenu();
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_EDIT_REMINDER,TEXT("Edit Reminder")) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_DEL_REMINDER,TEXT("Delete Reminder")) ;
	AppendMenu (hMenuPopup, MF_SEPARATOR, 0, NULL) ;
	AppendMenu (hMenuPopup, MF_STRING,    IDM_SYS_ADD_REMINDER, TEXT("Add New Reminder")) ;
	
	//no data in listview or clicked on empty space
	if(RightClickMenuStatus==0)
	{
		EnableMenuItem(hMenuPopup,0,MF_BYPOSITION|MF_GRAYED);
		EnableMenuItem(hMenuPopup,1,MF_BYPOSITION|MF_GRAYED);
	}
	else
	{
		EnableMenuItem(hMenuPopup,0,MF_BYPOSITION|MF_ENABLED);
		EnableMenuItem(hMenuPopup,1,MF_BYPOSITION|MF_ENABLED);
	}
	//capture the point where mouse click has happened and display menu there.
	GetCursorPos(&pt);
	//SetForegroundWindow(hwnd);
	TrackPopupMenu(hMenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
	return hMenuPopup;

}





//create Status bar
HWND CreateStatusBar(const HWND hwnd)
{
	static int cyVScroll;
	LRESULT lResult;
	int i=3;
	int a[3]={100,200,-1};
	HWND hStatusBar=CreateStatusWindow(WS_CHILD|WS_VISIBLE ,L"",hwnd,ID_STATUS_BAR);
	lResult = SendMessage( hStatusBar, (UINT) SB_SIMPLE,(WPARAM) FALSE,0 );
	//create status bar with 3 parts and initialize it with blank text
	lResult = SendMessage(hStatusBar,(UINT) SB_SETPARTS,(WPARAM) i,(LPARAM) a);
	lResult = SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 0,(LPARAM) L"");
	lResult = SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 1,(LPARAM) L"");
	lResult = SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 2,(LPARAM) L"");
	return hStatusBar;
}

//Display time in the parts of status bar
void DisplayClockDateStatusBar(const HWND hStatusBar)
{
	LVITEM lvItem = {sizeof(LVITEM)};
	TCHAR szBuffer[260]=L"";
	TCHAR szSystemDate[20]=L"";
	TCHAR szSystemTime[20]=L"";
	GetTimeFormat(LOCALE_USER_DEFAULT,NULL,NULL,NULL,szSystemTime,sizeof(szSystemTime));
	GetDateFormat(LOCALE_USER_DEFAULT,NULL,NULL,NULL,szSystemDate,sizeof(szSystemDate));
	SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 0,(LPARAM) szSystemDate);
	SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 1,(LPARAM) szSystemTime);

	//to get first reminder in the list view control
	lvItem.mask=LVIF_TEXT;
	lvItem.iItem=0;
	lvItem.iSubItem=2;
	lvItem.cchTextMax=255;
	lvItem.pszText=szBuffer;

	if(SendMessage(hListViewControl,LVM_GETITEM,0,(LPARAM)&lvItem))
	{
				SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 2,(LPARAM) szBuffer);
	}
	else
		SendMessage(hStatusBar,(UINT) SB_SETTEXT,(WPARAM) 2,(LPARAM) L" Reminder Application by Kavitesh Singh");

}



HWND CreateListViewBox(const HINSTANCE hInstance,const HWND hwnd)
{
	LVCOLUMN LvCol; // Make Coluom struct for ListView
	LVITEM LvItem;  // ListView Item struct
	HWND hListControl=CreateWindowW(L"SysListView32",NULL,
		LVS_REPORT|WS_BORDER|WS_TABSTOP |WS_CHILD|LVS_SINGLESEL|LVS_NOSORTHEADER ,
		10,10,600,400,hwnd,NULL,hInstance,NULL);
	ListView_SetExtendedListViewStyle( hListControl,LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE );
	ShowWindow(hListControl,SW_SHOW);
	memset(&LvCol,0,sizeof(LvCol)); // Reset Coluom
	LvCol.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM; // Type of mask
	LvItem.iSubItem=0; 
	// Inserting Couloms as much as we want
	//column 1
	LvCol.cx=100;          // width between each coloum  
	LvCol.pszText=L"Date";                     // First Header
	SendMessage(hListControl,LVM_INSERTCOLUMN,0,(LPARAM)&LvCol); // Insert/Show the coloum
	//column 2
	LvCol.cx=100;
	LvCol.pszText=L"Time";                          // Next coloum
	SendMessage(hListControl,LVM_INSERTCOLUMN,1,(LPARAM)&LvCol); 
	//column 3
	LvCol.cx=400;
	LvCol.pszText=L"Reminder";                          // Next coloum
	SendMessage(hListControl,LVM_INSERTCOLUMN,2,(LPARAM)&LvCol); 
	return hListControl;
}


//a general function to fill the list view with all three parameters.
int AddListViewRow(const HWND hControl, TCHAR *szItemText, TCHAR *szSubItemText1,TCHAR *szSubItemText2)
{
	LVITEM LvItem;  // ListView Item struct
	DWORD  lResult=0;
	LvItem.mask=LVIF_TEXT;   // Text Style
	LvItem.cchTextMax = 256; // Max size of text
    LvItem.iItem= (int)SendMessage(hControl,LVM_GETITEMCOUNT,0,0); //add after last value  
	LvItem.iSubItem=0;       // Put in first coluom
	LvItem.pszText=szItemText; // Text to display (can be from a char variable) (Items)
    lResult=(int)SendMessage(hControl,LVM_INSERTITEM,0,(LPARAM)&LvItem); // Send to the Listview
	//to add data to subitem
	LvItem.iSubItem=1;       // Put in first coluom
	LvItem.pszText=szSubItemText1; // Text to display (can be from a char variable) (Items)
    lResult=(DWORD)SendMessage(hControl,LVM_SETITEM,0,(LPARAM)&LvItem); // Send to the Listview
	//to add data to subitem
	LvItem.iSubItem=2;       // Put in first coluom
	LvItem.pszText=szSubItemText2; // Text to display (can be from a char variable) (Items)
    lResult=(DWORD)SendMessage(hControl,LVM_SETITEM,0,(LPARAM)&LvItem); // Send to the Listview
	return TRUE;
}



//Create main registry key of application name
HKEY CreateApplicationKey()
{
	HKEY hKey;
	LONG lRet;
	lRet=RegCreateKeyEx(HKEY_CURRENT_USER,ApplicationRegKey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	if( lRet != ERROR_SUCCESS )
	{
		MessageBox(NULL,L"Error in Creating Registry Key for Application",L"Registry Error",0);
		PostQuitMessage(0);
		return FALSE;
	}
	else
		return hKey;
}

//general function to add registry data with a Key(szDate),KeyValue(szTime) and KeyValueData(szReminder)
int AddDatatoRegistry(const HKEY hKey, TCHAR *szDate,TCHAR *szTime,TCHAR *szReminder)
{
	HKEY hSubKey;
	int lRet;
	lRet=RegCreateKeyEx(hKey,szDate,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hSubKey,NULL);
	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hSubKey);
		MessageBox(NULL,L"Error in Creating Registry Key for Reminder",L"Registry Error",0);
		return FALSE;
	}
	lRet=RegSetValueEx(hSubKey,szTime,0,REG_SZ,(BYTE*)szReminder,(lstrlen(szReminder)+1)*sizeof(TCHAR));
	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hSubKey);
		MessageBox(NULL,L"Error in Creating Registry Key for Reminder",L"Registry Error",0);
		return FALSE;
	}
	RegCloseKey(hSubKey);
	return TRUE;
}


//general function to delete registry data with a Key(szDate),KeyValue(szTime).
//The KeyValue(szTime) gets deleted along with the data inside.
int DeleteDatafromRegistry(const HKEY hKey, TCHAR *szDate,TCHAR *szTime)
{
	HKEY hSubKey;
	int lRet;
	lRet=RegOpenKeyEx(hKey,szDate,0,KEY_ALL_ACCESS,&hSubKey);
	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hSubKey);
		//MessageBox(NULL,L"Error in Deleting Registry Key for Reminder",L"Registry Error",0);
		return FALSE;
	}
	lRet=RegDeleteValue(hSubKey,szTime);
	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hSubKey);
		//MessageBox(NULL,L"Error in Deleting Registry Key for Reminder",L"Registry Error",0);
		return FALSE;
	}
	RegCloseKey(hSubKey);
	return TRUE;

}


//search subkey,and value name in registry, if found return the value data.
int SearchRegistryString(const HKEY hKey,TCHAR *szSearchDate,TCHAR *szSearchTime,TCHAR *szReminderData)
{
	HKEY hSubKey;
	LONG lRet;
	DWORD nSubkeys=0;
	DWORD MaxSubKeyLen=0;
	DWORD nValues=0;
	DWORD MaxValueNameLen=0;
	DWORD nIndex=0;
	DWORD nValueName=0;
	TCHAR szValueName[MAX_VALUE_NAME]=L"";
	TCHAR szValueData[MAX_VALUE_NAME]=L"";
	DWORD nValueData=0;
	lRet=RegOpenKey(hKey,szSearchDate,&hSubKey);
	if( lRet != ERROR_SUCCESS )
		return FALSE;
	//Get the no of keys
	lRet= RegQueryInfoKey(hSubKey,NULL,NULL,NULL,&nSubkeys,&MaxSubKeyLen,NULL,&nValues,&MaxValueNameLen,
		NULL,NULL,NULL);
	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hSubKey);
		MessageBox(NULL,L"Error in Searching Registry Key for Reminder",L"Registry Error",0);
		return FALSE;
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
			lRet=RegEnumValue(hSubKey,nIndex,szValueName,&nValueName,NULL,NULL,(BYTE*)szValueData,&nValueData);
			if (lRet == ERROR_SUCCESS) 
			{
				lRet=lstrcmpi(szValueName,szSearchTime);
				if(0==lRet)
				{
					lstrcpy(szReminderData,szValueData);
					RegCloseKey(hSubKey);
					return TRUE;
				}
				else 
					ZeroMemory(szReminderData,lstrlen(szReminderData));//to reset garbage data if any data
			}
		}
	}
	RegCloseKey(hSubKey);
	return FALSE;
}


//enumerate all the data stored in registry and then add it to listview
int UpdateListViewfromRegistry(const HWND hListView,const HKEY hKey)
{
	HKEY hSubKey;
	LRESULT lRet;
	DWORD nSubKeys=0;
	DWORD nIndexMain=0;
	DWORD nIndexSub=0;
	TCHAR szDate[15]=L"";
	TCHAR szTime[15]=L"";
	TCHAR szReminder[MAX_VALUE_NAME]=L"";
	DWORD nszDate=0;
	DWORD nSubValues=0;
	
	lRet=SendMessage(hListView,LVM_DELETEALLITEMS,0,0);  
	//delete empty keys from registry
	DeleteEmptyKeysfromRegistry(hKey);
	//Get the no of keys
	lRet= RegQueryInfoKey(hKey,NULL,NULL,NULL,&nSubKeys,0,NULL,0,0,NULL,NULL,NULL);
	if( lRet != ERROR_SUCCESS )
	{
		MessageBox(NULL,L"Error in Updating Registry Data in List View Box",L"Registry Error",0);
		return FALSE;
	}
	if(nSubKeys)
	{
		for(nIndexMain=0;nIndexMain<nSubKeys;nIndexMain++)
		{
		//if ommited then only first key gets printed
			nszDate=MAX_KEY_LENGTH;
			lRet=RegEnumKeyEx(hKey,nIndexMain,szDate,&nszDate,NULL,NULL,NULL,NULL);
			if (lRet == ERROR_SUCCESS) 
			{
				lRet=RegOpenKey(hKey,szDate,&hSubKey);
				lRet= RegQueryInfoKey(hSubKey,NULL,NULL,NULL,NULL,NULL,NULL,&nSubValues,NULL,NULL,NULL,NULL);
				if(nSubValues)
				{
					for(nIndexSub=0;nIndexSub<nSubValues;nIndexSub++)
					{
						{
							//require these initilization else wont print subsequent data
							DWORD nszReminder=MAX_VALUE_NAME;
							DWORD nszTime=MAX_VALUE_NAME;

							lRet=RegEnumValue(hSubKey,nIndexSub,szTime,&nszTime,NULL,NULL,(BYTE*)szReminder,&nszReminder);
							if (lRet == ERROR_SUCCESS) 
								AddListViewRow(hListView, szDate, szTime, szReminder);
							ZeroMemory(szTime,sizeof(szTime));
							ZeroMemory(szReminder,sizeof(szReminder));
						}
					}
				}
				RegCloseKey(hSubKey);
			}
		ZeroMemory(szDate,sizeof(szDate));
		}
	}
	//for sorting the list-view box
	SendMessage( hListViewControl,(UINT) LVM_SORTITEMSEX,0,(LPARAM)SortingCodeListViewControl);  
	return TRUE;
}

//sorting list view using callback funtion
int CALLBACK SortingCodeListViewControl(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)  
{
	LVITEM lvItem={sizeof(LVITEM)};
	TCHAR szTemp[15]=L"";
	short nDay1=0,nDay2=0;
	short nMonth1=0,nMonth2=0;
	short nYear1=0,nYear2=0;
	short nHour1=0,nHour2=0;
	short nMin1=0,nMin2=0;
	short nAMP1=0,nAMP2=0;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = (int) lParam1;
	lvItem.pszText = szTemp;
	lvItem.cchTextMax = 14;
	//for item itself
	lvItem.iSubItem = 0;
	SendMessage(hListViewControl,(UINT) LVM_GETITEM,0,(LPARAM)&lvItem );  
	//get date for first item, lParam has the index of two items being compared
	DateStringtoDateComponents(szTemp,&nDay1, &nMonth1, &nYear1);
	ZeroMemory(szTemp,sizeof(szTemp));
	//for subitem
	lvItem.iSubItem = 1;
	SendMessage(hListViewControl,(UINT) LVM_GETITEM,0,(LPARAM)&lvItem );  
	//get time for first item, lParam has the index of two items being compared
	TimeStringtoTimeComponents(szTemp,&nHour1, &nMin1, &nAMP1);
	ZeroMemory(szTemp,sizeof(szTemp));
	//for second item now
	lvItem.iItem = (int) lParam2;
	//for item itself
	lvItem.iSubItem = 0;
	SendMessage(hListViewControl,(UINT) LVM_GETITEM,0,(LPARAM)&lvItem );  
	//get date for first item, lParam has the index of two items being compared
	DateStringtoDateComponents(szTemp,&nDay2, &nMonth2, &nYear2);
	ZeroMemory(szTemp,sizeof(szTemp));
	//for subitem
	lvItem.iSubItem = 1;
	SendMessage(hListViewControl,(UINT) LVM_GETITEM,0,(LPARAM)&lvItem );  
	//get time for first item, lParam has the index of two items being compared
	TimeStringtoTimeComponents(szTemp,&nHour2, &nMin2, &nAMP2);
	if((nDay1 == nDay2)&&(nMonth1 == nMonth2)&&(nYear1 == nYear2)&&(nHour1 == nHour2)&&(nMin1 == nMin2)&&(nAMP1 == nAMP2))
		return 0;  //no swap
	if(nYear1>nYear2)
		return 1;	//swap
	//date comparison
	if((nMonth2 < nMonth1)||((nDay2 < nDay1)&&(nMonth2 <= nMonth1)))
		return 1;
	//check for same date if same then see if time is different 
	if((nDay2==nDay1)&&(nMonth2==nMonth1)&&(nYear2==nYear1))
	{
		//see past time, should be done after date check
		if(nAMP1>nAMP2)
			return 1;	//swap for am and pm
		//time comparison
		if((nAMP1==nAMP2)&&(((nHour2<nHour1)||((nHour2==nHour1) && nMin2<=nMin1))))
			return 1;		
	}
	return 0; //no swap
}



int CompareCurrentTimewithRegistryDataTimer(const HKEY hKey)
{
	SYSTEMTIME SystemTime;
	TCHAR szSystemDate[20]=L"";
	TCHAR szSystemTime[20]=L"";
	TCHAR szReminder[260]=L"";
	short temp1=0,temp2=0,temp3=0;
	GetLocalTime(&SystemTime);
	//setting time in 12-hr format
	if(SystemTime.wHour<12)
	{
		if(SystemTime.wHour==0)
			temp1=12;
		else
			temp1=SystemTime.wHour;
		temp3=0;
	}
	else
	{
		if(SystemTime.wHour==12)
			temp1=12;
		else
			temp1=SystemTime.wHour-12;
		temp3=1;
	}
	temp2=SystemTime.wMinute;
	TimeComponentstoTimeString(temp1,temp2,temp3,szSystemTime);
	temp1=temp2=temp3=0;
	temp1=SystemTime.wDay;
	temp2=SystemTime.wMonth;
	temp3=SystemTime.wYear;
	DateComponentstoDateString(temp1,temp2,temp3,szSystemDate);
	//once systemtime/date is converted into registry stored format
	//it is searched in registry, if found the reminder is displayed
	if(SearchRegistryString(hKey,szSystemDate,szSystemTime,szReminder))
	{
		for(int i=100;i<150;i++)
			Beep(i+10,25);
		MessageBox(NULL,szReminder,L"Reminder",0);
		//reminder that is triggered is deleted and listview is 
		//updated with new values
		DeleteDatafromRegistry(hKey,szSystemDate,szSystemTime);
		UpdateListViewfromRegistry(hListViewControl,hKey);
		return TRUE;
	}
	return FALSE;
}


//this searches for any empty keys stored and deletes so that
//registry doesnt exceed nnecessarily 
int DeleteEmptyKeysfromRegistry(const HKEY hKey)
{
	HKEY hSubKey;
	LRESULT lRet;
	DWORD nSubKeys=0;
	DWORD nIndexMain=0;
	TCHAR szDate[15]=L"";
	DWORD nszDate=0;
	DWORD nSubValues=0;
	//Get the no of keys
	lRet= RegQueryInfoKey(hKey,NULL,NULL,NULL,&nSubKeys,0,NULL,0,0,NULL,NULL,NULL);
	if( lRet != ERROR_SUCCESS )
	{
		MessageBox(NULL,L"Error in Updating Registry Data in List View Box",L"Registry Error",0);
		return FALSE;
	}
	if(nSubKeys)
	{
		for(nIndexMain=0;nIndexMain<nSubKeys;nIndexMain++)
		{
		//if ommited then only first key gets printed
			nszDate=MAX_KEY_LENGTH;
			lRet=RegEnumKeyEx(hKey,nIndexMain,szDate,&nszDate,NULL,NULL,NULL,NULL);
			if (lRet == ERROR_SUCCESS) 
			{
				lRet=RegOpenKey(hKey,szDate,&hSubKey);
				lRet= RegQueryInfoKey(hSubKey,NULL,NULL,NULL,NULL,NULL,NULL,&nSubValues,NULL,NULL,NULL,NULL);
				if(!nSubValues)
				{
					RegCloseKey(hSubKey);
					lRet=RegDeleteKey(hKey,szDate);					
				}
				else
					RegCloseKey(hSubKey);
			}
		ZeroMemory(szDate,sizeof(szDate));
		}
	}
	return TRUE;
}
