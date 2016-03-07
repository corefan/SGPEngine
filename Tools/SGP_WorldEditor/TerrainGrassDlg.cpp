// TerrainGrassDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainGrassDlg.h"
#include "afxdialogex.h"
#include "TerrainEditorDlg.h"
#include "ThumbnailGenerator.h"
#include "WorldEditorConfig.h"
#include "MouseMsgInterface.h"
#include "TerrainGrassHandler.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "HelpFunction.h"
#include "TextureInfoDlg.h"
#include "OperationManager.h"
#include <io.h>

static int button_id[]={\
	IDB_ADD_GRASS,\
	IDB_DELETE_GRASS
};

CTerrainGrassDlg* CTerrainGrassDlg::s_pGrassDlg=NULL;

// CTerrainGrassDlg 对话框

IMPLEMENT_DYNAMIC(CTerrainGrassDlg, CDialogEx)

CTerrainGrassDlg::CTerrainGrassDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainGrassDlg::IDD, pParent)
{
	m_nGrassIndex = -1;
}

CTerrainGrassDlg::~CTerrainGrassDlg()
{
}

void CTerrainGrassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_GRASS_LIST,m_GrassTexList);
}

BEGIN_MESSAGE_MAP(CTerrainGrassDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND, &CTerrainGrassDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDB_IMPORT_TEXTURE,OnImportTexture)
	ON_BN_CLICKED(IDB_SETTING_APPLY,OnSettingApply)
	ON_BN_CLICKED(IDB_GENERATE_TEXTURE_LIST,OnGenerateTextureList)
	ON_BN_CLICKED(IDB_TEXTURE_PREVIEW,OnPreviewTexture)
END_MESSAGE_MAP()

// CTerrainGrassDlg 消息处理程序

BOOL CTerrainGrassDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ExpandButton.SubclassDlgItem(IDB_EXPAND,this);
	GetClientRect(m_rc);
	SetExpand(FALSE);

	for(int i=0;i<sizeof(button_id)/sizeof(int);++i)
		m_Button[i].SubclassDlgItem(button_id[i],this);

	CSliderCtrl* pGrassMinSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MINSIZE);
	pGrassMinSize->SetRange(1,255);
	CSliderCtrl* pGrassMaxSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MAXSIZE);
	pGrassMaxSize->SetRange(1,255);
	CSliderCtrl* pGrassDensity = (CSliderCtrl*)GetDlgItem(IDC_GRASS_DENSITY);
	pGrassDensity->SetRange(1,100);
	CSliderCtrl* pWindOffsetX = (CSliderCtrl*)GetDlgItem(IDC_WIND_OFFSET_X);
	pWindOffsetX->SetRange(0,255);
	CSliderCtrl* pWindOffsetZ = (CSliderCtrl*)GetDlgItem(IDC_WIND_OFFSET_Z);
	pWindOffsetZ->SetRange(0,255);
	CSliderCtrl* pBrushSize = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_SIZE);
	pBrushSize->SetRange(0,GRASS_SLIDER_LENGTH);

	// Init Grass Attributes
	pGrassMinSize->SetPos(127);
	SetGrassMinSize();
	pGrassMaxSize->SetPos(127);
	SetGrassMaxSize();
	pGrassDensity->SetPos(50);
	SetGrassDensity();
	pWindOffsetX->SetPos(127);
	SetWindOffsetX();
	pWindOffsetZ->SetPos(127);
	SetWindOffsetZ();
	pBrushSize->SetPos(400);
	SetBrushSize();

	// Set Initial Value
//	SetDlgItemInt(IDE_GRASS_TILE_X,1);
//	SetDlgItemInt(IDE_GRASS_TILE_Y,1);
	InitDefaultValue();

	s_pGrassDlg = this;

	return TRUE;
}

