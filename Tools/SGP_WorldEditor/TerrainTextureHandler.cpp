#include "stdafx.h"
#include "WorldEditor.h"
#include "TerrainTextureHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <math.h>

#include "TerrainTextureOperation.h"
#include "OperationManager.h"
#include "TerrainTextureDlg.h"
#include "WorldMapManager.h"
#include "WorldMapManager.h"
#include "MouseMsgInterface.h"

IMPLEMENT_SINGLETON(TerrainTextureHandler)

TerrainTextureHandler::TerrainTextureHandler()
{
	m_OuterRadius=8.0f;
	m_nCurrLayer=-1;
	m_BrushType=TextureType_None;
	m_bLBtnDown = false;
	m_pTerrainTexOp = NULL;
	m_pChunkTexOp = NULL;
}

TerrainTextureHandler::~TerrainTextureHandler()
{
}

void TerrainTextureHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	m_RelatedChunkVector.clear();
	if(!IsReady()) return ;

	switch(m_BrushType)
	{
	case TextureType_Add:
	case TextureType_Delete:
		switch(m_nCurrLayer)
		{
		case 0:
			{
				m_pChunkTexOp = new ChunkTextureOperation;
				int chunkId=WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
				if(chunkId!=-1)
				{
					m_pChunkTexOp->SetChunkTexture(chunkId,0,m_LayerImgPath[0]);
					m_pChunkTexOp->AddChunk(chunkId);
				}
			}
			break;
		case 1:
		case 2:
		case 3:
			m_pTerrainTexOp = new TerrainTextureOperation;
			m_pChunkTexOp = new ChunkTextureOperation;
			m_pTerrainTexOp->SetLayer(m_nCurrLayer);
			if(m_BrushType==TextureType_Add) AddTextureAction();
			if(m_BrushType==TextureType_Delete) DeleteTextureAction();
			break;
		}
		break;
	case TextureType_ChunkAdjust:
		{
			int chunkId=WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
			if(nFlags&MK_CONTROL)
			{
				if(chunkId!=-1)
				{
					int size=m_ChunkIdVector.size();
					bool bFinded=false;
					for(int i=0;i<size;++i)
					{
						if(m_ChunkIdVector[i]==chunkId)
						{
							bFinded=true;
							std::vector<int>::iterator iter=m_ChunkIdVector.begin()+i;
							m_ChunkIdVector.erase(iter);
							break;
						}
					}
					if(!bFinded) m_ChunkIdVector.push_back(chunkId);
				}
			}
			else
			{
				m_ChunkIdVector.clear();
				if(chunkId!=-1) m_ChunkIdVector.push_back(chunkId);
			}
			UpdateSelectedChunkInfo();
		}
		break;
	}
	m_bLBtnDown = true;
}

void TerrainTextureHandler::LButtonUp(CPoint pt,UINT nFlags)
{
	if(!IsReady()) return ;

	switch(m_BrushType)
	{
	case TextureType_Add:
	case TextureType_Delete:
		switch(m_nCurrLayer)
		{
		case 0:
			if(m_pChunkTexOp != NULL)
			{
				if(m_pChunkTexOp->IsValidate())
				{
					m_pChunkTexOp->FlushColorMiniMap();
					OperationManager::GetInstance()->AddNewOperation(m_pChunkTexOp);
				}
				else delete m_pChunkTexOp;
			}
			m_pChunkTexOp = NULL;
			break;
		case 1:
		case 2:
		case 3:
			if(m_pChunkTexOp != NULL&&m_pTerrainTexOp != NULL)
			{
				if(m_pTerrainTexOp->IsValidate())
				{
					if(m_pChunkTexOp->IsValidate())
						m_pTerrainTexOp->SetChunkTexOperation(m_pChunkTexOp);
					else delete m_pChunkTexOp;
					if(m_BrushType==TextureType_Delete) m_pTerrainTexOp->CheckOpAfterDelTex();
					m_pTerrainTexOp->FlushColorMiniMap();
					OperationManager::GetInstance()->AddNewOperation(m_pTerrainTexOp);
				}
				else
				{
					delete m_pTerrainTexOp;
					delete m_pChunkTexOp;
				}
			}
			m_pChunkTexOp = NULL;
			m_pTerrainTexOp = NULL;
			break;
		}
		break;
	default:
		break;
	}
	m_ChunkSet.clear();
	m_bLBtnDown = false;
}

void TerrainTextureHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if(!IsReady()) return ;

	if((nFlags&MK_LBUTTON)&&m_bLBtnDown)
	{
		switch(m_BrushType)
		{
		case TextureType_Add:
		case TextureType_Delete:
			switch(m_nCurrLayer)
			{
			case 0:
				{
					int chunkId = WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
					if(chunkId!=-1)
					{
						m_pChunkTexOp->SetChunkTexture(chunkId,0,m_LayerImgPath[0]);
						m_pChunkTexOp->AddChunk(chunkId);
					}
				}
				break;
			case 1:
			case 2:
			case 3:
				if(m_BrushType==TextureType_Add) AddTextureAction();
				if(m_BrushType==TextureType_Delete) DeleteTextureAction();
				break;
			}
			break;
		default:
			break;
		}
	}
}

void TerrainTextureHandler::DrawArea()
{
	WorldEditorRenderInterface* pInterface=WorldEditorRenderInterface::GetInstance();
	int size = m_RelatedChunkVector.size();
	for(int i=0;i<size;++i)
		pInterface->RenderSelectedChunk(m_RelatedChunkVector[i],1.0f,0.0f,0.0f);
	
	if(!IsReady()) return;
	
	int chunkId;
	switch(m_BrushType)
	{
	case TextureType_Add:
	case TextureType_Delete:
		switch(m_nCurrLayer)
		{
		case 0:	
			chunkId = pInterface->GetCurrPosChunkId();
			if(chunkId!=-1) pInterface->RenderSelectedChunk(chunkId,0.0f,1.0f,1.0f);
			break;
		case 1:
		case 2:
		case 3:
			if(!CTerrainTextureDlg::GetInstance()->IsLayerClosed(m_nCurrLayer))
			{
				if(pInterface->IsIntersectTerrain())
				{
					if(m_BrushType==TextureType_Add)
						pInterface->RenderRoundIntersectArea(m_InnerRadius,0.0f,1.0f,0.0f);
					pInterface->RenderRoundIntersectArea(m_OuterRadius,1.0f,0.0f,0.0f);
				}	
			}
			break;
		}
		break;
	case TextureType_ChunkAdjust:
		{
			int size=m_ChunkIdVector.size();
			for(int i=0;i<size;++i) pInterface->RenderSelectedChunk(m_ChunkIdVector[i],1.0f,1.0f,1.0f);
			if(MouseMsgInterface::GetInstance()->CursorInRenderWindow())
			{
				chunkId = pInterface->GetCurrPosChunkId();
				if(chunkId!=-1) pInterface->RenderSelectedChunk(chunkId,0.0f,1.0f,1.0f);
			}
		}
		break;
	}
}

