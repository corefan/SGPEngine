#pragma once

namespace sgp
{
	class CSGPTextureManager;
	class CSGPWorldMap;
	class ISGPWorldSystemManager;
	class CSGPGrass;
	class CSGPWater;
	class CSGPWorldMap;
	class CSGPSkyDome;
	class CSGPWorldSun;
	class COpenGLWaterRenderer;
	class COpenGLGrassRenderer;
	class CSGPTerrainChunk;
	class CSGPTerrain;
	class ISGPRenderDevice;
	class SGPDevice;
	struct ISGPObject;
	class OBBox;
	class AABBox;
	class Vector3D;
}

struct CommonObject;

class EditorExis;
class WorldEditorRender;

class WorldEditorRenderInterface
{
	friend class WorldEditorRender;
	WorldEditorRenderInterface();
	~WorldEditorRenderInterface();
	DECLARE_SINGLETON(WorldEditorRenderInterface)
public:
	//Init:this function should be called before rendering
	bool Init(HWND hwnd,CRichEditCtrl *pCtrl){return m_pRender->Init(hwnd,pCtrl);}
	//release resources
	void Release(){return m_pRender->Release();}
	//rendering
	void Render(){return m_pRender->Render();}
	// Whether Initialized
	bool IsInitialized(){return m_pRender->m_bInitilized;}
public:// camera related interface
	void ResizeWindow(int cx,int cy){return m_pRender->ResizeWindow(cx,cy);}
	void MoveCamera(float cx,float cy){m_pRender->m_pCamera->Move(cx,cy);}
	void RotateCamera(float cx,float cy){m_pRender->m_pCamera->Rotate(cx,cy);}
	void ZoomCamera(float fZoom){m_pRender->m_pCamera->Zoom(fZoom);}
	void SetCurPos(int x,int y);
	void FocusObject(const CommonObject& obj);
	void SetCameraFov(float fFov);
	void SetCameraNearPlane(float fNear);
	void SetCameraFarPlane(float fFar);
	void SetRotateCenter();
public:
	// render flag
	void RenderRoundIntersectArea(float radius,float r,float g,float b){m_pRender->RenderRoundIntersectArea(radius,r,g,b);}
	void RenderSquareIntersectArea(float radius,float r,float g,float b){m_pRender->RenderSquareIntersectArea(radius,r,g,b);}
	void RenderSelectedChunk(int chunkId,float r,float g,float b){m_pRender->RenderSelectedChunk(chunkId,r,g,b);}
	void RenderSelectedChunk(int chunkId,float height,float r,float g,float b){m_pRender->RenderSelectedChunk(chunkId,height,r,g,b);}
	void RenderSelectedCell(int cellId,int r,int g,int b){m_pRender->RenderSelectedCell(cellId,r,g,b);}
	void RenderRotateCenter();
	void RenderOBBox(const sgp::OBBox& obbox,int r,int g,int b);
	void RenderAABBox(const sgp::AABBox& aabbox,int r,int g,int b);
	void RenderAxis(EditorExis& xAxis,EditorExis& yAxis,EditorExis& zAxis,EditorExis* selectedAxis);
	void RenderBoxSelectArea(int left,int top,int right,int bottom,float r,float g,float b);
	void RenderLine(const sgp::Vector3D& ptStart,const sgp::Vector3D& ptEnd,float r,float g,float b);
public:
	sgp::ISGPWorldSystemManager* GetWorldSystemManager();
	sgp::CSGPWorldMap* GetWorldMap();
	sgp::ISGPRenderDevice* GetRenderDevice();
	sgp::SGPDevice* GetDevice();
public:
	int GetTerrainHeight(int index_x,int index_z);
	float GetTerrainHeight(float pos_x,float pos_z);
	void GetIntersectPoint(float &x,float &z);
	float GetIntersectHeight();
	int GetChunkSize();

	bool IsIntersectTerrain();
	int GetCurrPosChunkId();
	int GetCurrPosCellId();
	bool GetCurrPosObject(int pos_x,int pos_y,CommonObject& obj);
	void GetRenderWndSize(int& width,int& height);
public:
	WorldEditorRender* m_pRender;
	float m_RotateX,m_RotateY,m_RotateZ;
};