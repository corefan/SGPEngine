// LightMapBuildDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "LightMapBuildDlg.h"
#include "afxdialogex.h"

#include "..\SGPLibraryCode\SGPHeader.h"
#include "Render Interface\WorldEditorRenderInterface.h"
#include "HelpFunction.h"

// CLightMapBuildDlg 对话框

//#include "PVRTexture.h"
#include "PVRTextureUtilities.h"

IMPLEMENT_DYNAMIC(CLightMapBuildDlg, CDialogEx)

CLightMapBuildDlg::CLightMapBuildDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLightMapBuildDlg::IDD, pParent)
{
	m_ProcessRatio=0.0f;
	m_bReady=false;
}

CLightMapBuildDlg::~CLightMapBuildDlg()
{
}

void CLightMapBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
}

BEGIN_MESSAGE_MAP(CLightMapBuildDlg, CDialogEx)
	ON_WM_TIMER()
	ON_MESSAGE(UM_UPDATE_OBJ_TEXTURE,OnUpdateObjTexture)
	ON_MESSAGE(UM_UPDATE_TERRAIN_TEXTURE,OnUpdateTerrainTexture)
END_MESSAGE_MAP()

// CLightMapBuildDlg 消息处理程序

void __cdecl BuildLightMapThread(void* pArguments)
{
	CLightMapBuildDlg* pBuildDlg=(CLightMapBuildDlg*)pArguments;
	ISGPWorldSystemManager* pWorldSystemManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();

	if(WorldMapManager::GetInstance()->m_bRefreshCollSet)
	{
		// initialize CollisionSet;
		pBuildDlg->SetMode(false);
		pBuildDlg->SetProcessInfo("Prepare for building lightmap");
		pWorldSystemManager->initializeCollisionSet();
		WorldMapManager::GetInstance()->ClearRefreshCollSet();
	}

	pBuildDlg->SetMode(true);
	Random* pRandom = WorldEditorRenderInterface::GetInstance()->GetDevice()->getRandomizer();
	// terrain
	if(pBuildDlg->m_bIncludeTerrain)
	{
		pBuildDlg->SetProcessInfo("Generate Terrain Lightmap Texture");
		uint32 *lMap=pWorldSystemManager->updateTerrainLightmapTexture(pBuildDlg->m_ProcessRatio,pRandom);
		pBuildDlg->SendMessage(UM_UPDATE_TERRAIN_TEXTURE,(WPARAM)lMap,0);
	}
	// selected objects
	if(pBuildDlg->m_bIncludeSeledObjects)
	{
		std::vector<CommonObject>& selectedObj=SelectObjHandler::GetInstance()->GetSelectedObj();
		uint32 mf1Count = 0;
		for(uint32 i=0;i<selectedObj.size();++i)
		{
			if(selectedObj[i].IsMF1()&&selectedObj[i].m_pObj->m_bReceiveLight) ++mf1Count;
		}
		uint32 index=0;
		for(uint32 i=0;i<selectedObj.size();++i)
		{
			if(selectedObj[i].IsMF1()&&selectedObj[i].m_pObj->m_bReceiveLight)
			{
				index++;
				CString strInfo;
				strInfo.Format("Generate Lightmap Texture for StaticMesh:%s. (%u/%u)",selectedObj[i].m_pObj->m_SceneObjectName,index,mf1Count);
				pBuildDlg->SetProcessInfo(strInfo);
				uint32 width,height;
				width=height=pBuildDlg->GetObjLightmapSize(selectedObj[i]);
				uint32 *lMap=pWorldSystemManager->updateSceneObjectLightmapTexture(pBuildDlg->m_ProcessRatio,selectedObj[i].m_pObj,width,height,pRandom);
				pBuildDlg->SendMessage(UM_UPDATE_OBJ_TEXTURE,(WPARAM)lMap,(LPARAM)(selectedObj[i].m_pObj));
			}
		}
	}

	pBuildDlg->SendMessage(WM_CLOSE);
}

BOOL CLightMapBuildDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Progress.SetRange(0,100);
	m_Progress.SetPos(0);
	SetDlgItemText(IDC_CURR_INFO,"");
	SetDlgItemText(IDC_CURR_RATIO,"");
	SetDlgItemText(IDC_INFO_IDLE,"");

	GetDlgItem(IDC_INFO_IDLE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CURR_RATIO)->ShowWindow(SW_HIDE);

	SetTimer(0,400,NULL);
	UpdateCtrls();
	_beginthread(&BuildLightMapThread,0,(void*)this);

	return TRUE;
}

BOOL CLightMapBuildDlg::PreTranslateMessage(MSG* pMsg)
{
	//avoid ESC/OK to exit dialog
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_RETURN) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLightMapBuildDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateCtrls();

	CDialogEx::OnTimer(nIDEvent);
}

