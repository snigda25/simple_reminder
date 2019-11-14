#include "Reminder_APP.h"
#include "Dialog_Date.h"
#include "Status_List_Reg.h"
#include "resource.h"


extern struct CLICKEDITEMDATA ClickedItemData;


//convert a date string to integer values of day,month and year
//used for filling data in dialog box
void DateStringtoDateComponents(TCHAR *szDate,short *day, short *month, short *year)
{
	TCHAR *a=szDate;
	TCHAR szDay[3],szMonth[3],szYear[5];
	TCHAR temp[10]=L"";
	short i=0,j=0;
	short dm=0;
	
	while('\0'!=a[i])
	{
	if('-'==a[i])
	{
		if(0==dm)
		{
			lstrcpy(szDay,temp);
			ZeroMemory(temp,sizeof(temp));
			dm=1;
			i++;
			j=0;
			continue;
		}
		else
		{
			lstrcpy(szMonth,temp);
			ZeroMemory(temp,sizeof(temp));
			dm=0;
			i++;
			j=0;
			continue;
		}

	}
	temp[j]=a[i];
	j++;
	i++;
	}
	lstrcpy(szYear,temp);

	*day=(short)_wtoi(szDay);
	*month=(short)_wtoi(szMonth);
	*year=(short)_wtoi(szYear);


}


//to convert day,month and year in date string which can be stored in 
//registry and displayed in listview control
void DateComponentstoDateString(const short day,const short month,const short year,TCHAR *szDateString)
{
	TCHAR temp[10]=L"";
	ZeroMemory(szDateString,lstrlen(szDateString));

	_itow(day,temp,10);
	if(day>9)
	{
		lstrcpy(szDateString,temp);	
	}
	else
	{
		lstrcpy(szDateString,L"0");
		lstrcpy(szDateString,temp);
	}

	lstrcat(szDateString,L"-");

	ZeroMemory(temp,sizeof(temp));
	
	_itow(month,temp,10);

	if(month>9)
	{
		lstrcat(szDateString,temp);
	}
	else
	{
		lstrcat(szDateString,L"0");
		lstrcat(szDateString,temp);
	}
	
	lstrcat(szDateString,L"-");

	ZeroMemory(temp,sizeof(temp));
	_itow(year,temp,10);

	lstrcat(szDateString,temp);
}

//convert a time string to integer values of hour,minute and AM/PM
//used for filling data in dialog box
void TimeStringtoTimeComponents(TCHAR *szTime,short *hour, short *minute, short *AMP)
{
	TCHAR *a=szTime;
	TCHAR szHour[3],szMinute[3],szAMP[3]=L"AM";
	TCHAR temp[10]=L"";
	short i=0,j=0;
	short hm=0;
	

	while('\0'!=a[i])
	{
	if(':'==a[i])
	{
		if(0==hm)
		{
			lstrcpy(szHour,temp);
			ZeroMemory(temp,sizeof(temp));
			hm=1;
			i++;
			j=0;
			continue;
		}
		else
		{
			lstrcpy(szMinute,temp);
			ZeroMemory(temp,sizeof(temp));
			hm=0;
			i++;
			j=0;
			continue;
		}

	}
	temp[j]=a[i];
	j++;
	i++;
	}
	
	if(!lstrcmpi (szAMP,temp))
		*AMP=0;
	else
		*AMP=1;


	*hour=(short)_wtoi(szHour);
	*minute=(short)_wtoi(szMinute);
	
}

//to convert hour,minute and AM/PM in time string which can be stored in 
//registry and displayed in listview control
void TimeComponentstoTimeString(const short hour,const short minute,const short AMP,TCHAR *szTimeString)
{
	
	TCHAR temp[10]=L"";
	//TCHAR szTimeString[15]=L"";
	ZeroMemory(szTimeString,lstrlen(szTimeString));

	_itow(hour,temp,10);
	if(hour>9)
	{
		lstrcpy(szTimeString,temp);	
	}
	else
	{
		lstrcpy(szTimeString,L"0");
		lstrcat(szTimeString,temp);
	}
	
	lstrcat(szTimeString,L":");

	ZeroMemory(temp,sizeof(temp));
	_itow(minute,temp,10);
	if(minute>9)
	{
		lstrcat(szTimeString,temp);
	}
	else
	{
		lstrcat(szTimeString,L"0");
		lstrcat(szTimeString,temp);
	}
	
	lstrcat(szTimeString,L":");

	ZeroMemory(temp,sizeof(temp));
	if(0==AMP)
	lstrcat(szTimeString,L"AM");
	else 
	lstrcat(szTimeString,L"PM");
		
}