BOOL CTerrainGrassDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	int nID = ::GetDlgCtrlID(pMsg->hwnd);
	if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_LBUTTONUP)
	{
		switch (nID)
		{
		case IDC_GRASS_MINSIZE:
			SetGrassMinSize();
			break;
		case IDC_GRASS_MAXSIZE:
			SetGrassMaxSize();
			break;
		case IDC_GRASS_DENSITY:
			SetGrassDensity();
			break;
		case IDC_WIND_OFFSET_X:
			SetWindOffsetX();
			break;
		case IDC_WIND_OFFSET_Z:
			SetWindOffsetZ();
			break;
		case IDC_BRUSH_SIZE:
			SetBrushSize();
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTerrainGrassDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.MoveWindow(0,0,cx,20);
}

void CTerrainGrassDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	else
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE);
}

void CTerrainGrassDlg::OnBnClickedExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CTerrainEditorDlg*)GetParent())->SetCtrlPos();
}

void CTerrainGrassDlg::OnImportTexture()
{
	TCHAR strFilter[] = _T("DDS Files (*.dds)|*.dds|TGA Files (.tga)|*.tga||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter,this);
	dlg.m_ofn.lpstrInitialDir=(char*)(LPCTSTR)WorldEditorConfig::GetInstance()->GetDataFileDir();
	if(dlg.DoModal() == IDOK)
	{
		CString texPath=dlg.GetPathName();
		CString workDir = WorldEditorConfig::GetInstance()->GetDataFileDir();

		if(!FileInFolder(texPath,workDir))
		{
			MessageBox("The texture must be in Data Directory!","Terrain Grass",MB_OK|MB_ICONERROR);
			return ;
		}
		m_TexturePath = GetRelativePath(texPath,WorldEditorConfig::GetInstance()->GetDataFileDir());
	}
}

void CTerrainGrassDlg::OnGenerateTextureList()
{
	if(m_TexturePath.IsEmpty())
	{
		MessageBox("Please import grass texture firstly!","Terrain Grass",MB_OK|MB_ICONSTOP);
		return ;
	}

	CString strText;
	GetDlgItemText(IDE_GRASS_TILE_X,strText);
	if(strText==""||atoi(strText)==0)
	{
		MessageBox("Invalidate Grass Tile X！","Terrain Grass",MB_OK);
		return ;
	}
	int xTile = atoi(strText);
	GetDlgItemText(IDE_GRASS_TILE_Y,strText);
	if(strText==""||atoi(strText)==0)
	{
		MessageBox("Invalidate Grass Tile Y！","Terrain Grass",MB_OK);
		return ;
	}
	int yTile = atoi(strText);

	// check whether exist
	TerrainGrassHandler* pGrassHandler = TerrainGrassHandler::GetInstance();
	CSGPGrass* pGrass = (CSGPGrass*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getGrass());
	if(pGrass != NULL)
	{
		if(m_TexturePath==pGrassHandler->m_GrassTexPath&&xTile==pGrassHandler->m_TexTileX&&yTile==pGrassHandler->m_TexTileY) return ;
		CString strInfo;
		if(m_TexturePath != pGrassHandler->m_GrassTexPath)
			strInfo = _T("Grass texture have been changed.\nIf you use new grass texture, all previous grass in the map will be removed !!!\nSo ,do you want to use new grass texture ?");
		else
			strInfo = _T("Grass tile have been changed.\nIf you use new grass tile, all previous grass in the map will be removed !!!\nSo ,do you want to use new grass tile ?");
		if(MessageBox(strInfo,"Terrain Grass",MB_YESNO|MB_ICONWARNING)==IDNO) return ;

		((COpenGLRenderDevice*)(WorldEditorRenderInterface::GetInstance()->GetRenderDevice()))->getOpenGLGrassRenderer()->releaseGrassTexture();
		delete pGrass;
		OperationManager::GetInstance()->ClearGrassOperation();
		
		// delete previous grass info
		CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
		SGPWorldMapGrassTag& grassTag = pWorldMap->m_GrassData;
		int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
		chunkSize = chunkSize*chunkSize;
		for(int k=0;k<chunkSize;++k)
		{
			if(grassTag.m_ppChunkGrassCluster[k] != NULL)
			{
			//	delete grassTag.m_ppChunkGrassCluster[k];
				grassTag.m_ppChunkGrassCluster[k] = NULL;
			}
		}
	}

	// create grass
	BSTR texPath = m_TexturePath.AllocSysString();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->createGrass(String(texPath),xTile,yTile);
	pGrassHandler->SetGrassInfo(m_TexturePath,xTile,yTile);
	SysFreeString(texPath);

	CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	if(pWorldMap->m_GrassData.m_ppChunkGrassCluster==NULL)
	{
		int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
		int chunkCount=chunkSize*chunkSize;
		pWorldMap->m_GrassData.m_ppChunkGrassCluster=new SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag*[chunkCount];
		for(int i=0;i<chunkCount;++i)
			pWorldMap->m_GrassData.m_ppChunkGrassCluster[i]=NULL;
	}
	

	m_nGrassIndex = -1;
	pGrassHandler->SetTiledTexIndex(m_nGrassIndex);

/*	// clear old thumbnail
	CFileFind finder;
	BOOL bFind = finder.FindFile(WorldEditorConfig::GetInstance()->GetGrassTextureThumbnailDir()+"\\*.bmp");
	while(bFind)
	{
		bFind = finder.FindNextFile();
		if(finder.IsDots()) continue;
		if(finder.IsDirectory()) continue;
		DeleteFile(finder.GetFilePath());
	}

	ThumbnailGenerator generator(this);
	if(!generator.GenerateGrassThumbnail(m_TexturePath,xTile,yTile)) return ;

	m_GrassTexList.DeleteAllItems();
	m_ImageList.DeleteImageList();
	m_ImageList.Create(64,64,ILC_COLOR24,0,0);
	m_GrassTexList.SetImageList(&m_ImageList,LVSIL_NORMAL);

	CString grassThumbDir = WorldEditorConfig::GetInstance()->GetGrassTextureThumbnailDir();
	for(int i = 1; i <= xTile*yTile; ++i)
	{
		CString sFilePath,itemText;
		sFilePath.Format("%s\\%d.bmp",grassThumbDir,i);
		itemText.Format("%d",i);
		CImage img;
		img.Load(sFilePath);
		if(!img.IsNull())
		{
			HBITMAP hBitmap = img.Detach();
			CBitmap* pBitmap = CBitmap::FromHandle(hBitmap);
			int currTexIndex = m_GrassTexList.GetItemCount();
			int currImgIndex = m_ImageList.GetImageCount();
			m_ImageList.Add(pBitmap,RGB(0,0,0));
			m_GrassTexList.InsertItem(currTexIndex,itemText,currImgIndex);
			::DeleteObject(hBitmap);
		}
	}*/
	UpdateGrassList(m_TexturePath,xTile,yTile);
}

void CTerrainGrassDlg::UpdateGrassList(CString strPath,int xTile,int yTile)
{
	// clear old thumbnail
	CFileFind finder;
	BOOL bFind = finder.FindFile(WorldEditorConfig::GetInstance()->GetGrassTextureThumbnailDir()+"\\*.bmp");
	while(bFind)
	{
		bFind = finder.FindNextFile();
		if(finder.IsDots()) continue;
		if(finder.IsDirectory()) continue;
		DeleteFile(finder.GetFilePath());
	}

	ThumbnailGenerator generator(this);
	if(!generator.GenerateGrassThumbnail(strPath,xTile,yTile)) return ;

	m_GrassTexList.DeleteAllItems();
	m_ImageList.DeleteImageList();
	m_ImageList.Create(64,64,ILC_COLOR24,0,0);
	m_GrassTexList.SetImageList(&m_ImageList,LVSIL_NORMAL);

	CString grassThumbDir = WorldEditorConfig::GetInstance()->GetGrassTextureThumbnailDir();
	for(int i = 1; i <= xTile*yTile; ++i)
	{
		CString sFilePath,itemText;
		sFilePath.Format("%s\\%d.bmp",grassThumbDir,i);
		itemText.Format("%d",i);
		CImage img;
		img.Load(sFilePath);
		if(!img.IsNull())
		{
			HBITMAP hBitmap = img.Detach();
			CBitmap* pBitmap = CBitmap::FromHandle(hBitmap);
			int currTexIndex = m_GrassTexList.GetItemCount();
			int currImgIndex = m_ImageList.GetImageCount();
			m_ImageList.Add(pBitmap,RGB(0,0,0));
			m_GrassTexList.InsertItem(currTexIndex,itemText,currImgIndex);
			::DeleteObject(hBitmap);
		}
	}
}

void CTerrainGrassDlg::SetButtonState(int nSelID)
{
	BOOL bFind = FALSE;
	COLORREF normalColor=RGB(0,0,0),focusColor=RGB(255,0,255);
	for(int i=0;i<sizeof(button_id)/sizeof(int);++i)
	{
		if(nSelID==button_id[i])
		{
			m_Button[i].SetTextColor(focusColor);
			bFind = TRUE;
		}
		else m_Button[i].SetTextColor(normalColor);
		m_Button[i].Invalidate();
	}
	if(!bFind) TerrainGrassHandler::GetInstance()->SetBrushType(GrassType_None);
}

BOOL CTerrainGrassDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam);
	switch(nID)
	{
	case IDB_ADD_GRASS:
		if(m_nGrassIndex==-1)
			MessageBox("Please select a grass from grass list firstly！","Terrain Grass",MB_OK|MB_ICONSTOP);
		else
		{
			MouseMsgInterface::GetInstance()->SwitchToGrassEditor();
			TerrainGrassHandler::GetInstance()->SetBrushType(GrassType_Add);
			((CTerrainEditorDlg*)GetParent())->SetButtonState(IDB_ADD_GRASS);
		//	SetButtonState(IDB_ADD_GRASS);
		}
		break;
	case IDB_DELETE_GRASS:
		if(m_nGrassIndex==-1)
			MessageBox("Please select a grass from grass list firstly！","Terrain Grass",MB_OK|MB_ICONSTOP);
		else
		{
			MouseMsgInterface::GetInstance()->SwitchToGrassEditor();
			TerrainGrassHandler::GetInstance()->SetBrushType(GrassType_Delete);
			((CTerrainEditorDlg*)GetParent())->SetButtonState(IDB_DELETE_GRASS);
		//	SetButtonState(IDB_DELETE_GRASS);
		}
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

void CTerrainGrassDlg::SetGrassMinSize()
{
	CSliderCtrl* pGrassMinSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MINSIZE);
	int nPos = pGrassMinSize->GetPos();
	TerrainGrassHandler::GetInstance()->SetGrassMinSize(nPos);
	
	CSliderCtrl* pGrassMaxSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MAXSIZE);
	if(pGrassMaxSize->GetPos()<nPos)
	{
		pGrassMaxSize->SetPos(nPos);
		TerrainGrassHandler::GetInstance()->SetGrassMaxSize(nPos);
	}
}

