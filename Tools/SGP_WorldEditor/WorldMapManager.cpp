#include "stdafx.h"
#include "WorldEditor.h"
#include "WorldMapManager.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "TerrainTextureDlg.h"
#include <algorithm>
#include "SceneObjectManager.h"
#include "HelpFunction.h"
#include "WorldEditorConfig.h"
#include "OperationManager.h"
#include "LightmapConfigDlg.h"
#include "LightMapBuildDlg.h"
#include "SelectObjHandler.h"
#include "WorldEditorConfig.h"
#include "SkydomeSettingDlg.h"
#include "TerrainWaterDlg.h"
#include "TerrainGrassDlg.h"
#include "SceneObjectTree.h"
#include "MouseMsgInterface.h"
#include "MapLoadingDlg.h"
#include "NewMapConfigDlg.h"
#include "NewMapLoadingDlg.h"
#include "MultiPlatformResPackupDlg.h"
#include "PackupResProgressDlg.h"
#include "EditorPane.h"

ESGPTerrainChunkTexture ChunkLayer[7]=\
	{\
		eChunk_Diffuse0Texture,\
		eChunk_Diffuse1Texture,\
		eChunk_Diffuse2Texture,\
		eChunk_Diffuse3Texture,\
		eChunk_NormalMapTexture,\
		eChunk_DetailMapTexture,\
		eChunk_SlopeMapTexture\
	};

IMPLEMENT_SINGLETON(WorldMapManager)

WorldMapManager::WorldMapManager()
{
	m_pNewAlphaTexData = NULL;
	m_bHaveMap = false;
	m_bOpenFile = false;
	m_pWorldMap = NULL;
	m_bRefreshCollSet=true;
}

WorldMapManager::~WorldMapManager()
{
	PurgeResource();
}

void WorldMapManager::SetChunkTexture(int chunkID,int nLayer,CString texturePath)
{
	// set texture
	BSTR texPath = texturePath.AllocSysString();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->setTerrainChunkLayerTexture((uint32)chunkID,ChunkLayer[nLayer],String(texPath));
	SysFreeString(texPath);

	// store information
	if(texturePath.IsEmpty())
	{
		ASSERT(nLayer != 0);
		m_pWorldMap->m_pChunkTextureIndex[chunkID].m_ChunkTextureIndex[ChunkLayer[nLayer]]=-1;
		return ;
	}

	SGPWorldMapChunkTextureNameTag textureNameTag;
	strcpy_s(textureNameTag.m_ChunkTextureFileName,LPCTSTR(texturePath));

	int i,size=m_TexNameVector.size();
	for(i=0;i<size;++i)
	{
		if(m_TexNameVector[i]==textureNameTag) break;
	}

	m_pWorldMap->m_pChunkTextureIndex[chunkID].m_ChunkTextureIndex[ChunkLayer[nLayer]]=i;
	if(i>=size) m_TexNameVector.push_back(textureNameTag);
}

CString WorldMapManager::GetChunkTexture(int chunkID,int nLayer)
{
	int texIndex = m_pWorldMap->m_pChunkTextureIndex[chunkID].m_ChunkTextureIndex[ChunkLayer[nLayer]];
	if(texIndex<0) return CString("");
	else return CString(m_TexNameVector[texIndex].m_ChunkTextureFileName);
}

void WorldMapManager::CloseLayer(BOOL layer1Close,BOOL layer2Close,BOOL layer3Close)
{
	if(m_pNewAlphaTexData==NULL)
		m_pNewAlphaTexData = new uint32[m_pWorldMap->m_Header.m_iChunkAlphaTextureSize*m_pWorldMap->m_Header.m_iChunkAlphaTextureSize];

	uint32* pAlphaTexData = m_pWorldMap->m_WorldChunkAlphaTextureData;
	uint32 mask = 0xFFFFFFFF;
	if(layer1Close)
		mask &= 0xFF00FFFF;
	if(layer2Close)
		mask &= 0xFFFF00FF;
	if(layer3Close)
		mask &= 0xFFFFFF00;
	uint32 buffSize = m_pWorldMap->m_Header.m_iChunkAlphaTextureSize*m_pWorldMap->m_Header.m_iChunkAlphaTextureSize;
	for(uint32 i=0;i<buffSize;++i)
		m_pNewAlphaTexData[i] = pAlphaTexData[i] & mask;
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->editTerrainChunkBlendTexture(0,0,\
		m_pWorldMap->m_Header.m_iChunkAlphaTextureSize,m_pWorldMap->m_Header.m_iChunkAlphaTextureSize,m_pNewAlphaTexData);
}

