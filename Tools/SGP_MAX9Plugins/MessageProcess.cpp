#include <windows.h>
#include <CommCtrl.h>

#include "resource.h"
#include "MessageProcess.h"
#include "ActionModifyDlg.h"
#include "FolderDialog.h"
#include "SGP_MAX9ControlPanel.h"
#include "SGP_MAX9Interface.h"

extern HINSTANCE hInstance;
//static member initialize
MessageProcess* MessageProcess::s_instance=NULL;

MessageProcess* MessageProcess::GetInstance()
{
	if(s_instance==NULL)
		s_instance=new MessageProcess;
	return s_instance;
}

void MessageProcess::ReleaseInstance()
{
	if(s_instance!=NULL)
	{
		s_instance->ReleaseResource();
		delete s_instance;
		s_instance=NULL;
	}
}

MessageProcess::MessageProcess()
{
	m_hToolTip=NULL;
}

void MessageProcess::InitDlg(HWND hwnd)
{
	SetWindowText(hwnd,"SGPEngine-Export Setting");//设置窗口标题
	//设置窗口位置
	int width=GetSystemMetrics(SM_CXSCREEN);
	int height=GetSystemMetrics(SM_CYSCREEN);
	RECT rc;
	::GetWindowRect(hwnd,&rc);
	::MoveWindow(hwnd,(width-rc.right+rc.left)/2,(height-rc.bottom+rc.top)/2,\
		rc.right-rc.left,rc.bottom-rc.top,TRUE);
}

void MessageProcess::InitCtrl(HWND hwnd)
{
	//初始化动作列表框
	HWND hList=::GetDlgItem(hwnd,IDC_ACTION_LIST);
	ListView_SetExtendedListViewStyleEx(hList,0,LVS_EX_FULLROWSELECT);
	RECT rect;
	::GetWindowRect(hList,&rect);
	LVCOLUMN lv;
	lv.cx=(rect.right-rect.left)/4-1;
	lv.pszText="Action";
	lv.mask=LVCF_WIDTH|LVCF_TEXT|LVCF_MINWIDTH;
	ListView_InsertColumn(hList,0,&lv);
	lv.pszText="Start";
	ListView_InsertColumn(hList,1,&lv);
	lv.pszText="End";
	ListView_InsertColumn(hList,2,&lv);
	lv.pszText="FileNo.";
	ListView_InsertColumn(hList,3,&lv);

	//initialize tooltip control
	CreateToolTipCtrl(hwnd);
	AddToolTip(hwnd,IDB_SET_WORKDIR,"存放数据的Data文件夹");
	//set tooltip delay time
	::SendMessage(m_hToolTip,TTM_SETDELAYTIME,(WPARAM)TTDT_AUTOMATIC,(LPARAM)150);

	//设置文件编号默认值为0
	::SetDlgItemText(hwnd,IDC_ACTION_FILENO,"0");
}

void MessageProcess::CreateToolTipCtrl(HWND hwnd)
{
	if(m_hToolTip!=NULL)
		DestroyWindow(m_hToolTip);
	m_hToolTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,\
		WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON,\
		CW_USEDEFAULT, CW_USEDEFAULT,\
		CW_USEDEFAULT, CW_USEDEFAULT,\
		hwnd, NULL, \
		hInstance, NULL);
}

void MessageProcess::AddToolTip(HWND hwnd,int nCtrlID,PTSTR pszText)
{
	TOOLINFO toolInfo = {0};
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hwnd;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)GetDlgItem(hwnd,nCtrlID);
	toolInfo.lpszText = pszText;
	SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
}

void MessageProcess::ShowToolTips(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	MSG msg;
	msg.hwnd=hwnd;
	msg.message=uMsg;
	msg.wParam=wParam;
	msg.lParam=lParam;
	msg.time=GetTickCount();
	POINT pt;
	GetCursorPos(&pt);
	msg.pt=pt;
	if(m_hToolTip!=NULL)
		SendMessage(m_hToolTip,TTM_RELAYEVENT,(WPARAM)0,(LPARAM)&msg);
}