void CTerrainGrassDlg::SetGrassMaxSize()
{
	CSliderCtrl* pGrassMaxSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MAXSIZE);
	int nPos = pGrassMaxSize->GetPos();
	TerrainGrassHandler::GetInstance()->SetGrassMaxSize(nPos);

	CSliderCtrl* pGrassMinSize = (CSliderCtrl*)GetDlgItem(IDC_GRASS_MINSIZE);
	if(pGrassMinSize->GetPos()>nPos)
	{
		pGrassMaxSize->SetPos(nPos);
		TerrainGrassHandler::GetInstance()->SetGrassMaxSize(nPos);
	}
}

void CTerrainGrassDlg::SetGrassDensity()
{
	CSliderCtrl* pGrassDensity = (CSliderCtrl*)GetDlgItem(IDC_GRASS_DENSITY);
	int nPos = pGrassDensity->GetPos();
	TerrainGrassHandler::GetInstance()->SetGrassDensity(nPos);
}

void CTerrainGrassDlg::SetWindOffsetX()
{
	CSliderCtrl* pWindOffsetX = (CSliderCtrl*)GetDlgItem(IDC_WIND_OFFSET_X);
	int nPos = pWindOffsetX->GetPos();
	TerrainGrassHandler::GetInstance()->SetWindOffsetX(nPos);
}