void WorldMapManager::EditTerrainChunkBlendTexture(uint32 SrcX,uint32 SrcZ,uint32 width,uint32 height)
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	uint32 alphaDataSize = m_pWorldMap->m_Header.m_iChunkAlphaTextureSize * m_pWorldMap->m_Header.m_iChunkAlphaTextureSize;
	uint32 mask = 0xFFFFFFFF;
	if(CTerrainTextureDlg::GetInstance()->IsLayerClosed(1)) mask &= 0xFF00FFFF;
	if(CTerrainTextureDlg::GetInstance()->IsLayerClosed(2)) mask &= 0xFFFF00FF;
	if(CTerrainTextureDlg::GetInstance()->IsLayerClosed(3)) mask &= 0xFFFFFF00;
	if(mask == 0xFFFFFFFF)
	{
		pRenderInterface->GetWorldSystemManager()->editTerrainChunkBlendTexture(SrcX,SrcZ,width,height,m_pWorldMap->m_WorldChunkAlphaTextureData);
	}
	else
	{
		if(m_pNewAlphaTexData==NULL)
			m_pNewAlphaTexData = new uint32[m_pWorldMap->m_Header.m_iChunkAlphaTextureSize*m_pWorldMap->m_Header.m_iChunkAlphaTextureSize];
		for(uint32 i=0;i<alphaDataSize;++i) m_pNewAlphaTexData[i] = m_pWorldMap->m_WorldChunkAlphaTextureData[i] & mask;
		pRenderInterface->GetWorldSystemManager()->editTerrainChunkBlendTexture(SrcX,SrcZ,width,height,m_pNewAlphaTexData);
	}
}

CString WorldMapManager::GetObjSceneNameByPath(CString& path)
{
	int count=0;
	for(uint32 i=0;i<m_ObjectVector.size();++i)
	{
		if(path==m_ObjectVector[i]->m_MF1FileName) ++count;
	}
	CString strName=path.Right(path.GetLength()-path.ReverseFind('\\')-1);
	strName=strName.Left(strName.ReverseFind('.'));

	CString sceneName;
	sceneName.Format("%s_%02d",strName,count+1);

	return sceneName;
}

CString WorldMapManager::GetLightObjSceneName()
{
	int count=m_LightObjVector.size();
	CString sceneName;
	sceneName.Format("PointLight_%02d",count+1);
	return sceneName;
}

void WorldMapManager::AddSceneObject(const CommonObject& obj)
{
	if(obj.IsMF1())
	{
		m_ObjectVector.push_back(obj.m_pObj);
	}
	else if(obj.IsLight())
	{
		std::vector<ISGPLightObject*>::iterator iter=find(m_LightObjVector.begin(),m_LightObjVector.end(),obj.m_pLightObj);
		if(iter==m_LightObjVector.end()) m_LightObjVector.push_back(obj.m_pLightObj);
	}
	else ASSERT(FALSE);
}

void WorldMapManager::DeleteSceneObject(const CommonObject& obj)
{
	if(obj.IsMF1())
	{
		std::vector<ISGPObject*>::iterator iter=find(m_ObjectVector.begin(),m_ObjectVector.end(),obj.m_pObj);
		if(iter!=m_ObjectVector.end()) m_ObjectVector.erase(iter);
	}
	else if(obj.IsLight())
	{
		std::vector<ISGPLightObject*>::iterator iter=find(m_LightObjVector.begin(),m_LightObjVector.end(),obj.m_pLightObj);
		if(iter!=m_LightObjVector.end())
			m_LightObjVector.erase(iter);
	}
}

bool WorldMapManager::CheckExist(CString mapPath)
{
	CString strExt=PathFindExtension(mapPath);
	strExt.MakeLower();
	if(PathFileExists(mapPath)&&strExt==".map") return true;
	AfxGetMainWnd()->MessageBox(mapPath+" don't exist!","WorldEditor",MB_OK|MB_ICONERROR);
	return false;
}

