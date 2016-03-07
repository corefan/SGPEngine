#pragma once

#include "..\SGPLibraryCode\SGPHeader.h"
#include <vector>
#include "CommonObjDef.h"

class WorldMapManager
{
	WorldMapManager();
	~WorldMapManager();
	DECLARE_SINGLETON(WorldMapManager)
public:
	void SetChunkTexture(int chunkID,int nLayer,CString texturePath);
	CString GetChunkTexture(int chunkID,int nLayer);
	void CloseLayer(BOOL layer1Close,BOOL layer2Close,BOOL layer3Close);
	void EditTerrainChunkBlendTexture(uint32 SrcX,uint32 SrcZ,uint32 width,uint32 height);
public:// Object Interface
	void AddSceneObject(const CommonObject& obj);
	void DeleteSceneObject(const CommonObject& obj);
	CString GetObjSceneNameByPath(CString& path);
	CString GetLightObjSceneName();
	std::vector<ISGPObject*>& GetWorldObjects(){return m_ObjectVector;}
	std::vector<ISGPLightObject*>& GetLightObjects(){return m_LightObjVector;}
public:// map load and save interface
	void LoadMap(CString mapPath);
	void CreateMap();
	void CreateMap(CString mapName,int terrainSize,bool bUsePerlinNoise,int maxTerrainHeight,CString layer0TexPath);
	void SaveMap();
	void PreSaveMap();
	void AfterSaveMap();
	void SaveMapAs(CString mapPath);
	bool CloseMap();
	void ReleaseMap();
	void PurgeResource();
	bool CheckExist(CString mapPath);
public:// lightmap interface
	void BuildLightMap();
	void PackupResourceForMultiPlatform();
	void NotifyRefreshCollSet(){m_bRefreshCollSet=true;}
	void ClearRefreshCollSet(){m_bRefreshCollSet=false;}
public:
	std::vector<SGPWorldMapChunkTextureNameTag> m_TexNameVector;
	SGPWorldMapChunkTextureNameTag* m_pTmpTextureNameTag;
	uint32* m_pNewAlphaTexData;// used for close layer
	std::vector<ISGPObject*> m_ObjectVector;
	std::vector<ISGPLightObject*> m_LightObjVector;
	CSGPWorldMap* m_pWorldMap;
	CString m_MapName;
	CString m_MapFolder;

	bool m_bHaveMap;
	bool m_bOpenFile;
	bool m_bRefreshCollSet;
};