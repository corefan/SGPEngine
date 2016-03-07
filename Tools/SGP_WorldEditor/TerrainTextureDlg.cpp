// TerrainTexureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainTextureDlg.h"
#include "afxdialogex.h"
#include "TerrainEditorDlg.h"

#include "TerrainTextureHandler.h"
#include "MouseMsgInterface.h"
#include "ThumbnailGenerator.h"
#include "WorldEditorConfig.h"
#include "TerrainTextureLoadDlg.h"
#include "WorldMapManager.h"
#include "FolderDialog.h"
#include "HelpFunction.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <io.h>
// CTerrainTexureDlg 对话框

static int button_id2[]={\
	IDB_ADD_VERTEX_TEXTURE,\
	IDB_DELETE_VERTEX_TEXTURE,\
	IDB_SHOW_CHUNK_TEXTURE
};

extern ESGPTerrainChunkTexture ChunkLayer[7];

CTerrainTextureDlg* CTerrainTextureDlg::s_pTextureDlg=NULL;

IMPLEMENT_DYNAMIC(CTerrainTextureDlg, CDialogEx)

CTerrainTextureDlg::CTerrainTextureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainTextureDlg::IDD, pParent)
{
	m_nCurrLayer=-1;
	m_bInitialized=FALSE;
}

CTerrainTextureDlg::~CTerrainTextureDlg()
{
}

void CTerrainTextureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXTURE_LIST, m_TextureList);
}

BEGIN_MESSAGE_MAP(CTerrainTextureDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND, &CTerrainTextureDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDB_TEXTURE_REFRESH, &CTerrainTextureDlg::OnTextureResourceRefresh)
	ON_BN_CLICKED(IDB_BROWSE_FOLDER,OnBrowseFolder)
END_MESSAGE_MAP()

// CTerrainTexureDlg 消息处理程序

BOOL CTerrainTextureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ExpandButton.SubclassDlgItem(IDB_EXPAND,this);
	GetClientRect(m_rc);
	SetExpand(FALSE);

	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	pInnerCircle->SetRange(0,TEX_SLIDER_LENGTH);
	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	pOuterCircle->SetRange(0,TEX_SLIDER_LENGTH);
	CSliderCtrl* pAlphaCtrl=(CSliderCtrl*)GetDlgItem(IDC_TEXTURE_ALPHA);
	pAlphaCtrl->SetRange(1,255);

	pInnerCircle->SetPos(250);
	SetInnerSize();
	pOuterCircle->SetPos(500);
	SetOuterSize();
	pAlphaCtrl->SetPos(125);
	SetTextureAlpha();

	m_ImageList.Create(64,64,ILC_COLOR24,0,0);
	m_TextureList.SetImageList(&m_ImageList,LVSIL_NORMAL);

	m_LayerImageCtrl[0].SubclassDlgItem(IDC_LAYER0_IMAGE,this);
	m_LayerImageCtrl[0].SetLayer(VertexLayer0);
	m_LayerImageCtrl[1].SubclassDlgItem(IDC_LAYER1_IMAGE,this);
	m_LayerImageCtrl[1].SetLayer(VertexLayer1);
	m_LayerImageCtrl[2].SubclassDlgItem(IDC_LAYER2_IMAGE,this);
	m_LayerImageCtrl[2].SetLayer(VertexLayer2);
	m_LayerImageCtrl[3].SubclassDlgItem(IDC_LAYER3_IMAGE,this);
	m_LayerImageCtrl[3].SetLayer(VertexLayer3);

	m_ChunkImageCtrl[0].SubclassDlgItem(IDC_CHUNK_LAYER0_IMAGE,this);
	m_ChunkImageCtrl[0].SetLayer(ChunkLayer0);
	m_ChunkImageCtrl[1].SubclassDlgItem(IDC_CHUNK_LAYER1_IMAGE,this);
	m_ChunkImageCtrl[1].SetLayer(ChunkLayer1);
	m_ChunkImageCtrl[2].SubclassDlgItem(IDC_CHUNK_LAYER2_IMAGE,this);
	m_ChunkImageCtrl[2].SetLayer(ChunkLayer2);
	m_ChunkImageCtrl[3].SubclassDlgItem(IDC_CHUNK_LAYER3_IMAGE,this);
	m_ChunkImageCtrl[3].SetLayer(ChunkLayer3);
	m_ChunkImageCtrl[4].SubclassDlgItem(IDC_CHUNK_NORMALMAP_IMAGE,this);
	m_ChunkImageCtrl[4].SetLayer(ChunkNormalMap);
	m_ChunkImageCtrl[5].SubclassDlgItem(IDC_CHUNK_DETAILMAP_IMAGE,this);
	m_ChunkImageCtrl[5].SetLayer(ChunkDetailMap);
	m_ChunkImageCtrl[6].SubclassDlgItem(IDC_CHUNK_SLOPEMAP_IMAGE,this);
	m_ChunkImageCtrl[6].SetLayer(ChunkSlopeMap);

	for(int i=0;i<sizeof(button_id2)/sizeof(int);++i)
		m_Button[i].SubclassDlgItem(button_id2[i],this);

	CheckDlgButton(IDC_VIEWMODE_ICON,BST_CHECKED);

	s_pTextureDlg = this;

	if(WorldEditorConfig::GetInstance()->IsTerrainTexturePathExist())
		SetDlgItemText(IDE_TEXTURE_FOLDER,WorldEditorConfig::GetInstance()->GetTerrainTextureDir());

	return TRUE;
}