//initialise the drop down values in various input parameters in dialog box
int FillDefaultDatainDialogBox(const HWND hDlg)
{
	SYSTEMTIME SystemTime={sizeof(SYSTEMTIME)};
	int i=0;
	TCHAR szTemp[5]=L"";
	//set limit of edit box
	SendDlgItemMessage(hDlg,IDC_REMINDER,EM_LIMITTEXT ,(WPARAM)MAX_REMINDER_IN_EDIT_BOX,0);
	//set selection to current time
	GetLocalTime(&SystemTime);
	//fill day for date combo box
	for(i=1;i<=31;i++)
	{
		ZeroMemory(szTemp,sizeof(szTemp));
		_itow(i,szTemp,10);
		SendDlgItemMessage(hDlg,IDC_DATE_DAY,CB_ADDSTRING,0,(LPARAM)szTemp);
	}
	//fill month for date combo box
	for(i=1;i<=12;i++)
	{
		ZeroMemory(szTemp,sizeof(szTemp));
		_itow(i,szTemp,10);
		SendDlgItemMessage(hDlg,IDC_DATE_MONTH,CB_ADDSTRING,0,(LPARAM)szTemp);
	}
	//fill year(from current) for date combo box
	for(i=SystemTime.wYear;i<=(SystemTime.wYear+10);i++)
	{
		ZeroMemory(szTemp,sizeof(szTemp));
		_itow(i,szTemp,10);
		SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_ADDSTRING,0,(LPARAM)szTemp);
	}
	//fill hour for time combo box
	for(i=1;i<=12;i++)
	{
		ZeroMemory(szTemp,sizeof(szTemp));
		_itow(i,szTemp,10);
		SendDlgItemMessage(hDlg,IDC_TIME_HOURS,CB_ADDSTRING,0,(LPARAM)szTemp);
	}
	//fill minutes for time combo box
	for(i=0;i<=59;i++)
	{
		ZeroMemory(szTemp,sizeof(szTemp));
		_itow(i,szTemp,10);
		SendDlgItemMessage(hDlg,IDC_TIME_MINUTES,CB_ADDSTRING,0,(LPARAM)szTemp);
	}
	//fill am/pm for time combo box
	SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_ADDSTRING,0,(LPARAM)L"A.M.");
	SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_ADDSTRING,0,(LPARAM)L"P.M.");

	//setting time in 12-hr format
	short SystemHour=0,SystemAMP=0;

	if(SystemTime.wHour<12)
	{
		if(SystemTime.wHour==0)
			SystemHour=12;
		else
			SystemHour=SystemTime.wHour;
		SystemAMP=0;
	}
	else
	{
		if(SystemTime.wHour==12)
			SystemHour=12;
		else
			SystemHour=SystemTime.wHour-12;
		SystemAMP=1;
	}
	//setting focus to current time
	SendDlgItemMessage(hDlg,IDC_DATE_DAY,CB_SETCURSEL,SystemTime.wDay-1,0);
	SendDlgItemMessage(hDlg,IDC_DATE_MONTH,CB_SETCURSEL,SystemTime.wMonth-1,0);
    SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_SETCURSEL,0,0);
	SendDlgItemMessage(hDlg,IDC_TIME_HOURS,CB_SETCURSEL,SystemHour-1,0);
	SendDlgItemMessage(hDlg,IDC_TIME_MINUTES,CB_SETCURSEL,SystemTime.wMinute,0);
	SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_SETCURSEL,SystemAMP,0);
	return 0;
}



