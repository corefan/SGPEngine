#include "stdafx.h"
#include "ModelViewerConfig.h"

ModelViewerConfig* ModelViewerConfig::s_Instance=NULL;

ModelViewerConfig::ModelViewerConfig():\
	m_bShowBoundingBox(FALSE),\
	m_bShowWireframe(FALSE),\
	m_bShowGround(TRUE),\
	m_bShowBones(FALSE),\
	m_bShowNormalLines(FALSE),\
	m_bShowAttachment(FALSE),\
	m_bShowParticleEmitter(FALSE),\
	m_bShowMesh(TRUE),\
	m_nCurrSelParticle(-1)
{
	m_BoundingBoxColor[0]=1.0f;
	m_BoundingBoxColor[1]=1.0f;
	m_BoundingBoxColor[2]=1.0f;
	m_BoundingBoxColor[3]=1.0f;

	m_BkgndColor[0]=0.5f;
	m_BkgndColor[1]=0.5f;
	m_BkgndColor[2]=0.5f;
	m_BkgndColor[3]=1.0f;

	m_DefaultBkgndColor[0]=0.5f;
	m_DefaultBkgndColor[1]=0.5f;
	m_DefaultBkgndColor[2]=0.5f;
	m_DefaultBkgndColor[3]=1.0f;

}


ModelViewerConfig::~ModelViewerConfig()
{
}

ModelViewerConfig* ModelViewerConfig::GetInstance()
{
	if(s_Instance == NULL)
		s_Instance = new ModelViewerConfig;
	return s_Instance;
}

void ModelViewerConfig::ReleaseInstance()
{
	if(s_Instance!= NULL)
	{
		delete s_Instance;
		s_Instance = NULL;
	}
}

void ModelViewerConfig::SetBoxShow(int index,BOOL bShow)
{
	ASSERT(index<(int)m_bBoxShowVector.size());
	m_bBoxShowVector[index]=bShow;
}

CString ModelViewerConfig::GetExeDir()
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

CString ModelViewerConfig::GetDataFileDir()
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