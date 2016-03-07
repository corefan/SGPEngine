// TerrainWaterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainWaterDlg.h"
#include "afxdialogex.h"

#include "TerrainEditorDlg.h"
#include "ThumbnailGenerator.h"
#include "WorldEditorConfig.h"
#include "HelpFunction.h"
#include "MouseMsgInterface.h"
#include "TerrainWaterHandler.h"
#include "OperationManager.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <io.h>
// CTerrainWaterDlg 对话框

static int button_id3[]={IDB_ADD_WATER,IDB_DELETE_WATER};

CTerrainWaterDlg* CTerrainWaterDlg::s_pWaterDlg = NULL;

IMPLEMENT_DYNAMIC(CTerrainWaterDlg, CDialogEx)

CTerrainWaterDlg::CTerrainWaterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTerrainWaterDlg::IDD, pParent)
{

}

CTerrainWaterDlg::~CTerrainWaterDlg()
{
}

void CTerrainWaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_WATER_IMAGE,m_WaterImgCtrl);
}

BEGIN_MESSAGE_MAP(CTerrainWaterDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDB_EXPAND, &CTerrainWaterDlg::OnBnClickedExpand)
	ON_BN_CLICKED(IDB_SET_TEXTURE,OnImportTexture)
	ON_BN_CLICKED(IDB_ADD_WATER,OnAddWater)
	ON_BN_CLICKED(IDB_DELETE_WATER,OnDeleteWater)
	ON_BN_CLICKED(IDB_GENERATE_WATER,OnGenerateWater)
	ON_BN_CLICKED(IDB_SETTING_APPLY,OnApplySetting)
END_MESSAGE_MAP()

// CTerrainWaterDlg 消息处理程序

BOOL CTerrainWaterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ExpandButton.SubclassDlgItem(IDB_EXPAND,this);
	GetClientRect(m_rc);
	SetExpand(FALSE);

	for(int i=0;i<sizeof(button_id3)/sizeof(int);++i)
		m_Button[i].SubclassDlgItem(button_id3[i],this);

	InitDefaultValue();

	GetDlgItem(IDB_ADD_WATER)->EnableWindow(FALSE);
	GetDlgItem(IDB_DELETE_WATER)->EnableWindow(FALSE);

	s_pWaterDlg=this;
/*	// Set test value
	m_WaterImgCtrl.SetImage(WorldEditorConfig::GetInstance()->GetWaterTextureThumbnailDir()+"\\wavesbump.dds-262271.bmp",\
		WorldEditorConfig::GetInstance()->GetDataFileDir()+"\\Texture\\water\\wavesbump.dds");
	m_TexturePath = "Texture\\water\\wavesbump.dds";
	SetDlgItemText(IDE_WATER_HEIGHT,"60.0");*/

	return TRUE;
}


void CTerrainWaterDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(m_ExpandButton.m_hWnd!=NULL)
		m_ExpandButton.MoveWindow(0,0,cx,20);
}


BOOL CTerrainWaterDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CTerrainWaterDlg::SetExpand(BOOL bExpand)
{
	m_ExpandButton.SetExpand(bExpand);
	if(bExpand)
		SetWindowPos(NULL,0,0,m_rc.Width(),m_rc.Height(),SWP_NOMOVE);
	else
		SetWindowPos(NULL,0,0,m_rc.Width(),20+1,SWP_NOMOVE);
}

void CTerrainWaterDlg::OnBnClickedExpand()
{
	SetExpand(!m_ExpandButton.m_bExpanded);
	((CTerrainEditorDlg*)GetParent())->SetCtrlPos();
}