void MessageProcess::DrawExportButton(LPARAM lParam)
{
	DRAWITEMSTRUCT* pStruct=(DRAWITEMSTRUCT*)lParam;
	HDC hDC=pStruct->hDC;
	RECT rc=pStruct->rcItem;
	if(pStruct->itemState&ODS_SELECTED)
	{
		HBRUSH hBrush=CreateSolidBrush(RGB(172,172,172));
		::FillRect(hDC,&rc,hBrush);
		DeleteObject(hBrush);
	}
	else
	{
		HBRUSH hBrush=CreateSolidBrush(RGB(222,222,222));
		::FillRect(hDC,&rc,hBrush);
		DeleteObject(hBrush);
	}
	HBRUSH hBrush=CreateSolidBrush(RGB(142,142,142));
	FrameRect(hDC,&rc,hBrush);
	DeleteObject(hBrush);

	HFONT hFont=CreateFont(150,0,0,0,FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,\
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"微软雅黑");
	HFONT hOldFont=(HFONT)SelectObject(hDC,hFont);
	SetBkMode(hDC,TRANSPARENT);
	DrawText(hDC,"导           出",strlen("导           出"),&rc,DT_CENTER|DT_VCENTER);
	SelectObject(hDC,hOldFont);
	DeleteObject(hFont);
}

void MessageProcess::AddAction(HWND hwnd)
{
	if(!CheckInputAction(hwnd)) return ;
	int insertPos=GetInsertPos(hwnd);
	if(!CheckActionFileNo(hwnd,insertPos)) return ;

	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	//获取编辑框输入的动作信息
	char sActionName[CHARLENGTH],sStart[CHARLENGTH],sEnd[CHARLENGTH],sNo[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_NAME,sActionName,CHARLENGTH);
	GetDlgItemText(hwnd,IDC_ACTION_STARTTIME,sStart,CHARLENGTH);
	GetDlgItemText(hwnd,IDC_ACTION_ENDTIME,sEnd,CHARLENGTH);
	GetDlgItemText(hwnd,IDC_ACTION_FILENO,sNo,CHARLENGTH);

	//向动作列表中插入动作信息
	int nItemCount=ListView_GetItemCount(hList);
	if(insertPos==nItemCount)
	{
		LVITEM lvItem;
		lvItem.mask=LVIF_TEXT;
		lvItem.pszText=sActionName;
		lvItem.iItem=insertPos;
		lvItem.iSubItem=0;
		ListView_InsertItem(hList,&lvItem);
		ListView_SetItemText(hList,insertPos,1,sStart);
		ListView_SetItemText(hList,insertPos,2,sEnd);
		ListView_SetItemText(hList,insertPos,3,sNo);
	}
	else
	{
		LVITEM lvItem;
		lvItem.mask=LVIF_TEXT;
		lvItem.pszText=" ";
		lvItem.iItem=nItemCount;
		lvItem.iSubItem=0;
		ListView_InsertItem(hList,&lvItem);
		for(int i=nItemCount;i>insertPos;--i)
		{
			for(int j=0;j<4;++j)
			{
				char str[CHARLENGTH];
				ListView_GetItemText(hList,i-1,j,str,CHARLENGTH);
				ListView_SetItemText(hList,i,j,str);
			}
		}
		ListView_SetItemText(hList,insertPos,0,sActionName);
		ListView_SetItemText(hList,insertPos,1,sStart);
		ListView_SetItemText(hList,insertPos,2,sEnd);
		ListView_SetItemText(hList,insertPos,3,sNo);
	}

	//清空编辑框
	SetDlgItemText(hwnd,IDC_ACTION_NAME,"");
	//下次开始时间=上次结束时间+1
	int lastime=GetDlgItemInt(hwnd,IDC_ACTION_ENDTIME,NULL,FALSE);
	SetDlgItemInt(hwnd,IDC_ACTION_STARTTIME,lastime+1,FALSE);

	SetDlgItemText(hwnd,IDC_ACTION_ENDTIME,"");
}

