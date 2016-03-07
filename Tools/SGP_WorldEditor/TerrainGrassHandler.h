#pragma once

#include "MouseMsgHandler.h"

enum GrassBrushType
{
	GrassType_None,
	GrassType_Add,
	GrassType_Delete
};

class TerrainGrassOperation;

class TerrainGrassHandler : public MouseMsgHandler
{
	friend class CTerrainGrassDlg;
private:
	TerrainGrassHandler();
	~TerrainGrassHandler();
	DECLARE_SINGLETON(TerrainGrassHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea();
	virtual bool IsTerrainHandler(){return true;}
private:
	GrassBrushType m_BrushType;
	float m_BrushRadius;
	int m_GrassMinSize;
	int m_GrassMaxSize;
	int m_GrassDensity;
	int m_WindOffsetX;
	int m_WindOffsetZ;
	int m_nTiledTexIndex;
	bool m_bLbtnDown;
	TerrainGrassOperation* m_pGrassOp;
	CString m_GrassTexPath;
	int m_TexTileX;
	int m_TexTileY;
public:
	void SetBrushType(GrassBrushType type){m_BrushType=type;}
	void SetGrassMinSize(int size){m_GrassMinSize=size;}
	void SetGrassMaxSize(int size){m_GrassMaxSize=size;}
	void SetGrassDensity(int density){m_GrassDensity=density;}
	void SetWindOffsetX(int offsetX){m_WindOffsetX=offsetX;}
	void SetWindOffsetZ(int offsetZ){m_WindOffsetZ=offsetZ;}
	void SetBrushSize(float radius){m_BrushRadius=radius;}
	void SetTiledTexIndex(int index){m_nTiledTexIndex=index;}
	void SetGrassInfo(CString texPath,int TexAtlasX,int TexAtlasY);

	void AddGrassAction();
	void DeleteGrassAction();
	BOOL IsReady();
};