void CTerrainWaterDlg::OnImportTexture()
{
	TCHAR strFilter[] = _T("DDS Files (*.dds)|*.dds|TGA Files (.tga)|*.tga||");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter,this);
	if(dlg.DoModal() == IDOK)
	{
		CString texPath=dlg.GetPathName();
		CString workDir = WorldEditorConfig::GetInstance()->GetDataFileDir();

		if(!FileInFolder(texPath,workDir))
		{
			MessageBox("The texture must be in Data Directory!","Terrain Grass",MB_OK|MB_ICONERROR);
			return ;
		}

		CString texName = texPath.Right(texPath.GetLength()-texPath.ReverseFind('\\')-1);
		CString imgPath;
		CFileStatus status;
		CFile::GetStatus(texPath,status);
		imgPath.Format("%s\\%s-%I64u.bmp",WorldEditorConfig::GetInstance()->GetWaterTextureThumbnailDir(),\
			texName,status.m_size);
		m_TexturePath = GetRelativePath(texPath,WorldEditorConfig::GetInstance()->GetDataFileDir());
		BOOL bExist = TRUE;
		CRect rcImage;
		m_WaterImgCtrl.GetClientRect(rcImage);
		ThumbnailGenerator generator(this);
		if(_access(imgPath,0) == -1)
			bExist = generator.GenerateThumbnail(texPath,imgPath,rcImage.Width(),rcImage.Height());
		if(bExist) m_WaterImgCtrl.SetImage(imgPath,texPath);
	}
}

void CTerrainWaterDlg::OnAddWater()
{
	MouseMsgInterface::GetInstance()->SwitchToWaterEditor();
	TerrainWaterHandler::GetInstance()->SetBrushType(WaterType_Add);
//	SetButtonState(IDB_ADD_WATER);
	((CTerrainEditorDlg*)GetParent())->SetButtonState(IDB_ADD_WATER);
}

void CTerrainWaterDlg::OnDeleteWater()
{
	MouseMsgInterface::GetInstance()->SwitchToWaterEditor();
	TerrainWaterHandler::GetInstance()->SetBrushType(WaterType_Delete);
//	SetButtonState(IDB_DELETE_WATER);
	((CTerrainEditorDlg*)GetParent())->SetButtonState(IDB_DELETE_WATER);
}

void CTerrainWaterDlg::OnGenerateWater()
{
	if(m_TexturePath.IsEmpty())
	{
		MessageBox("Please set water texture firstly!","Terrain Water",MB_OK|MB_ICONSTOP);
		return ;
	}

	CString sWaterHeight;
	GetDlgItemText(IDE_WATER_HEIGHT,sWaterHeight);
	if(sWaterHeight.IsEmpty())
	{
		MessageBox("Please set water height firstly!","Terrain Water",MB_OK|MB_ICONSTOP);
		return ;
	}

	float fWaterHeight = (float)atof(sWaterHeight);
	TerrainWaterHandler* pWaterHandler = TerrainWaterHandler::GetInstance();

	CSGPWater* pWater=(CSGPWater*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater());
	if(pWater != NULL)
	{
		if(fWaterHeight == pWaterHandler->m_WaterHeight&&m_TexturePath==pWaterHandler->m_WaterTexPath) return ;//No changed
		CString strInfo;
		if(m_TexturePath != pWaterHandler->m_WaterTexPath) strInfo=_T("Water texture have been changed.\nIf you use new water texture, all previous water in the map will be removed !!!\nSo ,do you want to use new water texture ?");
		if(fWaterHeight != pWaterHandler->m_WaterHeight) strInfo=_T("Water texture have been height.\nIf you use new water height, all previous water in the map will be removed !!!\nSo ,do you want to use new water height ?");
		if(MessageBox(strInfo,"Terrain Water",MB_YESNO|MB_ICONWARNING)==IDNO) return ;
		((COpenGLRenderDevice*)(WorldEditorRenderInterface::GetInstance()->GetRenderDevice()))->getOpenGLWaterRenderer()->releaseWaterWaveTexture();
		delete pWater;
		OperationManager::GetInstance()->ClearWaterOperation();
	}

	BSTR texPath = m_TexturePath.AllocSysString();
	
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	pRenderInterface->GetWorldSystemManager()->createWater(fWaterHeight,String(texPath));
	TerrainWaterHandler::GetInstance()->SetWaterAttr(m_TexturePath,fWaterHeight);

	GetDlgItem(IDB_ADD_WATER)->EnableWindow(TRUE);
	GetDlgItem(IDB_DELETE_WATER)->EnableWindow(TRUE);

	SysFreeString(texPath);
}