void WorldMapManager::LoadMap(CString mapPath)
{
	CloseMap();
	if(!CheckExist(mapPath)) return ;

	CWaitCursor cursor;
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	ISGPWorldSystemManager* pWorldManager=pRenderInterface->GetWorldSystemManager();
	// load Map
	BSTR path = mapPath.AllocSysString();
	pWorldManager->loadWorldFromFile(String(""),String(path),false);
	SysFreeString(path);

	m_pWorldMap = pRenderInterface->GetWorldMap();
	// Chunk Texture
	ASSERT(m_TexNameVector.empty());
	for(uint32 i=0;i<m_pWorldMap->m_Header.m_iChunkTextureNameNum;++i)
		m_TexNameVector.push_back(m_pWorldMap->m_pChunkTextureNames[i]);

	// Grass Data
	CSGPTerrain* pTerrain=(CSGPTerrain*)(pWorldManager->getTerrain());
	if(m_pWorldMap->m_GrassData.m_ppChunkGrassCluster!=NULL)
	{
		SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** pGrassClusterTag=new SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* [m_pWorldMap->m_Header.m_iTerrainSize*m_pWorldMap->m_Header.m_iTerrainSize];
		for(uint32 i=0;i<m_pWorldMap->m_GrassData.m_nChunkGrassClusterNum;++i)
		{
			if(m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[i]!=NULL)
			{
				SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* pNewGrassClusterData = new SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag;
				memcpy(pNewGrassClusterData,m_pWorldMap->m_GrassData.m_ppChunkGrassCluster[i],sizeof(SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag));
				pGrassClusterTag[i]=pNewGrassClusterData;
				CSGPTerrainChunk* pGrassTerrainChunk=pTerrain->m_TerrainChunks[pNewGrassClusterData->m_nChunkIndex];
				pGrassTerrainChunk->SetChunkGrassCluster(pNewGrassClusterData->m_GrassLayerData,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION);
			}
			else
				pGrassClusterTag[i]=NULL;
		}
		m_pWorldMap->m_GrassData.m_ppChunkGrassCluster=pGrassClusterTag;
	}

	// Set Flag
	m_bHaveMap = true;
	m_bOpenFile = true;
	m_MapName=FileTitleFromPath(mapPath);
	m_MapFolder = FileFolderFromPath(mapPath);

	NotifyRefreshCollSet();
	CEditorPane::GetInstance()->EnableAllCtrls(TRUE);

	CMainFrame::GetInstance()->SetMapName(m_MapName);
	CSkydomeSettingDlg::GetInstance()->LoadAttributeFromMap();
	CTerrainWaterDlg::GetInstance()->LoadAttributeFromMap();
	CTerrainGrassDlg::GetInstance()->LoadAttributeFromMap();

	CMapLoadingDlg dlg;
	dlg.DoModal();
	pWorldManager->initializeQuadTree();
}

void WorldMapManager::CreateMap()
{
	CNewMapLoadingDlg loadingDlg;
	CNewMapConfigDlg configDlg(&loadingDlg);
	if(configDlg.DoModal()==IDOK)
	{
		m_MapFolder = loadingDlg.m_MapFolder;
		CloseMap();
		loadingDlg.DoModal();
	}
}

void WorldMapManager::CreateMap(CString mapName,int terrainSize,bool bUsePerlinNoise,int maxTerrainHeight,CString layer0TexPath)
{
	CloseMap();

	m_pWorldMap = new CSGPWorldMap;
	
	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	pWorldManager->createWorldSun();
	BSTR texPath = layer0TexPath.AllocSysString();
	pWorldManager->createNewWorld(m_pWorldMap,mapName,(SGP_TERRAIN_SIZE)terrainSize,bUsePerlinNoise,(uint16)maxTerrainHeight,String(texPath));
	SysFreeString(texPath);
	pWorldManager->setWorldSunPosition(0.0f);
	pWorldManager->initializeQuadTree();

	// Chunk Texture
	ASSERT(m_TexNameVector.empty());
	for(uint32 i=0;i<m_pWorldMap->m_Header.m_iChunkTextureNameNum;++i)
		m_TexNameVector.push_back(m_pWorldMap->m_pChunkTextureNames[i]);

	// Alpha TextureSize
	m_pNewAlphaTexData = new uint32[m_pWorldMap->m_Header.m_iChunkAlphaTextureSize*m_pWorldMap->m_Header.m_iChunkAlphaTextureSize];

	// Set Flag
	m_bHaveMap = true;
	m_bOpenFile = false;
	m_MapName = mapName;
	NotifyRefreshCollSet();
	CMainFrame::GetInstance()->SetMapName(m_MapName);
	CEditorPane::GetInstance()->EnableAllCtrls(TRUE);
	OperationManager::GetInstance()->SetModifyFlag();
}

void WorldMapManager::SaveMap()
{
	if(!m_bHaveMap) return ;

	CString mapPath = m_MapFolder+"\\"+m_MapName+".map";
	SaveMapAs(mapPath);
}