BOOL CTerrainTextureDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	int nID=::GetDlgCtrlID(pMsg->hwnd);

	if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_MOUSEMOVE)
	{
		if(nID==IDC_INNER_CIRCLE_SIZE) SetInnerSize();
		if(nID==IDC_OUTER_CIRCLE_SIZE) SetOuterSize();
		if(nID==IDC_TEXTURE_ALPHA) SetTextureAlpha();
	}

	if(nID==IDE_TEXTURE_FOLDER)
	{
		if(pMsg->message == WM_CHAR) return TRUE;
		if(pMsg->message>=WM_KEYFIRST&&pMsg->message<=WM_KEYLAST) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTerrainTextureDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
	{
		InitTextureResourceList();
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	}
	else
	{
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE);
	}
}

void CTerrainTextureDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.SetWindowPos(NULL,0,0,cx,20,SWP_SHOWWINDOW);
}


void CTerrainTextureDlg::OnBnClickedExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CTerrainEditorDlg*)GetParent())->SetCtrlPos();
}

void CTerrainTextureDlg::SetInnerSize()
{
	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	int pos=pInnerCircle->GetPos();
	float innerRadius=(float)MIN_TEXCIRCLE_RADIUS+(float)(MAX_TEXCIRCLE_RADIUS-MIN_TEXCIRCLE_RADIUS)*pos/TEX_SLIDER_LENGTH;
	TerrainTextureHandler::GetInstance()->SetInnerRadius(innerRadius);

	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	if(pOuterCircle->GetPos()<pos)
	{
		pOuterCircle->SetPos(pos);
		TerrainTextureHandler::GetInstance()->SetOuterRadius(innerRadius);
	}
}

void CTerrainTextureDlg::SetOuterSize()
{
	CSliderCtrl* pOuterCircle=(CSliderCtrl*)GetDlgItem(IDC_OUTER_CIRCLE_SIZE);
	int pos=pOuterCircle->GetPos();
	float outerRadius=(float)MIN_TEXCIRCLE_RADIUS+(float)(MAX_TEXCIRCLE_RADIUS-MIN_TEXCIRCLE_RADIUS)*pos/TEX_SLIDER_LENGTH;
	TerrainTextureHandler::GetInstance()->SetOuterRadius(outerRadius);

	CSliderCtrl* pInnerCircle=(CSliderCtrl*)GetDlgItem(IDC_INNER_CIRCLE_SIZE);
	if(pInnerCircle->GetPos()>pos)
	{
		pInnerCircle->SetPos(pos);
		TerrainTextureHandler::GetInstance()->SetOuterRadius(outerRadius);
	}
}

void CTerrainTextureDlg::SetTextureAlpha()
{
	CSliderCtrl* pAlphaCtrl=(CSliderCtrl*)GetDlgItem(IDC_TEXTURE_ALPHA);
	int pos=pAlphaCtrl->GetPos();
	TerrainTextureHandler::GetInstance()->SetTextureAlpha(pos);
}

void CTerrainTextureDlg::SetLayerImage(CString strName)
{
	CString fileName=strName.Left(strName.ReverseFind('.'))+".bmp";
	CString filePath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+fileName;
	CString relativeFilePath=WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()+"\\"+strName;
	if(m_nCurrLayer!=-1)
	{
		m_LayerImageCtrl[m_nCurrLayer].SetImage(filePath,strName);
		TerrainTextureHandler::GetInstance()->SetLayerImgPath(m_nCurrLayer,relativeFilePath);
	}
}

