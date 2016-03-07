#pragma once

#include "../SGPLibraryCode/SGPHeader.h"//engine header
#include "ModelViewerCamera.h"
using namespace sgp;
#include "RichEditLogger.h"

/****************************************************************
this class is used for render
it wrap all functions related with render,creates a bridge between
ui and render engine.In this way,we can reduce the coupling.
****************************************************************/
class ModelViewerRender
{
	friend class ModelViewerRenderInterface;
	ModelViewerRender(void);
	~ModelViewerRender(void);
private:
	//Init:this function should before rendering
	bool Init(HWND hwnd,CRichEditCtrl *pCtrl);
	void InitGround();//surface
	//release resources
	void Release();
	//rendering
	void Render();
	void RenderGround();
	void RenderCoordinateAxis();
	void RenderCoordinateLabel();
	void ChangeModel(wchar_t *modelPath,BOOL bDynamic=TRUE);
	void CloseModel();
	void CalFPS(double deltaTime);//calculate fps
	void RenderFPS();
	void RenderBoundingBox(AABBox& box);
	void RenderBones();//render bones
	void RenderDynamicNormals();//render normals for dynamic model
	void RenderStaticNormals();// render normals for static model
	void RenderParticleEmitter();
	void RenderAttachments();
private:
	void ResizeWindow(int cx,int cy);//resize the render window
	//init SGPVertex_UPOS_VERTEXCOLOR
	inline void SetVertexPos(SGPVertex_UPOS_VERTEXCOLOR &vertex,float x,float y,float z);
	inline void SetVertexColor(SGPVertex_UPOS_VERTEXCOLOR &vertex,float r=1.0f,float g=1.0f,float b=1.0f,float a=1.0f);
	void CalTranlsatedVertex(SGPVertex_UPOS_VERTEXCOLOR &vertex,Matrix4x4* pBoneMatrix,int meshIndex,int vertexIndex,BOOL bOrigin);
private:
	SGPDevice* m_pDevice;
	ISGPRenderDevice* m_pRenderDevice;
	RichEditLogger* m_pLogger;
	ModelViewerCamera* m_pCamera;
	CSkeletonMeshInstance* m_pDynamicModel;
	CStaticMeshInstance* m_pStaticModel;
	CMF1FileResource* m_pMF1FileResource;
	CSGPModelMF1* m_pModelMF1;
private:
	bool m_bInitilized;
private:
	unsigned int m_iModelID;//model id
	SGPVertex_UPOS_VERTEXCOLOR m_SurfaceVertex[108];//surface vertex array
	SGPVertex_UPOS_VERTEXCOLOR m_AxisVertex[6];//axia vertex array
	SGPVertex_UPOS_VERTEXCOLOR m_BoundingBoxVertex[16];//boundingbox vertex array
	double m_FPS;//fps
	String m_ModelPath;
};
