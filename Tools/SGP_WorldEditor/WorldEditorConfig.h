#pragma once

#include <vector>

class WorldEditorConfig
{
	WorldEditorConfig();
	~WorldEditorConfig();
	static WorldEditorConfig* s_Instance;
public:
	static WorldEditorConfig* GetInstance();
	static void ReleaseInstance();
public:
	CString GetExeDir();
	CString GetDataFileDir();
	CString GetWorldMapDir();
	CString GetTerrainTextureThumbnailDir();
	CString GetGrassTextureThumbnailDir();
	CString GetWaterTextureThumbnailDir();
	CString GetWorldEditorConfigPath();

	CString m_ExePath;
	CString m_DataPath;
	CString GetThumbnailDir();
	CString GetTerrainTextureRelativeDir();
	void SaveTerrainTextureRelativeDir(CString strPath);
	bool IsTerrainTexturePathExist();
	CString GetTerrainTextureDir();
public:
	BOOL m_bShowChunkGrid;
	BOOL m_bShowChunk;
	BOOL m_bShowWireframe;
	BOOL m_bShowLights;
	BOOL m_bShowWater;
	BOOL m_bShowObjects;

	void GetRecentFiles(std::vector<CString>& fileVector);
	void SaveRecentFiles(CString lastPath);
	void DeleteRecentFiles(CString filePath);
};