void CLightMapBuildDlg::UpdateCtrls()
{
	CString strText;
	GetDlgItemText(IDC_CURR_INFO,strText);
	if(strText!=m_CurrProcessInfo) SetDlgItemText(IDC_CURR_INFO,m_CurrProcessInfo);
	if(m_bReady)
	{
		if(GetDlgItem(IDC_INFO_IDLE)->IsWindowVisible())
			GetDlgItem(IDC_INFO_IDLE)->ShowWindow(SW_HIDE);
		if(!GetDlgItem(IDC_PROGRESS)->IsWindowVisible())
			GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_SHOW);
		if(!GetDlgItem(IDC_CURR_RATIO)->IsWindowVisible())
			GetDlgItem(IDC_CURR_RATIO)->ShowWindow(SW_SHOW);

		CString str;
		str.Format("%.2f%%",m_ProcessRatio*100);
		GetDlgItemText(IDC_CURR_RATIO,strText);
		if(str!=strText) SetDlgItemText(IDC_CURR_RATIO,str);
		int currPos=(int)(m_ProcessRatio*100);
		if(m_Progress.GetPos()!=currPos) m_Progress.SetPos(currPos);
	}
	else
	{
		static int index=-1;
		index++;
		index=index%4;
		CString str;
		str=_T("Start building Collision Tree");
		for(int i=0;i<index;++i) str+=".";
		SetDlgItemText(IDC_INFO_IDLE,str);
	}
}

void CLightMapBuildDlg::SetMode(bool bReady)
{
	m_bReady = bReady;
}

void CLightMapBuildDlg::GenerateLightmapTexture(uint32 width,uint32 height,uint32* lMap,CString strPath)
{
	char* pData=(char*)lMap;
	for(uint32 i=0;i<width*height;++i)
	{
		char tmp = pData[4*i];
		pData[4*i] = pData[4*i+2];
		pData[4*i+2] = tmp;
	}

	using namespace pvrtexture;
	PixelType argbType;
	argbType=PixelType('r','g','b','a',8,8,8,8);
	CPVRTextureHeader cHeader(argbType.PixelTypeID,height,width);
	CPVRTexture cTexture(cHeader,lMap);
	GenerateMIPMaps(cTexture,eResizeLinear);
	Transcode(cTexture,ePVRTPF_BC3,ePVRTVarTypeUnsignedByteNorm,ePVRTCSpacelRGB);
	cTexture.saveFile((LPCTSTR)strPath);
}

LRESULT CLightMapBuildDlg::OnUpdateTerrainTexture(WPARAM wParam,LPARAM lParam)
{
	uint32* lMap=(uint32*)wParam;
	uint32 width,height;
	width=height=WorldEditorRenderInterface::GetInstance()->GetChunkSize()*SGPTT_TILENUM*SGPTLD_LIGHTMAPTEXTURE_DIMISION;
	((COpenGLRenderDevice*)(WorldEditorRenderInterface::GetInstance()->GetRenderDevice()))->getOpenGLTerrainRenderer()->updateLightmapTexture(width,height,lMap);
	
	CString strPath=m_TextureFolder+"\\"+CString("TerrainLightmap.dds");
	GenerateLightmapTexture(width,height,lMap,strPath);

	delete [] lMap;
	return 0;
}

LRESULT CLightMapBuildDlg::OnUpdateObjTexture(WPARAM wParam,LPARAM lParam)
{
	uint32* lMap=(uint32*)wParam;
	ISGPObject* pObj=(ISGPObject*)lParam;
	uint32 width,height;
	width=height=GetObjLightmapSize(CommonObject(pObj));
	CStaticMeshInstance* pMeshInstance=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()\
		->getMeshInstanceBySceneID(pObj->getSceneObjectID());
	pMeshInstance->updateLightmapTexture(width,height,lMap);

	CString strPath=m_TextureFolder+CString("\\")+CString(pObj->m_SceneObjectName)+CString(".dds");
	GenerateLightmapTexture(width,height,lMap,strPath);

	delete [] lMap;
	return 0;
}

uint32 CLightMapBuildDlg::GetObjLightmapSize(CommonObject& obj)
{
	ASSERT(obj.IsMF1());
	if(m_bDefaultLightmapSize)
	{
		AABBox aabox;
		GetObjectAABox(obj,aabox);
		Vector3D vec=aabox.vcMax-aabox.vcMin;
		float maxLength=jmax(vec.x,vec.y);
		maxLength=jmax(maxLength,vec.z);
		maxLength=maxLength*SGPTLD_LIGHTMAPTEXTURE_DIMISION/*/SGPTT_TILE_METER*/;
		uint32 length=(uint32)maxLength;
		uint32 result=DEFAULT_LIGHTMAP_MINSIZE;
		while(result<length) result*=2;
		if(result>DEFAULT_LIGHTMAP_MAXSIZE) result=DEFAULT_LIGHTMAP_MAXSIZE;
		return result;
	}
	else return m_TextureWidth;
}