int MessageProcess::GetInsertPos(HWND hwnd)
{
	int index=0;
	int starttime=GetDlgItemInt(hwnd,IDC_ACTION_STARTTIME,NULL,FALSE);
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	int nItemCount=ListView_GetItemCount(hList);
	for(int i=0;i<nItemCount;++i)
	{
		char str[CHARLENGTH];
		ListView_GetItemText(hList,i,1,str,CHARLENGTH);
		int itemTime=atoi(str);
		if(itemTime<starttime)
		{
			++index;
		}
		else
		{
			break;
		}
	}
	return index;
}

BOOL MessageProcess::CheckActionFileNo(HWND hwnd,int pos)
{
	//can't be empty
	char strNo[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_FILENO,strNo,CHARLENGTH);
	if(strcmp(strNo,"")==0)
	{
		MessageBox(hwnd,"动作文件编号不能为空！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_FILENO));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_FILENO),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}

	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	int nItemCount=ListView_GetItemCount(hList);
	int fileno[2];
	char text[CHARLENGTH];
	if(nItemCount==0||pos==0)
	{
		fileno[0]=fileno[1]=0;
	}
	else if(pos<nItemCount)
	{		
		ListView_GetItemText(hList,pos-1,3,text,CHARLENGTH);
		fileno[0]=atoi(text);
		ListView_GetItemText(hList,pos,3,text,CHARLENGTH);
		fileno[1]=atoi(text);
	}
	else
	{
		ListView_GetItemText(hList,nItemCount-1,3,text,CHARLENGTH);
		fileno[0]=atoi(text);
		fileno[1]=fileno[0]+1;
	}


	int inputno=GetDlgItemInt(hwnd,IDC_ACTION_FILENO,NULL,FALSE);
	if(inputno!=fileno[0]&&inputno!=fileno[1])
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

BOOL MessageProcess::CheckInputAction(HWND hwnd)
{
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	char sActionName[CHARLENGTH];
	GetDlgItemText(hwnd,IDC_ACTION_NAME,sActionName,CHARLENGTH);
	int nStart=GetDlgItemInt(hwnd,IDC_ACTION_STARTTIME,NULL,FALSE);
	int nEnd=GetDlgItemInt(hwnd,IDC_ACTION_ENDTIME,NULL,FALSE);
	//check action name
	if(!CheckActionName(hwnd,sActionName)) return FALSE;
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
	if(!CheckStartEndTime(hwnd,nStart,nEnd)) return FALSE;
	//compare with existed action
	int nItemCount=ListView_GetItemCount(hList);
	for(int i=0;i<nItemCount;++i)
	{
		char sName[CHARLENGTH],sStart[CHARLENGTH],sEnd[CHARLENGTH];
		ListView_GetItemText(hList,i,0,sName,CHARLENGTH);
		ListView_GetItemText(hList,i,1,sStart,CHARLENGTH);
		ListView_GetItemText(hList,i,2,sEnd,CHARLENGTH);
		int start=atoi(sStart);
		int end=atoi(sEnd);
		if(strcmp(sActionName,sName)==0)
		{
			MessageBox(hwnd,"动作名已经定义，请勿重复定义！","Error",MB_OK|MB_ICONERROR);
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

BOOL MessageProcess::CheckStartEndTime(HWND hwnd,int nStart,int nEnd)
{
	if(nStart>=nEnd)
	{
		MessageBox(hwnd,"动作起始时间必须小于终止时间！","Error",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	int startFrame=g_SGPMax9CtrlPanel.GetMaxIP()->GetStartFrame();
	int endFrame=g_SGPMax9CtrlPanel.GetMaxIP()->GetEndFrame();
	if(nStart<startFrame||nStart>endFrame)
	{
		MessageBox(hwnd,"动作起始时间超出范围！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_STARTTIME));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_STARTTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	if(nEnd>endFrame||nEnd<startFrame)
	{
		MessageBox(hwnd,"动作终止时间超出范围！","Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_ENDTIME));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_ENDTIME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	return TRUE;
}

BOOL MessageProcess::CheckActionName(HWND hwnd,char* str)
{
	/*action name must be lowercase、number、underline or their combination*/
	if(strcmp(str,"")==0)
	{
		MessageBox(hwnd,"动作文件名不能为空","Error",MB_OK);
		SetFocus(GetDlgItem(hwnd,IDC_ACTION_NAME));
		SendMessage(GetDlgItem(hwnd,IDC_ACTION_NAME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}

	for(int i=0;str[i]!='\0';++i)
	{
		char c=str[i];
		if((c>='0'&&c<='9')||(c>='a'&&c<='z')||c=='_'||c=='-')
		{}
		else
		{
			MessageBox(hwnd,"动作文件名必须是小写字母、数字或下划线的组合！","Error",MB_OK);
			SetFocus(GetDlgItem(hwnd,IDC_ACTION_NAME));
			SendMessage(GetDlgItem(hwnd,IDC_ACTION_NAME),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
			return FALSE;
		}
	}
	return TRUE;
}

void MessageProcess::DeleteAction(HWND hwnd)
{
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	if(ListView_GetSelectedCount(hList)==0)
	{
		MessageBox(hwnd,"请在动作列表中至少选中一项，再进行删除操作","Tips",MB_OK);
		return ;
	}
	int nItem;
	while((nItem=ListView_GetNextItem(hList,-1,LVNI_SELECTED))!=-1)
		ListView_DeleteItem(hList,nItem);
}

void MessageProcess::ClearAction(HWND hwnd)
{
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	ListView_DeleteAllItems(hList);
}

void MessageProcess::ModifyAction(HWND hwnd,BOOL bCheck)
{
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	int nSelectedCount=ListView_GetSelectedCount(hList);
	if(nSelectedCount==0)
	{
		if(bCheck)
			MessageBox(hwnd,"请至少从动作列表中选中一项进行修改！","Tips",MB_OK|MB_ICONINFORMATION);
		return ;
	}
	if(nSelectedCount>1)
	{
		if(bCheck)
			MessageBox(hwnd,"只能从动作列表中选中一项进行修改！","Tips",MB_OK|MB_ICONINFORMATION);
		return ;
	}
	ActionModifyDlg dlg(hwnd);
	dlg.DoModal();
}

void MessageProcess::SetWorkDir(HWND hwnd)
{
	char strPath[MAX_PATH];
	GetDlgItemText(hwnd,IDE_WORK_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath)) strcpy(strPath,"");
	FolderDialog dlg(hwnd,"设置工作目录",strPath);
	dlg.DoModal();
	if(dlg.IsDirectory())
		SetDlgItemText(hwnd,IDE_WORK_DIR,dlg.GetPath());
}

void MessageProcess::SetMeshDir(HWND hwnd)
{
	char strPath[MAX_PATH];
	GetDlgItemText(hwnd,IDE_MESH_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		GetDlgItemText(hwnd,IDE_WORK_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		strcpy(strPath,"");
	FolderDialog dlg(hwnd,"设置Mesh存放目录",strPath);
	dlg.DoModal();
	if(dlg.IsDirectory())
		SetDlgItemText(hwnd,IDE_MESH_DIR,dlg.GetPath());
}

void MessageProcess::SetBoneDir(HWND hwnd)
{
	char strPath[MAX_PATH];
	GetDlgItemText(hwnd,IDE_BONE_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		GetDlgItemText(hwnd,IDE_WORK_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		strcpy(strPath,"");
	FolderDialog dlg(hwnd,"设置骨骼存放目录",strPath);
	dlg.DoModal();
	if(dlg.IsDirectory())
		SetDlgItemText(hwnd,IDE_BONE_DIR,dlg.GetPath());
}

void MessageProcess::SetTextureDir(HWND hwnd)
{
	char strPath[MAX_PATH];
	GetDlgItemText(hwnd,IDE_TEXTURE_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		GetDlgItemText(hwnd,IDE_WORK_DIR,strPath,MAX_PATH);
	if(!FolderDialog::IsDirectory(strPath))
		strcpy(strPath,"");
	FolderDialog dlg(hwnd,"设置Texture存放目录",strPath);
	dlg.DoModal();
	if(dlg.IsDirectory())
		SetDlgItemText(hwnd,IDE_TEXTURE_DIR,dlg.GetPath());
}

void MessageProcess::SaveSettings(HWND hwnd)
{
	//save dir to variables
	GetDlgItemText(hwnd,IDE_WORK_DIR,m_WorkDir,MAX_PATH);
	GetDlgItemText(hwnd,IDE_BONE_DIR,m_BoneDir,MAX_PATH);
	GetDlgItemText(hwnd,IDE_MESH_DIR,m_MeshDir,MAX_PATH);
	GetDlgItemText(hwnd,IDE_TEXTURE_DIR,m_TextureDir,MAX_PATH);
	//save option to variables
	m_bStaticMesh=IsDlgButtonChecked(hwnd,IDC_USE_STATIC_MESH);
	m_bUseVertexCompress=IsDlgButtonChecked(hwnd,IDC_USE_VERTEX_COMPRESS);
	m_bUseAnimationCompress=IsDlgButtonChecked(hwnd,IDC_USE_ANIMATION_COMPRESS);
	m_bUseMaterialCompress=IsDlgButtonChecked(hwnd,IDC_USE_MATERIAL_COMPRESS);
	m_bExportUV2=IsDlgButtonChecked(hwnd,IDC_EXPORT_UV2);
	m_bExportVertexColor=IsDlgButtonChecked(hwnd,IDC_EXPORT_VERTEX_COLOR);
	char zoomRatio[20];
	GetDlgItemText(hwnd,IDE_ZOOM_RATIO,zoomRatio,20);
	m_fZoomRatio=(float)atof(zoomRatio);//retrieve zoom ratio
	//save actionlist to variables
	m_ActionList.clear();
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	int nNum=ListView_GetItemCount(hList);
	for(int i=0;i<nNum;++i)
	{
		ActionItemType actionItem;
		char sText[CHARLENGTH];
		ListView_GetItemText(hList,i,0,sText,CHARLENGTH);
		strcpy(actionItem.actionName,sText);
		ListView_GetItemText(hList,i,1,sText,CHARLENGTH);
		actionItem.startFrameID=atoi(sText);
		ListView_GetItemText(hList,i,2,sText,CHARLENGTH);
		actionItem.endFrameID=atoi(sText);
		ListView_GetItemText(hList,i,3,sText,CHARLENGTH);
		actionItem.fileNo=atoi(sText);
		m_ActionList.push_back(actionItem);
	}

	//save to file
	SaveSettingsToFile();
}

void MessageProcess::LoadSettings(HWND hwnd)
{
	LoadSettingsFromFile();

	//set dir
	SetDlgItemText(hwnd,IDE_WORK_DIR,m_WorkDir);
	SetDlgItemText(hwnd,IDE_MESH_DIR,m_MeshDir);
	SetDlgItemText(hwnd,IDE_BONE_DIR,m_BoneDir);
	SetDlgItemText(hwnd,IDE_TEXTURE_DIR,m_TextureDir);
	//setting option
	CheckDlgButton(hwnd,IDC_USE_STATIC_MESH,m_bStaticMesh);
	CheckDlgButton(hwnd,IDC_USE_VERTEX_COMPRESS,m_bUseVertexCompress);
	CheckDlgButton(hwnd,IDC_USE_ANIMATION_COMPRESS,m_bUseAnimationCompress);
	CheckDlgButton(hwnd,IDC_USE_MATERIAL_COMPRESS,m_bUseMaterialCompress);
	CheckDlgButton(hwnd,IDC_EXPORT_UV2,m_bExportUV2);
	CheckDlgButton(hwnd,IDC_EXPORT_VERTEX_COLOR,m_bExportVertexColor);
	char zoomratio[20];
	sprintf_s(zoomratio,"%f",m_fZoomRatio);
	int len=strlen(zoomratio);
	while(zoomratio[--len]=='0') zoomratio[len]='\0';//trim 0
	SetDlgItemText(hwnd,IDE_ZOOM_RATIO,zoomratio);//set zoom ratio
	//load actionlist
	HWND hList=GetDlgItem(hwnd,IDC_ACTION_LIST);
	ListView_DeleteAllItems(hList);
	int nNum=m_ActionList.size();
	for(int i=0;i<nNum;++i)
	{
		ActionItemType actionItem=m_ActionList[i];
		char str[CHARLENGTH];
		LVITEM lvItem;
		lvItem.mask=LVIF_TEXT;
		lvItem.pszText=actionItem.actionName;
		lvItem.iItem=i;
		lvItem.iSubItem=0;
		ListView_InsertItem(hList,&lvItem);
		sprintf_s(str,"%d",actionItem.startFrameID);
		ListView_SetItemText(hList,i,1,str);
		sprintf_s(str,"%d",actionItem.endFrameID);
		ListView_SetItemText(hList,i,2,str);
		sprintf_s(str,"%d",actionItem.fileNo);
		ListView_SetItemText(hList,i,3,str);
	}
}

void MessageProcess::SaveSettingsToFile()
{
	char strPath[MAX_PATH];
	GetSettingFilePath(strPath);
	DeleteFile(strPath);
	//save path
	WritePrivateProfileString("directory","workDir",m_WorkDir,strPath);
	WritePrivateProfileString("directory","boneDir",m_BoneDir,strPath);
	WritePrivateProfileString("directory","meshDir",m_MeshDir,strPath);
	WritePrivateProfileString("directory","textureDir",m_TextureDir,strPath);

	//save option
	char str[10];
	sprintf_s(str,"%d",m_bStaticMesh);
	WritePrivateProfileString("output","useStaticMesh",str,strPath);
	sprintf_s(str,"%f",m_fZoomRatio);
	WritePrivateProfileString("output","zoomRatio",str,strPath);//zoom ratio
	sprintf_s(str,"%d",m_bUseVertexCompress);
	WritePrivateProfileString("compress","useVertexCompress",str,strPath);
	sprintf_s(str,"%d",m_bUseAnimationCompress);
	WritePrivateProfileString("compress","useAnimationCompress",str,strPath);
	sprintf_s(str,"%d",m_bUseMaterialCompress);
	WritePrivateProfileString("compress","useMaterialCompress",str,strPath);
	sprintf_s(str,"%d",m_bExportUV2);
	WritePrivateProfileString("option","exportUV2",str,strPath);
	sprintf_s(str,"%d",m_bExportVertexColor);
	WritePrivateProfileString("option","exportVertexColor",str,strPath);

	//save actionlist
	int nNum=m_ActionList.size();
	for(int i=0;i<nNum;++i)
	{
		ActionItemType actionItem=m_ActionList[i];
		char strKey[CHARLENGTH],strValue[CHARLENGTH];

		sprintf_s(strKey,"action%d",i);
		WritePrivateProfileString("actionlist",strKey,actionItem.actionName,strPath);
		sprintf_s(strKey,"startframe%d",i);
		sprintf_s(strValue,"%d",actionItem.startFrameID);
		WritePrivateProfileString("actionlist",strKey,strValue,strPath);
		sprintf_s(strKey,"endframe%d",i);
		sprintf_s(strValue,"%d",actionItem.endFrameID);
		WritePrivateProfileString("actionlist",strKey,strValue,strPath);
		sprintf_s(strKey,"fileno%d",i);
		sprintf_s(strValue,"%d",actionItem.fileNo);
		WritePrivateProfileString("actionlist",strKey,strValue,strPath);
	}
}

void MessageProcess::LoadSettingsFromFile()
{
	char strPath[MAX_PATH];
	GetSettingFilePath(strPath);
	//load path[section directory]
	char path[MAX_PATH];
	GetPrivateProfileString("directory","workDir","",path,MAX_PATH,strPath);
	strcpy(m_WorkDir,path);
	GetPrivateProfileString("directory","boneDir","",path,MAX_PATH,strPath);
	strcpy(m_BoneDir,path);
	GetPrivateProfileString("directory","meshDir","",path,MAX_PATH,strPath);
	strcpy(m_MeshDir,path);
	GetPrivateProfileString("directory","textureDir","",path,MAX_PATH,strPath);
	strcpy(m_TextureDir,path);

	//load settings
	//[section output]
	m_bStaticMesh=GetPrivateProfileInt("output","useStaticMesh",0,strPath);
	char zoomRatio[20];
	GetPrivateProfileString("output","zoomRatio","0.02",zoomRatio,20,strPath);
	m_fZoomRatio=(float)atof(zoomRatio);
	//[section compression]
	m_bUseVertexCompress=GetPrivateProfileInt("compress","useVertexCompress",0,strPath);
	m_bUseAnimationCompress=GetPrivateProfileInt("compress","useAnimationCompress",1,strPath);
	m_bUseMaterialCompress=GetPrivateProfileInt("compress","useMaterialCompress",1,strPath);
	//[section option]
	m_bExportUV2=GetPrivateProfileInt("option","exportUV2",0,strPath);
	m_bExportVertexColor=GetPrivateProfileInt("option","exportVertexColor",1,strPath);

	//load action list[section actionlist]
	m_ActionList.clear();
	for(int i=0;;++i)
	{
		ActionItemType actionItem;
		char keyname[CHARLENGTH];
		sprintf_s(keyname,"action%d",i);
		char actionName[CHARLENGTH];
		GetPrivateProfileString("actionlist",keyname,"",actionName,CHARLENGTH,strPath);
		strcpy(actionItem.actionName,actionName);
		if(strcmp(actionItem.actionName,"")==0) break;
		sprintf_s(keyname,"startframe%d",i);
		actionItem.startFrameID=GetPrivateProfileInt("actionlist",keyname,0,strPath);
		sprintf_s(keyname,"endframe%d",i);
		actionItem.endFrameID=GetPrivateProfileInt("actionlist",keyname,0,strPath);
		sprintf_s(keyname,"fileno%d",i);
		actionItem.fileNo=GetPrivateProfileInt("actionlist",keyname,0,strPath);
		m_ActionList.push_back(actionItem);
	}
}

void MessageProcess::GetSettingFilePath(char* path)
{
	GetModuleFileName(/*GetModuleHandle("MAX9Plugins.dlu")*/hInstance,path,MAX_PATH);
	int nLen=strlen(path);
	while(path[--nLen]!='\\');
	path[++nLen]='\0';
	strcat(path,"\\SGP_MAX9Plugins_Setting.ini");
}

void MessageProcess::SetToDefaultSettings(HWND hwnd)
{
	LoadDefaultSetting();

	CheckDlgButton(hwnd,IDC_USE_STATIC_MESH,m_bStaticMesh);
	CheckDlgButton(hwnd,IDC_USE_VERTEX_COMPRESS,m_bUseVertexCompress);
	CheckDlgButton(hwnd,IDC_USE_ANIMATION_COMPRESS,m_bUseAnimationCompress);
	CheckDlgButton(hwnd,IDC_USE_MATERIAL_COMPRESS,m_bUseMaterialCompress);
	CheckDlgButton(hwnd,IDC_EXPORT_UV2,m_bExportUV2);
	CheckDlgButton(hwnd,IDC_EXPORT_VERTEX_COLOR,m_bExportVertexColor);

	//clear actionlist
	ClearAction(hwnd);
}

void MessageProcess::LoadDefaultSetting()
{
	char strPath[MAX_PATH];
	GetDefaultFilePath(strPath);
	//load default settings
	//[section output]
	m_bStaticMesh=GetPrivateProfileInt("output","useStaticMesh",0,strPath);
	//[section compression]
	m_bUseVertexCompress=GetPrivateProfileInt("compress","useVertexCompress",0,strPath);
	m_bUseAnimationCompress=GetPrivateProfileInt("compress","useAnimationCompress",1,strPath);
	m_bUseMaterialCompress=GetPrivateProfileInt("compress","useMaterialCompress",1,strPath);
	//[section option]
	m_bExportUV2=GetPrivateProfileInt("option","exportUV2",0,strPath);
	m_bExportVertexColor=GetPrivateProfileInt("option","exportVertexColor",1,strPath);
}

void MessageProcess::GetDefaultFilePath(char *path)
{
	GetModuleFileName(/*GetModuleHandle("MAX9Plugins.dlu")*/hInstance,path,MAX_PATH);
	int nLen=strlen(path);
	while(path[--nLen]!='\\');
	path[++nLen]='\0';
	strcat(path,"\\SGP_MAX9Plugins_DefaultSetting.ini");
}

BOOL MessageProcess::CheckDlgSettings(HWND hwnd)
{
	//check dir
	int editID[]={IDE_WORK_DIR,IDE_BONE_DIR,IDE_MESH_DIR,IDE_TEXTURE_DIR};
	char* str[]={"工作目录","Bone存放目录","Mesh存放目录","Texture存放目录"};
	int i;
	for(i=0;i<4;++i)
	{
		char strPath[MAX_PATH];
		GetDlgItemText(hwnd,editID[i],strPath,MAX_PATH);
		if(!FolderDialog::IsDirectory(strPath))
		{
			char info[150];
			strcpy(info,str[i]);
			strcat(info,"不存在或无效！\n请正确设置后再执行导出操作！");
			MessageBox(hwnd,info,"Export Error",MB_OK|MB_ICONERROR);
			SetFocus(GetDlgItem(hwnd,editID[i]));
			SendMessage(GetDlgItem(hwnd,editID[i]),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
			return FALSE;
		}
	}
	char zoomratio[20];
	GetDlgItemText(hwnd,IDE_ZOOM_RATIO,zoomratio,20);
	BOOL bError=FALSE;
	int len=strlen(zoomratio);
	for(i=0;i<len;++i)
	{
		if((zoomratio[i]>='0'&&zoomratio[i]<='9')||zoomratio[i]=='.'){}
		else bError=TRUE;
	}
	float fzoomratio=(float)atof(zoomratio);
	if(fzoomratio<1e-10) bError=TRUE;
	if(bError)
	{
		MessageBox(hwnd,"输出缩放比设置不正确！\n请正确设置后再执行导出操作！",\
			"Export Error",MB_OK|MB_ICONERROR);
		SetFocus(GetDlgItem(hwnd,IDE_ZOOM_RATIO));
		SendMessage(GetDlgItem(hwnd,IDE_ZOOM_RATIO),EM_SETSEL,(WPARAM)0,(LPARAM)-1);
		return FALSE;
	}
	return TRUE;
}

void MessageProcess::ReleaseResource()
{
	DestroyWindow(m_hToolTip);
	m_hToolTip=NULL;
}