void TerrainTextureHandler::AddTextureAction()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	if(!pRenderInterface->IsIntersectTerrain()) return ;

	int offset=(3-m_nCurrLayer)*8;
	uint32 mask=(0xFF<<offset);

	int chunkSize=pRenderInterface->GetChunkSize();
	uint32 chunkCount=chunkSize*chunkSize;
	int alphaTextureSize = chunkSize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
	int mapSize=chunkSize*SGPTT_TILENUM*SGPTT_TILE_METER;
	float ratio = (float)alphaTextureSize/mapSize;
	float ratio2 = (float)mapSize/alphaTextureSize;

	float center_x,center_z;
	pRenderInterface->GetIntersectPoint(center_x,center_z);
	float center_x1=center_x*ratio;
	float center_z1=center_z*ratio;

	float innerRadius,outerRadius;
	innerRadius=m_InnerRadius*ratio;
	outerRadius=m_OuterRadius*ratio;
	int min_intx,max_intx,min_intz,max_intz;
	min_intx = jlimit(0,alphaTextureSize-1,(int)(center_x1 - outerRadius -1));
	max_intx = jlimit(0,alphaTextureSize-1,(int)(center_x1 + outerRadius +1));
	min_intz = jlimit(0,alphaTextureSize-1,(int)(center_z1 - outerRadius -1));
	max_intz = jlimit(0,alphaTextureSize-1,(int)(center_z1 + outerRadius +1));

	uint32* pAlphaData=pRenderInterface->GetWorldMap()->m_WorldChunkAlphaTextureData;

	for(int i=min_intx;i<=max_intx;++i)
	{
		for(int j=min_intz;j<=max_intz;++j)
		{
			int index = (alphaTextureSize -1 - j) * alphaTextureSize + i;
			float distance = sqrt((float)(((float)i-center_x1)*((float)i-center_x1)+((float)j-center_z1)*((float)j-center_z1)));
			if(distance <= innerRadius)// Inner Circle
			{
				TerrainTextureOpData opData;
				opData.index=index;
				int prevAlpha,currAlpha;
				if(m_pTerrainTexOp->FindData(opData,prevAlpha,currAlpha))// Finded
				{
					opData.prevAlpha=prevAlpha;
					int alpha=jlimit(0,255,opData.prevAlpha+m_TexAlpha);
					if(alpha>currAlpha) currAlpha=alpha;
				}
				else
				{
					prevAlpha=(pAlphaData[index]&mask)>>offset;
					currAlpha=jlimit(0,255,prevAlpha+m_TexAlpha);
				}
				opData.prevAlpha=prevAlpha;
				opData.currAlpha=currAlpha;
				if(opData.prevAlpha!=opData.currAlpha)
				{
					// Set Chunk Texture
					uint32 chunkId = ((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex((float)i*ratio2,\
						(float)j*ratio2);
					if(chunkId!=0xFFFFFFFF&&chunkId<chunkCount)
					{
						if(!m_pChunkTexOp->IsContainChunk((int)chunkId))
						{
							m_pChunkTexOp->SetChunkTexture((int)chunkId,m_nCurrLayer,m_LayerImgPath[m_nCurrLayer]);
							m_pTerrainTexOp->AddChunk((int)chunkId);
						}
						
					}
					// Set AlphaTexture Data
					pAlphaData[index] = (pAlphaData[index]|mask)&((~mask)|(opData.currAlpha<<offset));

					m_pTerrainTexOp->AddData(opData);
				}
			}
			else if(distance<=outerRadius)// Between InnerCircle And OuterCircle
			{
				TerrainTextureOpData opData;
				opData.index=index;
				int prevAlpha,currAlpha;
				if(m_pTerrainTexOp->FindData(opData,prevAlpha,currAlpha))// Finded
				{
					opData.prevAlpha=prevAlpha;
					int alpha = opData.prevAlpha+(int(m_TexAlpha*(outerRadius-distance)/(outerRadius-innerRadius)));
					alpha = jlimit(0,255,alpha);
					if(alpha>currAlpha) currAlpha=alpha;
				}
				else
				{
					prevAlpha=(pAlphaData[index]&mask)>>offset;
					currAlpha=prevAlpha+(int(m_TexAlpha*(outerRadius-distance)/(outerRadius-innerRadius)));
					currAlpha = jlimit(0,255,currAlpha);
				}
				opData.prevAlpha=prevAlpha;
				opData.currAlpha=currAlpha;
				if(opData.prevAlpha!=opData.currAlpha)
				{	
					// Set Chunk Texture
					uint32 chunkId = ((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex((float)i*ratio2,(float)j*ratio2);
					if(chunkId!=0xFFFFFFFF&&chunkId<chunkCount)
					{
						if(!m_pChunkTexOp->IsContainChunk((int)chunkId))
						{
							m_pChunkTexOp->SetChunkTexture((int)chunkId,m_nCurrLayer,m_LayerImgPath[m_nCurrLayer]);
							m_pTerrainTexOp->AddChunk((int)chunkId);
						}
						// Set AlphaTexture Data
						pAlphaData[index] = (pAlphaData[index]|mask)&((~mask)|(opData.currAlpha<<offset));
						m_pTerrainTexOp->AddData(opData);
					}
				}
			}
		}
	}
	m_pTerrainTexOp->ContainRect(min_intx,max_intx,min_intz,max_intz);
	WorldMapManager::GetInstance()->EditTerrainChunkBlendTexture(min_intx,alphaTextureSize-1-max_intz,max_intx-min_intx+1,max_intz-min_intz+1);
}

void TerrainTextureHandler::DeleteTextureAction()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	if(!pRenderInterface->IsIntersectTerrain()) return ;

	int offset=(3-m_nCurrLayer)*8;
	uint32 mask=(0xFF<<offset);

	int chunkSize=pRenderInterface->GetChunkSize();
	int alphaTextureSize=chunkSize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
	int mapSize = chunkSize * SGPTT_TILENUM * SGPTT_TILE_METER;
	float ratio = (float)alphaTextureSize/mapSize;
	float ratio2 = (float)mapSize/alphaTextureSize;

	float center_x,center_z;
	pRenderInterface->GetIntersectPoint(center_x,center_z);
	float center_x1=center_x*ratio;
	float center_z1=center_z*ratio;

	float innerRadius=m_InnerRadius*ratio;
	float outerRadius=m_OuterRadius*ratio;
	int min_intx,max_intx,min_intz,max_intz;
	min_intx = jlimit(0,alphaTextureSize-1,(int)(center_x1 - outerRadius -1));
	max_intx = jlimit(0,alphaTextureSize-1,(int)(center_x1 + outerRadius +1));
	min_intz = jlimit(0,alphaTextureSize-1,(int)(center_z1 - outerRadius -1));
	max_intz = jlimit(0,alphaTextureSize-1,(int)(center_z1 + outerRadius +1));

	uint32* pAlphaData=pRenderInterface->GetWorldMap()->m_WorldChunkAlphaTextureData;

	for(int i=min_intx;i<=max_intx;++i)
	{
		for(int j=min_intz;j<=max_intz;++j)
		{
			int index=(alphaTextureSize-1-j)*alphaTextureSize+i;
			float distance = sqrt((float)(((float)i-center_x1)*((float)i-center_x1)+((float)j-center_z1)*((float)j-center_z1)));
			if(distance <= outerRadius)// Inner Circle
			{
				TerrainTextureOpData opData;
				opData.index=index;
				int prevAlpha,currAlpha;
				if(m_pTerrainTexOp->FindData(opData,prevAlpha,currAlpha))// Finded
				{
					opData.prevAlpha=prevAlpha;
					int alpha = jlimit(0,255,opData.prevAlpha-m_TexAlpha);
					if(alpha<currAlpha) currAlpha=alpha;
				}
				else
				{
					prevAlpha=(pAlphaData[index]&mask)>>offset;
					currAlpha = jlimit(0,255,prevAlpha-m_TexAlpha);
				}
				opData.prevAlpha=prevAlpha;
				opData.currAlpha=currAlpha;
				if(opData.prevAlpha!=opData.currAlpha)
				{
					// Set Chunk Texture
					uint32 chunkId = ((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex((float)i*ratio2,(float)j*ratio2);
					if(chunkId!=0xFFFFFFFF)
					{
						m_pTerrainTexOp->AddChunk((int)chunkId);
						// Set AlphaTexture Data
						pAlphaData[index] = (pAlphaData[index]|mask)&((~mask)|(opData.currAlpha<<offset));
						m_pTerrainTexOp->AddData(opData);
					}				
				}
			}
		}
	}
	m_pTerrainTexOp->ContainRect(min_intx,max_intx,min_intz,max_intz);
	WorldMapManager::GetInstance()->EditTerrainChunkBlendTexture(min_intx,alphaTextureSize-1-max_intz,max_intx-min_intx+1,max_intz-min_intz+1);
/*	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	if(!pRenderInterface->IsIntersectTerrain()) return ;

	int offset=(3-m_nCurrLayer)*8;
	uint32 mask=(0xFF<<offset);

	int chunkCount=pRenderInterface->GetChunkSize();
	int mapSize = chunkCount * SGPTT_TILENUM * SGPTT_TILE_METER;
	float center_x,center_z;
	pRenderInterface->GetIntersectPoint(center_x,center_z);

	int min_intx,max_intx,min_intz,max_intz;
	min_intx = jlimit(0,mapSize,(int)(center_x - m_OuterRadius -1));
	max_intx = jlimit(0,mapSize,(int)(center_x + m_OuterRadius +1));
	min_intz = jlimit(0,mapSize,(int)(center_z - m_OuterRadius -1));
	max_intz = jlimit(0,mapSize,(int)(center_z + m_OuterRadius +1));

	uint32* pAlphaData=pRenderInterface->GetWorldMap()->m_WorldChunkAlphaTextureData;

	for(int i=min_intx;i<=max_intx;++i)
	{
		for(int j=min_intz;j<=max_intz;++j)
		{
			int index=(mapSize-j)*mapSize+i;
			float distance = sqrt((float)(((float)i-center_x)*((float)i-center_x)+((float)j-center_z)*((float)j-center_z)));
			if(distance <= m_OuterRadius)// Inner Circle
			{
				TerrainTextureOpData opData;
				opData.index=index;
				int prevAlpha,currAlpha;
				if(m_pTerrainTexOp->FindData(opData,prevAlpha,currAlpha))// Finded
				{
					opData.prevAlpha=prevAlpha;
					int alpha = jlimit(0,255,opData.prevAlpha-m_TexAlpha);
					if(alpha<currAlpha) currAlpha=alpha;
				}
				else
				{
					prevAlpha=(pAlphaData[index]&mask)>>offset;
					currAlpha = jlimit(0,255,prevAlpha-m_TexAlpha);
				}
				opData.prevAlpha=prevAlpha;
				opData.currAlpha=currAlpha;
				if(opData.prevAlpha!=opData.currAlpha)
				{
					// Set Chunk Texture
					uint32 chunkId = ((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex((float)i,(float)j);
					if(chunkId!=0xFFFFFFFF)
					{
						m_pTerrainTexOp->AddChunk((int)chunkId);
						// Set AlphaTexture Data
						pAlphaData[index] = (pAlphaData[index]|mask)&((~mask)|(opData.currAlpha<<offset));
						m_pTerrainTexOp->AddData(opData);
					}				
				}
			}
		}
	}
	m_pTerrainTexOp->ContainRect(min_intx,max_intx,min_intz,max_intz);
	WorldMapManager::GetInstance()->EditTerrainChunkBlendTexture(min_intx,mapSize-max_intz,max_intx-min_intx,max_intz-min_intz);*/
}

void TerrainTextureHandler::SetChunkTexture(int nLayer,CString texturePath)
{
	m_pChunkTexOp = new ChunkTextureOperation;
	int size=m_ChunkIdVector.size();
	for(int i=0; i<size; ++i)
	{
		m_pChunkTexOp->SetChunkTexture(m_ChunkIdVector[i],nLayer,texturePath);
		m_pChunkTexOp->AddChunk(m_ChunkIdVector[i]);
	}
	
	if(m_pChunkTexOp->IsValidate())
	{
		m_pChunkTexOp->FlushColorMiniMap();
		OperationManager::GetInstance()->AddNewOperation(m_pChunkTexOp);
	}
	else delete m_pChunkTexOp;

	UpdateSelectedChunkInfo();
}

void TerrainTextureHandler::SetBrushType(TextureBrushType type)
{
	m_BrushType = type;
	if(m_BrushType != TextureType_ChunkAdjust) m_ChunkIdVector.clear();
	UpdateSelectedChunkInfo();
}

void TerrainTextureHandler::UpdateSelectedChunkInfo()
{
	CTerrainTextureDlg::GetInstance()->UpdateSelectedChunkInfo(m_ChunkIdVector);
}

bool TerrainTextureHandler::IsReady()
{
	switch(m_BrushType)
	{
	case TextureType_None:
		return false;
	case TextureType_ChunkAdjust:
		return true;
	case TextureType_Add:
		if(m_nCurrLayer>=0&&m_nCurrLayer<=3) return !CTerrainTextureDlg::GetInstance()->IsLayerClosed(m_nCurrLayer);
		else return false;
	case TextureType_Delete:
		if(m_nCurrLayer>=1&&m_nCurrLayer<=3) return !CTerrainTextureDlg::GetInstance()->IsLayerClosed(m_nCurrLayer);
		else return false;
	}
	return false;
}

void TerrainTextureHandler::RetrieveRelatedChunk(CString texPath)
{
	m_RelatedChunkVector.clear();
	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int chunkCount = chunkSize * chunkSize;
	for(int i=0;i<chunkCount;++i)
	{
		for(int nLayer=0;nLayer<7;++nLayer)
		{
			if(texPath == WorldMapManager::GetInstance()->GetChunkTexture(i,nLayer))
			{
				m_RelatedChunkVector.push_back(i);
				break;
			}
		}	
	}
}