void CTerrainWaterDlg::SetButtonState(int nSelID)
{
	BOOL bFind = FALSE;
	COLORREF normalColor=RGB(0,0,0),focusColor=RGB(255,0,255);
	for(int i=0;i<sizeof(button_id3)/sizeof(int);++i)
	{
		if(nSelID==button_id3[i])
		{
			m_Button[i].SetTextColor(focusColor);
			bFind = TRUE;
		}
		else m_Button[i].SetTextColor(normalColor);
		m_Button[i].Invalidate();
	}
	if(!bFind) TerrainWaterHandler::GetInstance()->SetBrushType(WaterType_None);
}

void CTerrainWaterDlg::OnApplySetting()
{
	CString str;
	CSGPWater* pWater = (CSGPWater*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater());
	if(pWater != NULL)
	{
		pWater->m_vWaterSurfaceColor.x = (float)GetDlgItemInt(IDE_WATER_COLOR_R)/255;
		pWater->m_vWaterSurfaceColor.y = (float)GetDlgItemInt(IDE_WATER_COLOR_G)/255;
		pWater->m_vWaterSurfaceColor.z = (float)GetDlgItemInt(IDE_WATER_COLOR_B)/255;

		GetDlgItemText(IDE_REFRACTION_BUMP_SCALE,str);
		pWater->m_fRefractionBumpScale = (float)atof(str);
		GetDlgItemText(IDE_REFLECTION_BUMP_SCALE,str);
		pWater->m_fReflectionBumpScale = (float)atof(str);
		GetDlgItemText(IDE_FRESNEL_BIAS,str);
		pWater->m_fFresnelBias = (float)atof(str);
		GetDlgItemText(IDE_FRESNEL_POW,str);
		pWater->m_fFresnelPow = (float)atof(str);
		GetDlgItemText(IDE_WATER_DEPTH_SCALE,str);
		pWater->m_fWaterDepthScale = (float)atof(str);
		GetDlgItemText(IDE_WATER_BLEND_SCALE,str);
		pWater->m_fWaterBlendScale = (float)atof(str);
		GetDlgItemText(IDE_WAVE_DIR_X,str);
		pWater->m_vWaveDir.x = (float)atof(str);
		GetDlgItemText(IDE_WAVE_DIR_Z,str);
		pWater->m_vWaveDir.y = (float)atof(str);
		GetDlgItemText(IDE_WAVE_SPEED,str);
		pWater->m_fWaveSpeed = (float)atof(str);
		GetDlgItemText(IDE_WAVE_RATE,str);
		pWater->m_fWaveRate = (float)atof(str);
		GetDlgItemText(IDE_SUN_SPECULAR_POWER,str);
		pWater->m_fWaterSunSpecularPower = (float)atof(str);
	}
}

void CTerrainWaterDlg::InitDefaultValue()
{
	// Default Attribute Setting
	CSGPWater tmpWater(1.0f);
	CString strText;
	SetDlgItemInt(IDE_WATER_COLOR_R,(int)(tmpWater.m_vWaterSurfaceColor.x*255.0f));
	SetDlgItemInt(IDE_WATER_COLOR_G,(int)(tmpWater.m_vWaterSurfaceColor.y*255.0f));
	SetDlgItemInt(IDE_WATER_COLOR_B,(int)(tmpWater.m_vWaterSurfaceColor.z*255.0f));
	strText.Format("%.3f",tmpWater.m_fRefractionBumpScale);
	SetDlgItemText(IDE_REFRACTION_BUMP_SCALE,strText);
	strText.Format("%.3f",tmpWater.m_fReflectionBumpScale);
	SetDlgItemText(IDE_REFLECTION_BUMP_SCALE,strText);
	strText.Format("%.3f",tmpWater.m_fFresnelBias);
	SetDlgItemText(IDE_FRESNEL_BIAS,strText);
	strText.Format("%.3f",tmpWater.m_fFresnelPow);
	SetDlgItemText(IDE_FRESNEL_POW,strText);
	strText.Format("%.3f",tmpWater.m_fWaterDepthScale);
	SetDlgItemText(IDE_WATER_DEPTH_SCALE,strText);
	strText.Format("%.3f",tmpWater.m_fWaterBlendScale);
	SetDlgItemText(IDE_WATER_BLEND_SCALE,strText);
	strText.Format("%.3f",tmpWater.m_vWaveDir.x);
	SetDlgItemText(IDE_WAVE_DIR_X,strText);
	strText.Format("%.3f",tmpWater.m_vWaveDir.y);
	SetDlgItemText(IDE_WAVE_DIR_Z,strText);
	strText.Format("%.3f",tmpWater.m_fWaveSpeed);
	SetDlgItemText(IDE_WAVE_SPEED,strText);
	strText.Format("%.3f",tmpWater.m_fWaveRate);
	SetDlgItemText(IDE_WAVE_RATE,strText);
	strText.Format("%.3f",tmpWater.m_fWaterSunSpecularPower);
	SetDlgItemText(IDE_SUN_SPECULAR_POWER,strText);
}

