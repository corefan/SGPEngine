#include <windows.h>
#include <CommCtrl.h>

#include "resource.h"
#include "ActionModifyDlg.h"
#include "MessageProcess.h"

extern HINSTANCE hInstance;

HWND ActionModifyDlg::m_HwndParent=NULL;

ActionModifyDlg::ActionModifyDlg(HWND hwnd)
{
	m_HwndParent=hwnd;
}

void ActionModifyDlg::DoModal()
{
	DialogBox(/*GetModuleHandle("MAX9Plugins.dlu")*/hInstance,\
		MAKEINTRESOURCE(IDD_ACTION_MODIFY_DLG),\
		m_HwndParent,\
		ActionModifyDlgProc);
}

INT_PTR CALLBACK ActionModifyDlg::ActionModifyDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		InitDlg(hWnd);
		InitCtrl(hWnd);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDB_ACTIONLIST_MODIFY:
			if(CheckModifiedAction(hWnd))
			{
				ModifySelectedAction(hWnd);
				EndDialog(hWnd,0);
			}
			break;
		case IDCANCEL:
			EndDialog(hWnd,-1);
			break;
		}
		break;
	};
	return FALSE;
}

void ActionModifyDlg::InitDlg(HWND hwnd)
{
	RECT rect;
	::GetWindowRect(::GetDlgItem(m_HwndParent,IDC_ACTION_LIST),&rect);
	POINT pt={rect.left,rect.top};
	RECT rcModifyDlg;
	::GetWindowRect(hwnd,&rcModifyDlg);
	int width=rcModifyDlg.right-rcModifyDlg.left;
	int height=rcModifyDlg.bottom-rcModifyDlg.top;
	rcModifyDlg.left=pt.x-(rcModifyDlg.right-rcModifyDlg.left);
	rcModifyDlg.top=pt.y;
	::MoveWindow(hwnd,rcModifyDlg.left,rcModifyDlg.top,width,height,TRUE);
}

void ActionModifyDlg::InitCtrl(HWND hwnd)
{
	HWND hList=GetDlgItem(m_HwndParent,IDC_ACTION_LIST);
	int nItem=ListView_GetNextItem(hList,-1,LVNI_SELECTED);
	int editID[]={IDC_ACTION_NAME,IDC_ACTION_STARTTIME,IDC_ACTION_ENDTIME,IDC_ACTION_FILENO};
	for(int i=0;i<4;++i)
	{
		char itemText[CHARLENGTH];
		ListView_GetItemText(hList,nItem,i,itemText,CHARLENGTH);
		SetDlgItemText(hwnd,editID[i],itemText);
	}
}


