#pragma once

#include "MouseMsghandler.h"

enum HeightBrushType
{
	HeightType_None,
	HeightType_Adjust,
	HeightType_Up,
	HeightType_Down,
	HeightType_Smooth,
	HeightType_Noise,
	HeightType_Flatten,
	HeightType_Set,
	HeightType_ChunkAdjust,
	HeightType_CellAdjust
};

#include <vector>

class CVertexHeightOperation1;
class CVertexHeightOperation2;

class TerrainHeightHandler : public MouseMsgHandler
{
private:
	TerrainHeightHandler(void);
	virtual ~TerrainHeightHandler(void);
	DECLARE_SINGLETON(TerrainHeightHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual bool IsTerrainHandler(){return true;}
private:
	CVertexHeightOperation1* m_pVertexHeightOp1;
	CVertexHeightOperation2* m_pVertexHeightOp2;
	// Brush Attributes
	float m_InnerRadius;
	float m_OuterRadius;
	int m_Intensity;
	HeightBrushType m_BrushType;
	CPoint m_PrevPoint;
	CPoint m_LDownPoint;
	DWORD m_dwStart;
	bool m_bLBtnDown;
	std::vector<int> m_ChunkIdVector;
	std::vector<int> m_CellIdVector;
	bool m_bValidate;
public:
	// Brush Attributes Interface
	void SetInnerRadius(float fRadius){m_InnerRadius=fRadius;}
	void SetOuterRadius(float fRadius){m_OuterRadius=fRadius;}
	void SetIntensity(int intensity){m_Intensity=intensity;}
	float GetInnerRadius(){return m_InnerRadius;}
	float GetOuterRadius(){return m_OuterRadius;}

	void SetBrushType(HeightBrushType type);

	/************ Adjust Vertex Mode ************/
	struct AdjustedPoint
	{
		int index_x;
		int index_z;
		float ratio;
	};
	std::vector<AdjustedPoint> m_AdjustPointVector;
	void RetrieveAdjustVertex();// get all affected vertexes
	void AdjustVertexHeight(int deltaY);// adjust vertex height
	
	void RaiseTerrainHeight();
	void ReduceTerrainHeight();
	void SmoothTerrainHeight();
	void NoiseTerrainHeight();

	/************ Flatten Vertex Mode **********************/
	int m_FlattenHeight;
	void RetrieveFlattenHeight();
	void FlattenTerrainHeight();
	/************ Set Vertex Mode ***************************/
	int m_SettedVertexHeightVal;
	void SetTerrainHeight();
	/************ Adjust Chunk And Cell Mode ****************/
	void AdjustChunkHeight(int height,bool bSet);
	void AdjustCellHeight(int height,bool bSet);
	void AdjustChunkCellHeight(int height,bool bSet);
	void AdjustWorldHeight(int height);

	void ChangeSettedVertexHeightVal(int val);
public://help function
	float squareDistance(float x1,float y1,float x2,float y2);
	virtual void DrawArea();

	void HeightOp1AddChunk(int index_x,int index_z);
	void HeightOp2AddChunk(int index_x,int index_z);
};
