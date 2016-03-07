#include "SGP_MAX9ControlPanel.h"
#include "SGP_MAX9Interface.h"
#include "SGP_MF1Candidate.h"

#include "MessageProcess.h"
CSGPMAX9CtrlPanel g_SGPMax9CtrlPanel;

INT_PTR CALLBACK ExportSettingWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

CSGPMAX9CtrlPanel::CSGPMAX9CtrlPanel() : m_hCtrlPanel(NULL), m_nSelId(-1), m_bRunning(false)
{ 
	m_pMaxInterface = new SGP_MaxInterface(); 
}

CSGPMAX9CtrlPanel::~CSGPMAX9CtrlPanel()
{
	ShutDown();
}

void CSGPMAX9CtrlPanel::Create( HINSTANCE hInstance, HWND hPanel )
{
	InitCommonControls();

	INode* pSelNode = NULL;
	if( GetMaxIP()->GetSelectedNode() )
		pSelNode = GetMaxIP()->GetSelectedNode();
	else
		pSelNode = GetMaxIP()->GetRootNode();


	int ReturnValue = DialogBox( 
							hInstance, 
							MAKEINTRESOURCE( IDD_DIALOG_EXPORT ), 
							hPanel, 
							ExportSettingWndProc );
	if( ReturnValue == -1 )
		return;

	SGP_MF1Candidate mc;
	if( !mc.CreateMF1() )
	{
		MessageBox( m_hCtrlPanel, TEXT("创建模型动画失败！"), TEXT("error"), MB_OK );
		return;
	}

	if( !mc.CheckMF1FileFormat() )
	{
		MessageBox( m_hCtrlPanel, TEXT("制作不符合规范！"), TEXT("error"), MB_OK );
		return;
	}
	
	if( !mc.SaveMF1File( GetMaxIP()->GetCurFilePath().data() ) )
	{
		MessageBox( m_hCtrlPanel, TEXT("保存模型动画失败，可能是当前文件未命名或磁盘文件只读！"), TEXT("error"), MB_OK );
		return;
	}

	return;
}


void CSGPMAX9CtrlPanel::ShutDown()
{
	if( m_pMaxInterface )
	{
		delete m_pMaxInterface;
		m_pMaxInterface = NULL;
	}

	if( m_hCtrlPanel )
	{
		DestroyWindow( m_hCtrlPanel );
		m_hCtrlPanel = NULL;
	}
	MessageProcess::ReleaseInstance();
}


INT_PTR CALLBACK ExportSettingWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		MessageProcess::GetInstance()->InitDlg(hWnd);
		MessageProcess::GetInstance()->InitCtrl(hWnd);	
		MessageProcess::GetInstance()->LoadSettings(hWnd);
		break;
	case WM_DRAWITEM:
		if(IDOK==wParam) MessageProcess::GetInstance()->DrawExportButton(lParam);
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			if(MessageProcess::GetInstance()->CheckDlgSettings(hWnd))
			{
				MessageProcess::GetInstance()->SaveSettings(hWnd);
				MessageProcess::GetInstance()->ReleaseResource();
				EndDialog( hWnd, 0 );
			}
			break;
		case IDCANCEL:
			MessageProcess::GetInstance()->ReleaseResource();
			EndDialog( hWnd, -1 );
			break;
		case IDC_ACTION_ADD://添加动作
			MessageProcess::GetInstance()->AddAction(hWnd);
			break;
		case IDC_ACTION_DELETE://删除动作
			MessageProcess::GetInstance()->DeleteAction(hWnd);
			break;
		case IDC_ACTION_CLEAR://清空动作
			MessageProcess::GetInstance()->ClearAction(hWnd);
			break;
		case IDC_ACTION_MODIFY://加载动作
			MessageProcess::GetInstance()->ModifyAction(hWnd);
			break;

		case IDB_SET_WORKDIR://设置工作目录
			MessageProcess::GetInstance()->SetWorkDir(hWnd);
			break;
		case IDB_SET_MESHDIR://设置Mesh存放目录
			MessageProcess::GetInstance()->SetMeshDir(hWnd);
			break;
		case IDB_SET_BONEDIR://设置Bone存放目录
			MessageProcess::GetInstance()->SetBoneDir(hWnd);
			break;
		case IDB_SET_TEXTUREDIR://设置texture存放目录
			MessageProcess::GetInstance()->SetTextureDir(hWnd);
			break;

		case IDB_DEFAULT_SETTING://恢复默认设置
			MessageProcess::GetInstance()->SetToDefaultSettings(hWnd);
			break;
		default:
			break;
		};
		break;
	case WM_NOTIFY:
		//动作列表双击通知消息
		if((int)wParam==IDC_ACTION_LIST&&((LPNMHDR)lParam)->code==NM_DBLCLK)
			MessageProcess::GetInstance()->ModifyAction(hWnd,FALSE);
		break;
	case WM_MOUSEMOVE:
		//show tooltips
		MessageProcess::GetInstance()->ShowToolTips(hWnd,uMsg,wParam,lParam);
		break;
	};
	return FALSE;
}