void CTerrainGrassDlg::SetWindOffsetZ()
{
	CSliderCtrl* pWindOffsetZ = (CSliderCtrl*)GetDlgItem(IDC_WIND_OFFSET_Z);
	int nPos = pWindOffsetZ->GetPos();
	TerrainGrassHandler::GetInstance()->SetWindOffsetZ(nPos);
}

void CTerrainGrassDlg::SetBrushSize()
{
	CSliderCtrl* pBrushSize = (CSliderCtrl*)GetDlgItem(IDC_BRUSH_SIZE);
	float ratio = ((float)pBrushSize->GetPos())/GRASS_SLIDER_LENGTH;
	float radius = MIN_GRASSCIRCLE_RADIUS + (MAX_GRASSCIRCLE_RADIUS-MIN_GRASSCIRCLE_RADIUS) * ratio;
	TerrainGrassHandler::GetInstance()->SetBrushSize(radius);
}

void CTerrainGrassDlg::OnSettingApply()
{
	// Grass Fade Distance Start
	CString str;
	GetDlgItemText(IDE_GRASS_FADE_START,str);
	float fFadeStart=(float)atof(str);
	if(fFadeStart>0.0f)
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart = fFadeStart;
	else
		MessageBox("Grass Fade Distance Start Value is error!","Terrain Grass",MB_OK|MB_ICONERROR);
	// Grass Fade Distance End
	GetDlgItemText(IDE_GRASS_FADE_END,str);
	float fFadeEnd = (float)atof(str);
	if(fFadeEnd>0.0f)
		CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd = fFadeEnd;
	else
		MessageBox("Grass Fade Distance End Value is error!","Terrain Grass",MB_OK|MB_ICONERROR);

	CSGPGrass* pGrass = (CSGPGrass*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getGrass());
	if(pGrass != NULL)
	{
		// Wind Direction
		GetDlgItemText(IDE_WIND_DIRECTION_X,str);
		if(!str.IsEmpty()) pGrass->m_vWindDirectionAndStrength.x = (float)atof(str);
		GetDlgItemText(IDE_WIND_DIRECTION_Y,str);
		if(!str.IsEmpty()) pGrass->m_vWindDirectionAndStrength.y = (float)atof(str);
		GetDlgItemText(IDE_WIND_DIRECTION_Z,str);
		if(!str.IsEmpty()) pGrass->m_vWindDirectionAndStrength.z = (float)atof(str);

		GetDlgItemText(IDE_WIND_STRENGTH,str);
		if(!str.IsEmpty()) pGrass->m_vWindDirectionAndStrength.w = (float)atof(str);

		GetDlgItemText(IDE_GRASS_PERIOD,str);
		if(!str.IsEmpty()) pGrass->m_fGrassPeriod = (float)atof(str);
	}
	else
	{
		MessageBox("Please set grass first!","Terrain Grass",MB_OK|MB_ICONSTOP);
	}
}

