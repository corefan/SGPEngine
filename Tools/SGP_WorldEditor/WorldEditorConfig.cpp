#include "stdafx.h"
#include "WorldEditorConfig.h"
#include <algorithm>

WorldEditorConfig* WorldEditorConfig::s_Instance = NULL;

WorldEditorConfig::WorldEditorConfig()
{
	m_bShowChunkGrid = FALSE;
	m_bShowChunk = FALSE;
	m_bShowWireframe = FALSE;
	m_bShowLights = TRUE;
	m_bShowWater = TRUE;
	m_bShowObjects = TRUE;
}

WorldEditorConfig::~WorldEditorConfig()
{

}

WorldEditorConfig* WorldEditorConfig::GetInstance()
{
	if(s_Instance == NULL)
		s_Instance = new WorldEditorConfig;
	return s_Instance;
}

void WorldEditorConfig::ReleaseInstance()
{
	if(s_Instance != NULL)
	{
		delete s_Instance;
		s_Instance = NULL;
	}
}

CString WorldEditorConfig::GetExeDir()
{
	if(m_ExePath.IsEmpty())
	{
		TCHAR path[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(),path,MAX_PATH);
		m_ExePath=path;
		m_ExePath=m_ExePath.Left(m_ExePath.ReverseFind('\\'));
	}
	
	return m_ExePath;
}

CString WorldEditorConfig::GetDataFileDir()
{
	if(m_DataPath.IsEmpty())
	{
		CString path=GetExeDir();
		while(path.Right(4)!=_T("\\Bin"))
			path=path.Left(path.ReverseFind('\\'));
		path=path.Left(path.ReverseFind('\\'));
		m_DataPath=path+"\\Data";
	}
	return m_DataPath;
}

CString WorldEditorConfig::GetWorldMapDir()
{
	CString mapDir = GetDataFileDir() + "\\worldmap";
	if(!PathIsDirectory(mapDir)) ::CreateDirectory(mapDir,NULL);
	return mapDir;
}

CString WorldEditorConfig::GetTerrainTextureThumbnailDir()
{
	CString thumbnailDir=GetExeDir()+"\\Thumbnail";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	thumbnailDir+="\\Terrain";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	return thumbnailDir;
}

CString WorldEditorConfig::GetGrassTextureThumbnailDir()
{
	CString thumbnailDir=GetExeDir()+"\\Thumbnail";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	thumbnailDir+="\\Grass";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	return thumbnailDir;
}

CString WorldEditorConfig::GetWaterTextureThumbnailDir()
{
	CString thumbnailDir=GetExeDir()+"\\Thumbnail";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	thumbnailDir+="\\Water";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	return thumbnailDir;
}

CString WorldEditorConfig::GetThumbnailDir()
{
	CString thumbnailDir=GetExeDir()+"\\Thumbnail";
	if(!PathIsDirectory(thumbnailDir)) ::CreateDirectory(thumbnailDir,NULL);
	return thumbnailDir;
}

CString WorldEditorConfig::GetTerrainTextureRelativeDir()
{
	CString configPath = GetThumbnailDir()+"\\"+"Thumbnail Config.ini";
	
	char strPath[MAX_PATH];
	GetPrivateProfileString("Config","Relative Terrain Texture Dir","",strPath,MAX_PATH,configPath);
	return strPath;
}

void WorldEditorConfig::SaveTerrainTextureRelativeDir(CString strPath)
{
	CString configPath = GetThumbnailDir()+"\\"+"Thumbnail Config.ini";
	WritePrivateProfileString("Config","Relative Terrain Texture Dir",strPath,configPath);
	WritePrivateProfileString("Config","PathExist","1",configPath);
}

bool WorldEditorConfig::IsTerrainTexturePathExist()
{
	CString configPath = GetThumbnailDir()+"\\"+"Thumbnail Config.ini";
	if(GetPrivateProfileInt("Config","PathExist",0,configPath)==1) return true;
	else return false;
}

CString WorldEditorConfig::GetTerrainTextureDir()
{
	return GetDataFileDir()+"\\"+GetTerrainTextureRelativeDir();
}

CString WorldEditorConfig::GetWorldEditorConfigPath()
{
	return GetExeDir()+CString("\\WorldEditorConfig.ini");
}

void WorldEditorConfig::GetRecentFiles(std::vector<CString>& fileVector)
{
	CString filePath=GetWorldEditorConfigPath();
	for(uint32 i=1;i<=10;++i)
	{
		CString keyName;
		keyName.Format("%d",i);
		TCHAR path[MAX_PATH];
		GetPrivateProfileString("Recent Files",keyName,"",path,MAX_PATH,filePath);
		CString strPath=path;
		if(strPath!="") fileVector.push_back(strPath);
	}
}

void WorldEditorConfig::SaveRecentFiles(CString lastPath)
{
	lastPath=GetRelativePath(lastPath,m_DataPath);

	CString filePath=GetWorldEditorConfigPath();
	std::vector<CString> recentFileVector;
	GetRecentFiles(recentFileVector);
	std::vector<CString>::iterator iter=find(recentFileVector.begin(),recentFileVector.end(),lastPath);
	if(iter!=recentFileVector.end()) recentFileVector.erase(iter);
	recentFileVector.insert(recentFileVector.begin(),lastPath);

	int size=recentFileVector.size();
	for(int i=0;i<10;++i)
	{
		CString keyName;
		keyName.Format("%d",i+1);
		CString tmpName;
		if(i<size) tmpName=recentFileVector[i];
		else tmpName="";
		WritePrivateProfileString("Recent Files",keyName,tmpName,filePath);
	}
}

void WorldEditorConfig::DeleteRecentFiles(CString filePath)
{
	CString iniPath=GetWorldEditorConfigPath();
	std::vector<CString> recentFileVector;
	GetRecentFiles(recentFileVector);
	std::vector<CString>::iterator iter=find(recentFileVector.begin(),recentFileVector.end(),filePath);
	if(iter!=recentFileVector.end()) recentFileVector.erase(iter);

	int size=recentFileVector.size();
	for(int i=0;i<10;++i)
	{
		CString keyName;
		keyName.Format("%d",i+1);
		CString tmpName;
		if(i<size) tmpName=recentFileVector[i];
		else tmpName="";
		WritePrivateProfileString("Recent Files",keyName,tmpName,iniPath);
	}
}