void WorldMapManager::PreSaveMap()
{
	// Texture Name
	std::vector<SGPWorldMapChunkTextureNameTag> texNameVector;
	int size=m_TexNameVector.size();
	for(int i=0;i<size;++i)
	{
		int currIndex = texNameVector.size();
		bool finded = false;
		for(uint32 j=0;j<m_pWorldMap->m_Header.m_iTerrainSize*m_pWorldMap->m_Header.m_iTerrainSize;++j)
		{
			for(int nLayer=0;nLayer<7;++nLayer)
			{
				if(m_pWorldMap->m_pChunkTextureIndex[j].m_ChunkTextureIndex[ChunkLayer[nLayer]]==i)
				{
					m_pWorldMap->m_pChunkTextureIndex[j].m_ChunkTextureIndex[ChunkLayer[nLayer]]=currIndex;
					finded=true;
				}
			}
		}
		if(finded) texNameVector.push_back(m_TexNameVector[i]);
	}
	m_TexNameVector.swap(texNameVector);

	uint32 nameCount=m_TexNameVector.size();
	m_pTmpTextureNameTag = m_pWorldMap->m_pChunkTextureNames;
	if(nameCount==0) m_pWorldMap->m_pChunkTextureNames = NULL;
	else m_pWorldMap->m_pChunkTextureNames=&(m_TexNameVector[0]);
	m_pWorldMap->m_Header.m_iChunkTextureNameNum=nameCount;

	MouseMsgInterface::GetInstance()->CheckUncompleteOp();
}

void WorldMapManager::AfterSaveMap()
{
	m_pWorldMap->m_pChunkTextureNames = m_pTmpTextureNameTag;
	OperationManager::GetInstance()->ClearModifyFlag();
}

void WorldMapManager::SaveMapAs(CString mapPath)
{
	if(!m_bHaveMap) return ;

	PreSaveMap();
	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	pWorldManager->saveWorldToFile(String(""),String((LPCTSTR)mapPath));
	AfterSaveMap();
}

bool WorldMapManager::CloseMap()
{
	if(!m_bHaveMap) return true;

	// need save
	if(OperationManager::GetInstance()->NeedSave())
	{
		CString strInfo=_T("The map have been changed!\n\nDo you want to save it ?");
		int nResult=AfxGetMainWnd()->MessageBox(strInfo,"Save Map",MB_YESNOCANCEL|MB_ICONQUESTION);
		switch(nResult)
		{
		case IDYES:
			SaveMap();
			break;
		case IDNO:
			break;
		case IDCANCEL:
			return false;
			break;
		}
	}

	CString mapPath = m_MapFolder+"\\"+m_MapName+".map";
	if(PathFileExists(mapPath))
		WorldEditorConfig::GetInstance()->SaveRecentFiles(mapPath);

	CSceneObjectTree::GetInstance()->ResetAllItems();
	OperationManager::GetInstance()->CloseMap();
	CEditorPane::GetInstance()->EnableAllCtrls(FALSE);
	MouseMsgInterface::GetInstance()->CheckUncompleteOp();
	CFindResultList::GetInstance()->PurgeResource();

	ReleaseMap();
	return true;
}

void WorldMapManager::ReleaseMap()
{
	if(m_pWorldMap != NULL)
	{
		if(!m_bOpenFile)
		{
			m_pWorldMap->Release();
			SAFE_DELETE(m_pWorldMap);
		}
		else
		{
			SGPWorldMapGrassTag& grassTag=m_pWorldMap->m_GrassData;
			if(grassTag.m_ppChunkGrassCluster!=NULL)
			{
				for(uint32 i=0;i<grassTag.m_nChunkGrassClusterNum;++i)
				{
					if(grassTag.m_ppChunkGrassCluster[i]!=NULL)
					{
						delete grassTag.m_ppChunkGrassCluster[i];
						grassTag.m_ppChunkGrassCluster[i]=NULL;
					}
				}
				delete [] grassTag.m_ppChunkGrassCluster;
				grassTag.m_ppChunkGrassCluster=NULL;
			}
		}
	}

	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	if(m_bHaveMap) pWorldManager->shutdownWorld();

	PurgeResource();
	m_bHaveMap=false;
	m_bOpenFile=false;
}

void WorldMapManager::PurgeResource()
{
	for(uint32 i=0;i<m_ObjectVector.size();++i) delete m_ObjectVector[i];
	m_ObjectVector.clear();
	for(uint32 i=0;i<m_LightObjVector.size();++i) delete m_LightObjVector[i];
	m_LightObjVector.clear();
	
	m_TexNameVector.clear();
	SAFE_DELETE_ARRAY(m_pNewAlphaTexData);
	m_bHaveMap=false;
	m_bOpenFile=false;
}

void WorldMapManager::BuildLightMap()
{
	CLightMapBuildDlg buildDlg;
	CLightmapConfigDlg configDlg(&buildDlg);
	if(configDlg.DoModal()==IDOK) buildDlg.DoModal();
}

void WorldMapManager::PackupResourceForMultiPlatform()
{
	CPackupResProgressDlg progressDlg;
	CMultiPlatformResPackupDlg dlg(&progressDlg);
	if(dlg.DoModal()==IDOK) progressDlg.DoModal();
}