void CTerrainTextureDlg::SetLayerImage(int nLayer,CString sTextureName,CString sImageName)
{
	CString imgPath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+sImageName;
	CString texPath=WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()+"\\"+sTextureName;
	m_LayerImageCtrl[nLayer].SetImage(imgPath,texPath);
	TerrainTextureHandler::GetInstance()->SetLayerImgPath(nLayer,texPath);
}

void CTerrainTextureDlg::SetChunkImage(int index,CString sTextureName,CString sImageName)
{
	CString imgPath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+sImageName;
	CString texPath=WorldEditorConfig::GetInstance()->GetTerrainTextureRelativeDir()+"\\"+sTextureName;
	m_ChunkImageCtrl[index].SetImage(imgPath,texPath);
	TerrainTextureHandler::GetInstance()->SetChunkTexture(index,texPath);
}

void CTerrainTextureDlg::SetCurrentLayer(int nLayer)
{
	for(int i=0;i<4;++i)
	{
		m_LayerImageCtrl[i].SetLayerSel(i==nLayer);
	}
	TerrainTextureHandler::GetInstance()->SetCurrentLayer(nLayer);
	m_nCurrLayer=nLayer;
}

BOOL CTerrainTextureDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID=LOWORD(wParam);
	switch(nID)
	{
	case IDB_ADD_VERTEX_TEXTURE:
		MouseMsgInterface::GetInstance()->SwitchToTextureEditor();
		TerrainTextureHandler::GetInstance()->SetBrushType(TextureType_Add);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(nID);
		break;
	case IDB_DELETE_VERTEX_TEXTURE:
		MouseMsgInterface::GetInstance()->SwitchToTextureEditor();
		TerrainTextureHandler::GetInstance()->SetBrushType(TextureType_Delete);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(nID);
		break;
	case IDB_SHOW_CHUNK_TEXTURE:
		MouseMsgInterface::GetInstance()->SwitchToTextureEditor();
		TerrainTextureHandler::GetInstance()->SetBrushType(TextureType_ChunkAdjust);
		((CTerrainEditorDlg*)GetParent())->SetButtonState(nID);
	//	SetButtonState(nID);
		break;
	case IDC_VIEWMODE_LIST:
		m_TextureList.ModifyStyle(LVS_ICON,LVS_LIST);
		break;
	case IDC_VIEWMODE_ICON:
		m_TextureList.ModifyStyle(LVS_LIST,LVS_ICON);
		break;
	case IDC_CLOSE_LAYER1:
		WorldMapManager::GetInstance()->CloseLayer(IsDlgButtonChecked(IDC_CLOSE_LAYER1),\
			IsDlgButtonChecked(IDC_CLOSE_LAYER2),IsDlgButtonChecked(IDC_CLOSE_LAYER3));
		break;
	case IDC_CLOSE_LAYER2:
		WorldMapManager::GetInstance()->CloseLayer(IsDlgButtonChecked(IDC_CLOSE_LAYER1),\
			IsDlgButtonChecked(IDC_CLOSE_LAYER2),IsDlgButtonChecked(IDC_CLOSE_LAYER3));
		break;
	case IDC_CLOSE_LAYER3:
		WorldMapManager::GetInstance()->CloseLayer(IsDlgButtonChecked(IDC_CLOSE_LAYER1),\
			IsDlgButtonChecked(IDC_CLOSE_LAYER2),IsDlgButtonChecked(IDC_CLOSE_LAYER3));
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

void CTerrainTextureDlg::SetButtonState(int nSelID)
{
	BOOL bFind = FALSE;
	COLORREF normalColor=RGB(0,0,0),focusColor=RGB(255,0,255);
	for(int i=0;i<sizeof(button_id2)/sizeof(int);++i)
	{
		if(nSelID==button_id2[i])
		{
			m_Button[i].SetTextColor(focusColor);
			bFind = TRUE;
		}
		else m_Button[i].SetTextColor(normalColor);
		m_Button[i].Invalidate();
	}
	
	if(nSelID != IDB_SHOW_CHUNK_TEXTURE)
	{
		for(int i=0;i<7;++i) m_ChunkImageCtrl[i].SetImage("","");
	}

	if(!bFind) TerrainTextureHandler::GetInstance()->SetBrushType(TextureType_None);
}

void CTerrainTextureDlg::OnTextureResourceRefresh()
{
	if(!WorldEditorConfig::GetInstance()->IsTerrainTexturePathExist()) return ;

	m_TextureList.DeleteAllItems();
	m_ImageList.DeleteImageList();
	m_ImageList.Create(64,64,ILC_COLOR24,0,0);
	m_TextureList.SetImageList(&m_ImageList,LVSIL_NORMAL);
	m_TextureList.SetRedraw(FALSE);
	CTerrainTextureLoadDlg loadingDlg(this);
	loadingDlg.DoModal();
	m_TextureList.SetRedraw(TRUE);
}

void CTerrainTextureDlg::InitTextureResourceList()
{
	if(!m_bInitialized)
	{
		m_bInitialized = TRUE;
		if(WorldEditorConfig::GetInstance()->IsTerrainTexturePathExist())
		{
			CTerrainTextureLoadDlg loadingDlg(this);
			loadingDlg.DoModal();
		}
		
	}
}

void CTerrainTextureDlg::UpdateSelectedChunkInfo(const std::vector<int>& chunkVector)
{
	int chunkCount=chunkVector.size();
	if(chunkCount==0)
	{
		for(int i=0;i<7;++i) m_ChunkImageCtrl[i].SetImage("","");
	}
	else if(chunkCount==1)
	{
		int chunkId=chunkVector[0];
		for(int i=0;i<7;++i)
		{
			CString texRelativePath=WorldMapManager::GetInstance()->GetChunkTexture(chunkId,i);
			if(texRelativePath=="")
				m_ChunkImageCtrl[i].SetImage("","");
			else
			{
				CString texName=texRelativePath.Right(texRelativePath.GetLength()-texRelativePath.ReverseFind('\\')-1);
				CString texPath = WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\"+texRelativePath;
				CFileStatus status;
				CFile::GetStatus(texPath,status);
				CString imgName;
				imgName.Format("%s-%I64u.bmp",texName,status.m_size);
				CString imgPath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+imgName;
				m_ChunkImageCtrl[i].SetImage(imgPath,texPath);
			}
		}
	}
	else
	{
		for(int i=0;i<7;++i)
		{
			BOOL bSameTex = TRUE;
			CString texRelativePath = WorldMapManager::GetInstance()->GetChunkTexture(chunkVector[0],i);
			for(int j=1;j<chunkCount;++j)
			{
				if(WorldMapManager::GetInstance()->GetChunkTexture(chunkVector[j],i)!=texRelativePath)
				{
					bSameTex = FALSE;
					break;
				}
			}
			if(!bSameTex) texRelativePath = "";
			
			if(texRelativePath=="")
				m_ChunkImageCtrl[i].SetImage("","");
			else
			{
				CString texName=texRelativePath.Right(texRelativePath.GetLength()-texRelativePath.ReverseFind('\\')-1);
				CString texPath = WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\"+texRelativePath;
				CFileStatus status;
				CFile::GetStatus(texPath,status);
				CString imgName;
				imgName.Format("%s-%I64u.bmp",texName,status.m_size);
				CString imgPath=WorldEditorConfig::GetInstance()->GetTerrainTextureThumbnailDir()+"\\"+imgName;
				m_ChunkImageCtrl[i].SetImage(imgPath,texPath);
			}
		}
	}
}

void CTerrainTextureDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

BOOL CTerrainTextureDlg::IsLayerClosed(int nLayer)
{
	if(nLayer==0) return FALSE;
	ASSERT(nLayer>=1&&nLayer<=3);
	static int ButtonID[3] = {IDC_CLOSE_LAYER1,IDC_CLOSE_LAYER2,IDC_CLOSE_LAYER3};
	return IsDlgButtonChecked(ButtonID[nLayer-1]);
}

void CTerrainTextureDlg::OnBrowseFolder()
{
	CString initialDir;
	GetDlgItemText(IDE_TEXTURE_FOLDER,initialDir);
	if((!initialDir.IsEmpty())&&PathIsDirectory(initialDir));
	else initialDir = WorldEditorConfig::GetInstance()->GetDataFileDir();

	CFolderDialog dlg(m_hWnd,"Choose Terrain Texture Folder",initialDir);
	if(dlg.DoModal()==IDOK)
	{
		CString texPath = dlg.GetPath();
		if(FileInFolder(texPath,WorldEditorConfig::GetInstance()->GetDataFileDir()))
		{
			SetDlgItemText(IDE_TEXTURE_FOLDER,texPath);
			WorldEditorConfig::GetInstance()->SaveTerrainTextureRelativeDir(GetRelativePath(texPath,\
				WorldEditorConfig::GetInstance()->GetDataFileDir()));
			OnTextureResourceRefresh();
		}
		else
			MessageBox("The folder must be in Data directory!","WorldEditor",MB_OK|MB_ICONERROR);
	}
}