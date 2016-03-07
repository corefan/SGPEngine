/**********************************************************************
 *<
	FILE: SGP_MAX9Plugins.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#include "SGP_MAX9Plugins.h"
#include "SGP_MAX9ControlPanel.h"
#include "SGP_MAX9Interface.h"

#ifdef _DEBUG
#include "vld.h"
#endif

#define SGP_MAX9Plugins_CLASS_ID	Class_ID(0xbd9bcc6d, 0x8a1f82a8)


class SGP_MAX9Plugins : public UtilityObj {
	public:
		
		HWND			hPanel;
		IUtil			*iu;
		Interface		*ip;
		
		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);
		
		void DeleteThis() { }		
		//Constructor/Destructor

		SGP_MAX9Plugins();
		~SGP_MAX9Plugins();	

	public:
		void LaunchMF1Exporter();
};

static SGP_MAX9Plugins theSGP_MAX9Plugins;


class SGP_MAX9PluginsClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return &theSGP_MAX9Plugins; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
	Class_ID		ClassID() { return SGP_MAX9Plugins_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("SGP_MAX9Plugins"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	

};

static SGP_MAX9PluginsClassDesc SGP_MAX9PluginsDesc;
ClassDesc2* GetSGP_MAX9PluginsDesc() { return &SGP_MAX9PluginsDesc; }




static INT_PTR CALLBACK SGP_MAX9PluginsDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			theSGP_MAX9Plugins.Init(hWnd);
			break;

		case WM_DESTROY:
			theSGP_MAX9Plugins.Destroy(hWnd);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDC_BUTTON_EXPORT:
					if( !g_SGPMax9CtrlPanel.m_bRunning )
						theSGP_MAX9Plugins.LaunchMF1Exporter();
					break;
				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			theSGP_MAX9Plugins.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return 0;
	}
	return 1;
}



//--- SGP_MAX9Plugins -------------------------------------------------------
SGP_MAX9Plugins::SGP_MAX9Plugins()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

SGP_MAX9Plugins::~SGP_MAX9Plugins()
{

}

void SGP_MAX9Plugins::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PANEL),
		SGP_MAX9PluginsDlgProc,
		GetString(IDS_PARAMS),
		0);

    ip->PushPrompt(_M("SGP ENGINE MAX9Plugins Lunched!"));

	g_SGPMax9CtrlPanel.GetMaxIP()->Create(0, theSGP_MAX9Plugins.ip);
}
	
void SGP_MAX9Plugins::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	this->ip = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;

	ip->PopPrompt();
}

void SGP_MAX9Plugins::Init(HWND hWnd)
{

}

void SGP_MAX9Plugins::Destroy(HWND hWnd)
{

}

void SGP_MAX9Plugins::LaunchMF1Exporter()
{
	g_SGPMax9CtrlPanel.m_bRunning = true;
	INode* pNode = g_SGPMax9CtrlPanel.GetMaxIP()->GetSelectedNode();
	if( pNode )
	{
		if( !g_SGPMax9CtrlPanel.GetMaxIP()->IsBone( pNode ) &&
			!g_SGPMax9CtrlPanel.GetMaxIP()->IsBipedBone( pNode ) )
		{
			MessageBox( hPanel, TEXT("你只能选择骨骼类节点进行导出"), TEXT("error"), MB_OK );
			g_SGPMax9CtrlPanel.m_bRunning = false;
			return;
		}
	}

	g_SGPMax9CtrlPanel.Create( hInstance, hPanel );

	g_SGPMax9CtrlPanel.m_bRunning = false;
}