void CTerrainWaterDlg::EnableAllCtrls(BOOL bEnable)
{
	CWnd* pWnd = GetWindow(GW_CHILD);
	while(pWnd != NULL)
	{
		if(pWnd->GetDlgCtrlID()!=IDB_EXPAND)
			pWnd->EnableWindow(bEnable);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CTerrainWaterDlg::LoadAttributeFromMap()
{
	CSGPWater *pWater=(CSGPWater*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater());
	if(pWater==NULL) return ;

	CString strText;
	// Attribute Setting
	SetDlgItemInt(IDE_WATER_COLOR_R,(int)(pWater->m_vWaterSurfaceColor.x*255.0f));
	SetDlgItemInt(IDE_WATER_COLOR_G,(int)(pWater->m_vWaterSurfaceColor.y*255.0f));
	SetDlgItemInt(IDE_WATER_COLOR_B,(int)(pWater->m_vWaterSurfaceColor.z*255.0f));
	strText.Format("%.3f",pWater->m_fRefractionBumpScale);
	SetDlgItemText(IDE_REFRACTION_BUMP_SCALE,strText);
	strText.Format("%.3f",pWater->m_fReflectionBumpScale);
	SetDlgItemText(IDE_REFLECTION_BUMP_SCALE,strText);
	strText.Format("%.3f",pWater->m_fFresnelBias);
	SetDlgItemText(IDE_FRESNEL_BIAS,strText);
	strText.Format("%.3f",pWater->m_fFresnelPow);
	SetDlgItemText(IDE_FRESNEL_POW,strText);
	strText.Format("%.3f",pWater->m_fWaterDepthScale);
	SetDlgItemText(IDE_WATER_DEPTH_SCALE,strText);
	strText.Format("%.3f",pWater->m_fWaterBlendScale);
	SetDlgItemText(IDE_WATER_BLEND_SCALE,strText);
	strText.Format("%.3f",pWater->m_vWaveDir.x);
	SetDlgItemText(IDE_WAVE_DIR_X,strText);
	strText.Format("%.3f",pWater->m_vWaveDir.y);
	SetDlgItemText(IDE_WAVE_DIR_Z,strText);
	strText.Format("%.3f",pWater->m_fWaveSpeed);
	SetDlgItemText(IDE_WAVE_SPEED,strText);
	strText.Format("%.3f",pWater->m_fWaveRate);
	SetDlgItemText(IDE_WAVE_RATE,strText);
	strText.Format("%.3f",pWater->m_fWaterSunSpecularPower);
	SetDlgItemText(IDE_SUN_SPECULAR_POWER,strText);

	CString texPath=WorldEditorConfig::GetInstance()->GetDataFileDir()+CString("\\")+CString(pWater->m_WaterWaveTextureName.toUTF8().getAddress());
	CString texName=PathFindFileName(texPath);
	texName=texName.Left(texName.ReverseFind('.'))+".bmp";
	CString imgPath=WorldEditorConfig::GetInstance()->GetWaterTextureThumbnailDir()+CString("\\")+texName;
	m_WaterImgCtrl.SetImage(imgPath,texPath);
	m_TexturePath = pWater->m_WaterWaveTextureName.toUTF8().getAddress();

	strText.Format("%.2f",pWater->m_fWaterHeight);
	SetDlgItemText(IDE_WATER_HEIGHT,strText);

	TerrainWaterHandler::GetInstance()->SetWaterAttr(m_TexturePath,pWater->m_fWaterHeight);
}