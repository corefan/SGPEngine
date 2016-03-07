#pragma once

#include "MouseMsghandler.h"
#include <set>
#include <vector>

enum TextureBrushType
{
	TextureType_None,
	TextureType_Add,
	TextureType_Delete,
	TextureType_ChunkAdjust
};

class TerrainTextureOperation;
class TerrainTextureOperation2;
class ChunkTextureOperation;
class CTerrainTextureDlg;

class TerrainTextureHandler : public MouseMsgHandler
{
private:
	TerrainTextureHandler();
	~TerrainTextureHandler();
	DECLARE_SINGLETON(TerrainTextureHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea();
	virtual bool IsTerrainHandler(){return true;}
public:
	void SetInnerRadius(float radius){m_InnerRadius=radius;}
	void SetOuterRadius(float radius){m_OuterRadius=radius;}
	void SetTextureAlpha(int alpha){m_TexAlpha=alpha;}
	void SetCurrentLayer(int nLayer){m_nCurrLayer=nLayer;}
	void SetBrushType(TextureBrushType type);
	void SetLayerImgPath(int index,CString path){m_LayerImgPath[index]=path;}
	void SetChunkTexture(int nLayer,CString texturePath);
	bool CanSetTexture(){return m_BrushType==TextureType_ChunkAdjust&&m_ChunkIdVector.size()!=0;}
	void UpdateSelectedChunkInfo();
	bool IsReady();// whether can texture
	void RetrieveRelatedChunk(CString texPath);
private:
	float m_InnerRadius;
	float m_OuterRadius;
	int m_TexAlpha;
	DWORD m_dwStart;
	int m_nCurrLayer;// Current Layer:0,1,2,3
	TextureBrushType m_BrushType;//BrushType:Add,Delete,ChunkAdjust
	CString m_LayerImgPath[4];
	std::set<int> m_ChunkSet;
	std::vector<int> m_ChunkIdVector;//selected chunk id
	bool m_bLBtnDown;
	TerrainTextureOperation* m_pTerrainTexOp;
	TerrainTextureOperation2* m_pTerrainTexOp2;
	ChunkTextureOperation* m_pChunkTexOp;
	std::vector<int> m_RelatedChunkVector;
private:
	void AddTextureAction();
	void DeleteTextureAction();
};