//once save button clicked on dialog
int SaveDialogButton(const HWND hDlg,const HKEY hKey)
{
	short hour=0,minute=0,AMP=0;
	TCHAR szTimeString[15]=L"";
	short day=0,month=0,year=0;
	TCHAR szDateString[15]=L"";
	TCHAR szBuffer[10]=L"";
	TCHAR szReminder[MAX_REMINDER_IN_EDIT_BOX+1]=L"";
	SYSTEMTIME SystemTime={sizeof(SYSTEMTIME)};
	LRESULT lResult=-1;
	short SystemHour=0,SystemMinute=0,SystemAMP=0;
	//Add one for zero based selection index.So adding 1 to get relevant integer value
	day = (short)SendDlgItemMessage(hDlg,IDC_DATE_DAY,CB_GETCURSEL,0,0)+1;
	month = (short)SendDlgItemMessage(hDlg,IDC_DATE_MONTH,CB_GETCURSEL,0,0)+1;
	//here we require to get year in string and then convert it to int
	lResult = SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_GETCURSEL,0,0);
	SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_GETLBTEXT,lResult,(LPARAM)szBuffer);
	year=(short)_wtoi(szBuffer);
	//check if data fed is correct
	GetLocalTime(&SystemTime);
	
	//Add one for zero based selection index.
	hour = (short)SendDlgItemMessage(hDlg,IDC_TIME_HOURS,CB_GETCURSEL,0,0)+1;
	minute = (short)SendDlgItemMessage(hDlg,IDC_TIME_MINUTES,CB_GETCURSEL,0,0);
	AMP = (short)SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_GETCURSEL,0,0);
	//setting time in 12-hr format
	if(SystemTime.wHour<12)
	{
		if(SystemTime.wHour==0)
			SystemHour=12;
		else
			SystemHour=SystemTime.wHour;
		SystemAMP=0;
	}
	else
	{
		if(SystemTime.wHour==12)
			SystemHour=12;
		else
			SystemHour=SystemTime.wHour-12;
		SystemAMP=1;
	}
	SystemMinute=SystemTime.wMinute;


	//check past date
	//for year we need not check as the year drop down will have no previous year in it
	if((month < SystemTime.wMonth)||((day < SystemTime.wDay)&&(month <= SystemTime.wMonth)))
	{
		MessageBox(hDlg,L"Incorrect Date Parameters",L"Date/Time Error",0);
		return FALSE;
	}

	//check for current date if same then see if time is past 
	if((day==SystemTime.wDay)&&(month==SystemTime.wMonth)&&(year==SystemTime.wYear))
	{
		//see past time, should be done after date check
		if(SystemAMP>AMP)
		{	
			MessageBox(hDlg,L"Incorrect Time Parameters",L"Date/Time Error",0);
			return FALSE;
		}
		if((SystemAMP==AMP)&&((hour<SystemHour)||((hour==SystemHour)&&(minute<=SystemMinute))))
		{
			MessageBox(hDlg,L"Incorrect Time Parameters",L"Date/Time Error",0);
			return FALSE;
		}
	}

	//february month
	if(day>28&&month==2)
	{
		MessageBox(hDlg,L"Incorrect Date Parameters",L"Date/Time Error",0);
		return FALSE;
	}
	//for checking 31 days for months
	if(day==31&&(!(month==1||month==3||month==5||month==7||month==8||month==10||month==12)))
	{
		MessageBox(hDlg,L"Incorrect Date Parameters",L"Date/Time Error",0);
		return FALSE;
	}
	
	//convert into time and date string
	TimeComponentstoTimeString(hour,minute,AMP,szTimeString);
	DateComponentstoDateString(day,month,year,szDateString);
	SendDlgItemMessage(hDlg,IDC_REMINDER,WM_GETTEXT,(WPARAM)sizeof(szReminder),(LPARAM)szReminder);
	//see if data is put in reminder section
	if(!lstrlen(szReminder))
	{
		MessageBox(hDlg,L"Invalid Reminder input",L"Reminder input error",0);
		return FALSE;
	}
	//check if data has been edited or new reminder is added
	if((!lstrcmpi(szTimeString,ClickedItemData.szTime))&&(!lstrcmpi(szDateString,ClickedItemData.szDate)))
		AddDatatoRegistry(hKey,szDateString,szTimeString,szReminder);//new data is added
	else
	{	
		//data is modified
		AddDatatoRegistry(hKey,szDateString,szTimeString,szReminder);
		if(ClickedItemData.DatainsideListBox)
			DeleteDatafromRegistry(hKey,ClickedItemData.szDate,ClickedItemData.szTime);
	}
	SendMessage(hDlg,WM_CLOSE,0,0);
	return TRUE;
}


//for delete button on dialog box
int DeleteDialogButton(const HWND hDlg,const HKEY hKey)
{
	short hour=0,minute=0,AMP=0;
	TCHAR szTimeString[15]=L"";
	short day=0,month=0,year=0;
	TCHAR szDateString[15]=L"";
	TCHAR szBuffer[10]=L"";
	LRESULT lResult=-1;

	//Add one for zero based selection index.So adding 1 to get relevant integer value
	day = (short)SendDlgItemMessage(hDlg,IDC_DATE_DAY,CB_GETCURSEL,0,0)+1;
	month = (short)SendDlgItemMessage(hDlg,IDC_DATE_MONTH,CB_GETCURSEL,0,0)+1;
	//here we require to get year in string and then convert it to int
	lResult = SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_GETCURSEL,0,0);
	SendDlgItemMessage(hDlg,IDC_DATE_YEAR,CB_GETLBTEXT,lResult,(LPARAM)szBuffer);
	year=(short)_wtoi(szBuffer);
	DateComponentstoDateString(day,month,year,szDateString);
	//Add one for zero based selection index.
	hour = (short)SendDlgItemMessage(hDlg,IDC_TIME_HOURS,CB_GETCURSEL,0,0)+1;
	minute = (short)SendDlgItemMessage(hDlg,IDC_TIME_MINUTES,CB_GETCURSEL,0,0);
	AMP = (short)SendDlgItemMessage(hDlg,IDC_TIME_AMP,CB_GETCURSEL,0,0);
	TimeComponentstoTimeString(hour,minute,AMP,szTimeString);
	DeleteDatafromRegistry(hKey,szDateString,szTimeString);
	SendMessage(hDlg,WM_CLOSE,0,0);
	return 0;
}
