#pragma once

#include "MouseMsgHandler.h"

class TerrainWaterOperation;

enum WaterBrushType
{
	WaterType_None,
	WaterType_Add,
	WaterType_Delete
};

class TerrainWaterHandler : public MouseMsgHandler
{
	friend class CTerrainWaterDlg;
private:
	TerrainWaterHandler();
	~TerrainWaterHandler();
	DECLARE_SINGLETON(TerrainWaterHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea();
	virtual bool IsTerrainHandler(){return true;}
private:
	WaterBrushType m_BrushType;
	TerrainWaterOperation* m_pWaterOp;
	CString m_WaterTexPath;
	float m_WaterHeight;
	bool m_bLbtnDown;
public:
	void SetBrushType(WaterBrushType brushType){m_BrushType=brushType;}
	void SetWaterAttr(CString texPath,float height);
	bool IsNewWaterChunk(int chunkId);
	void CreateWaterIfNeed();
	void AddWater();
	void DeleteWater();
};