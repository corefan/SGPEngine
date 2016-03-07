#pragma once

#include <vector>
//ModelViewerConfig is a class to config parameters in ModelViewer
class ModelViewerConfig
{
	ModelViewerConfig();
	~ModelViewerConfig();
	static ModelViewerConfig* s_Instance;
public:
	static ModelViewerConfig* GetInstance();
	static void ReleaseInstance();
public:
	float m_BoundingBoxColor[4];			//box color
	float m_BkgndColor[4];					//background color

	float m_DefaultBkgndColor[4];			// default background color
	BOOL m_bShowBoundingBox;				// whether show bounding box
	BOOL m_bShowWireframe;					// whether show wire frame
	BOOL m_bShowGround;						// whether show ground
	BOOL m_bShowAttachment;					// whether show auxiliary
	BOOL m_bShowNormalLines;				// whether show normal lines
	BOOL m_bShowBones;						// whether show bones
	BOOL m_bShowMesh;						// whether show meshes
	BOOL m_bShowParticleEmitter;			// whether show particle emitter

	std::vector<BOOL> m_bBoxShowVector;
	void SetBoxShow(int index,BOOL bShow);

	int m_nCurrSelParticle;
	CString m_InitModelPath;

	CString m_ExePath;
	CString m_DataPath;

	CString GetExeDir();
	CString GetDataFileDir();
};