BOOL ActionModifyDlg::CheckModifiedAction(HWND hwnd)
{
	HWND hList=GetDlgItem(m_HwndParent,IDC_ACTION_LIST);
	char sActionName[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_NAME,sActionName,CHARLENGTH);
	int nStart=GetDlgItemInt(hwnd,IDC_ACTION_STARTTIME,NULL,FALSE);
	int nEnd=GetDlgItemInt(hwnd,IDC_ACTION_ENDTIME,NULL,FALSE);
	//check action name
	if(!MessageProcess::GetInstance()->CheckActionName(hwnd,sActionName)) return FALSE;
	//check starttime and endtime
	char sStart[CHARLENGTH],sEnd[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_STARTTIME,sStart,CHARLENGTH);
	GetDlgItemText(hwnd,IDC_ACTION_ENDTIME,sEnd,CHARLENGTH);
	if(strcmp(sStart,"")==0)
	{
		MessageBox(hwnd,"动作起始帧不能为空！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_STARTTIME));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_STARTTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	if(strcmp(sEnd,"")==0)
	{
		MessageBox(hwnd,"动作结束帧不能为空！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_ENDTIME));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_ENDTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	if(!MessageProcess::GetInstance()->CheckStartEndTime(hwnd,nStart,nEnd)) return FALSE;
	//check action file no.
	if(!CheckActionFileNo(hwnd)) return FALSE;

	//compare with existed actions
	int nSelectedItem=ListView_GetNextItem(hList,-1,LVNI_SELECTED);//被选中的项
	int nItemCount=ListView_GetItemCount(hList);
	for(int i=0;i<nItemCount;++i)
	{
		if(i==nSelectedItem) continue ;

		char sText[CHARLENGTH];
		ListView_GetItemText(hList,i,1,sText,CHARLENGTH);
		int start=atoi(sText);
		ListView_GetItemText(hList,i,2,sText,CHARLENGTH);
		int end=atoi(sText);
		ListView_GetItemText(hList,i,0,sText,CHARLENGTH);
		if(strcmp(sActionName,sText)==0)
		{
			MessageBox(hwnd,"动作名已经在动作列表中定义！\n请勿重复定义！","Error",MB_OK|MB_ICONERROR);
			SetFocus(GetDlgItem(hwnd,IDC_ACTION_NAME));
			SendMessage(GetDlgItem(hwnd,IDC_ACTION_NAME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
			return FALSE;
		}
		int leftmax=(nStart>=start?nStart:start);
		int rightmin=(nEnd<=end?nEnd:end);
		if(rightmin-leftmax>=0)
		{
			MessageBox(hwnd,"动作区间与动作列表中的动作有重叠，请修改！","Error",MB_OK|MB_ICONERROR);
			if(nStart>=start&&nStart<=end)
			{
				SetFocus(GetDlgItem(hwnd,IDC_ACTION_STARTTIME));
				SendMessage(GetDlgItem(hwnd,IDC_ACTION_STARTTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
			}
			if(nEnd>=start&&nEnd<=end)
			{
				SetFocus(GetDlgItem(hwnd,IDC_ACTION_ENDTIME));
				SendMessage(GetDlgItem(hwnd,IDC_ACTION_ENDTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
			}
			return FALSE;
		}
	}
	return TRUE;
}

BOOL ActionModifyDlg::CheckActionFileNo(HWND hwnd)
{
	HWND hList=GetDlgItem(m_HwndParent,IDC_ACTION_LIST);
	char sNo[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_FILENO,sNo,CHARLENGTH);
	if(strcmp(sNo,"")==0)
	{
		MessageBox(hwnd,"动作文件编号不能为空！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_FILENO));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_FILENO),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	//selected item
	int nSelectedItem=ListView_GetNextItem(hList,-1,LVNI_SELECTED);
	int nItemCount=ListView_GetItemCount(hList);
	int fileno[2]={0,0};
	if(nItemCount==1||nSelectedItem==0)
		fileno[0]=fileno[1]=0;
	else
	{
		if(nSelectedItem>0)
		{
			char sText[CHARLENGTH];
			ListView_GetItemText(hList,nSelectedItem-1,3,sText,CHARLENGTH);
			fileno[0]=atoi(sText);
		}
		if(nSelectedItem<nItemCount-1)
		{
			char sText[CHARLENGTH];
			ListView_GetItemText(hList,nSelectedItem+1,3,sText,CHARLENGTH);
			fileno[1]=atoi(sText);
		}
		else
			fileno[1]=fileno[0]+1;
	}
	if(fileno[1]-fileno[0]==2) fileno[0]=fileno[1]=fileno[1]-1;

	int currFileNo=GetDlgItemInt(hwnd,IDC_ACTION_FILENO,NULL,FALSE);
	if(currFileNo!=fileno[0]&&currFileNo!=fileno[1])
	{
		char info[100];
		if(fileno[0]!=fileno[1])
			sprintf_s(info,"当前输入的动作文件编号必须为%d或%d!",fileno[0],fileno[1]);
		else
			sprintf_s(info,"当前输入的动作文件编号必须为%d!",fileno[0]);
		MessageBox(hwnd,info,"Error",MB_OK);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_FILENO));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_FILENO),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}

	return TRUE;
}

void ActionModifyDlg::ModifySelectedAction(HWND hwnd)
{
	HWND hList=GetDlgItem(m_HwndParent,IDC_ACTION_LIST);
	int nSelectedItem=ListView_GetNextItem(hList,-1,LVNI_SELECTED);
	
	int editID[]={IDC_ACTION_NAME,IDC_ACTION_STARTTIME,IDC_ACTION_ENDTIME,IDC_ACTION_FILENO};
	for(int i=0;i<4;++i)
	{
		char sText[CHARLENGTH];
		GetDlgItemText(hwnd,editID[i],sText,CHARLENGTH);
		ListView_SetItemText(hList,nSelectedItem,i,sText);
	}
}