void CTerrainGrassDlg::SetTextureIndex(int index)
{
	m_nGrassIndex = index;
	TerrainGrassHandler::GetInstance()->SetTiledTexIndex(index);
}

void CTerrainGrassDlg::OnPreviewTexture()
{
	if(m_TexturePath.IsEmpty()) return ;
	CString textureName = m_TexturePath.Right(m_TexturePath.GetLength()-m_TexturePath.ReverseFind('\\')-1);
	CTextureInfoDlg dlg(m_TexturePath,textureName);
	dlg.DoModal();
}

void CTerrainGrassDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CTerrainGrassDlg::InitDefaultValue()
{
	CSGPGrass tmpGrass;
	CString strText;
	strText.Format("%.2f",CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart);
	SetDlgItemText(IDE_GRASS_FADE_START,strText);
	strText.Format("%.2f",CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd);
	SetDlgItemText(IDE_GRASS_FADE_END,strText);
	strText.Format("%.3f",tmpGrass.m_vWindDirectionAndStrength.x);
	SetDlgItemText(IDE_WIND_DIRECTION_X,strText);
	strText.Format("%.3f",tmpGrass.m_vWindDirectionAndStrength.y);
	SetDlgItemText(IDE_WIND_DIRECTION_Y,strText);
	strText.Format("%.3f",tmpGrass.m_vWindDirectionAndStrength.z);
	SetDlgItemText(IDE_WIND_DIRECTION_Z,strText);
	strText.Format("%.3f",tmpGrass.m_vWindDirectionAndStrength.w);
	SetDlgItemText(IDE_WIND_STRENGTH,strText);
	strText.Format("%.3f",tmpGrass.m_fGrassPeriod);
	SetDlgItemText(IDE_GRASS_PERIOD,strText);
}

