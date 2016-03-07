#pragma once

class RichEditLogger;
class WorldEditorCamera;

#include "..\SGPLibraryCode\SGPHeader.h"
using namespace sgp;

class WorldEditorRender
{
	friend class WorldEditorRenderInterface;
	WorldEditorRender();
	~WorldEditorRender();
public:
	// Init:this function should be called before rendering
	bool Init(HWND hwnd,CRichEditCtrl *pCtrl);
	// Release resources
	void Release();
public:/** Render related interface **/
	// Render
	void Render();
	// Render Chunk Grid Line
	void RenderChunkGridLine();
	// Render Chunk Line
	void RenderChunkLine();
	// Render FPS
	void RenderFPS(double deltaTime);
	// Render Ground
	void RenderGround();
	// Render Lights
	void RenderLights();
	// Render Intersection
	bool GetWorldIntersectPoint();
	void RenderRoundIntersectArea(float radius,float r,float g,float b);
	void RenderSquareIntersectArea(float radius,float r,float g,float b);

	void RenderSelectedChunk(int chunkId,float r,float g,float b);
	void RenderSelectedChunk(int chunkId,float height,float r,float g,float b);
	void RenderSelectedCell(int cellId,int r,int g,int b);

	void RenderCoordinateAxis();
	void RenderCoordinateLabel();

	void RenderBoxSelectArea(int left,int top,int right,int bottom,float r,float g,float b);
public:
	// Resize Window
	void ResizeWindow(int cx,int cy);
	void SetCurPos(int x,int y){m_CurPosX=x;m_CurPosY=y;}
	void SetVertexPos(SGPVertex_UPOS_VERTEXCOLOR& vertex,float x,float y,float z);
	void SetVertexColor(SGPVertex_UPOS_VERTEXCOLOR& vertex,float r,float g,float b,float a=1.0f);
	uint16 GetTerrainHeight(uint32 index_x,uint32 index_z);
private:
	RichEditLogger* m_pLogger;
	SGPDevice* m_pDevice;
	ISGPRenderDevice* m_pRenderDevice;
	WorldEditorCamera* m_pCamera;
	bool m_bInitilized;
	int m_CurPosX;
	int m_CurPosY;
	Vector3D m_IntersectPoint;
	SGPVertex_UPOS_VERTEXCOLOR m_AxisVertex[6];//axia vertex array
	CSGPWorldMap* m_pWorldMap;
	bool m_bIntersectTerrain;
//	bool m_bHaveWorld;
	int m_WndWidth,m_WndHeight;
};