void CTerrainGrassDlg::LoadAttributeFromMap()
{
	CSGPGrass* pGrass=(CSGPGrass*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getGrass());
	if(pGrass==NULL) return ;

	m_TexturePath=pGrass->GetGrassTextureName().toUTF8().getAddress();
	SetDlgItemInt(IDE_GRASS_TILE_X,(int)(pGrass->m_fTextureAtlasNbX));
	SetDlgItemInt(IDE_GRASS_TILE_Y,(int)(pGrass->m_fTextureAtlasNbY));

	// Update Grass List
	UpdateGrassList(m_TexturePath,(int)(pGrass->m_fTextureAtlasNbX),(int)(pGrass->m_fTextureAtlasNbY));
	TerrainGrassHandler::GetInstance()->SetGrassInfo(m_TexturePath,(int)(pGrass->m_fTextureAtlasNbX),(int)(pGrass->m_fTextureAtlasNbY));
	m_nGrassIndex = -1;
	TerrainGrassHandler::GetInstance()->SetTiledTexIndex(m_nGrassIndex);

	CString strText;
	strText.Format("%.2f",CSGPWorldConfig::getInstance()->m_fGrassFarFadingStart);
	SetDlgItemText(IDE_GRASS_FADE_START,strText);
	strText.Format("%.2f",CSGPWorldConfig::getInstance()->m_fGrassFarFadingEnd);
	SetDlgItemText(IDE_GRASS_FADE_END,strText);
	strText.Format("%.3f",pGrass->m_vWindDirectionAndStrength.x);
	SetDlgItemText(IDE_WIND_DIRECTION_X,strText);
	strText.Format("%.3f",pGrass->m_vWindDirectionAndStrength.y);
	SetDlgItemText(IDE_WIND_DIRECTION_Y,strText);
	strText.Format("%.3f",pGrass->m_vWindDirectionAndStrength.z);
	SetDlgItemText(IDE_WIND_DIRECTION_Z,strText);
	strText.Format("%.3f",pGrass->m_vWindDirectionAndStrength.w);
	SetDlgItemText(IDE_WIND_STRENGTH,strText);
	strText.Format("%.3f",pGrass->m_fGrassPeriod);
	SetDlgItemText(IDE_GRASS_PERIOD,strText);
}