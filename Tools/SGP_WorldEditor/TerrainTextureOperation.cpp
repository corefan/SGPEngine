#include "stdafx.h"

#include "..\SGPLibraryCode\SGPHeader.h"
#include "TerrainTextureOperation.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "WorldMapManager.h"
#include "TerrainTextureHandler.h"

/*******************************************************************
	class ChunkTextureOperation Implement
*******************************************************************/
ChunkTextureOperation::ChunkTextureOperation()
{

}

ChunkTextureOperation::~ChunkTextureOperation()
{

}

void ChunkTextureOperation::undo()
{
	std::set<ChunkTextureOpData>::iterator iter;
	for(iter=m_DataSet.begin(); iter!=m_DataSet.end(); ++iter)
		WorldMapManager::GetInstance()->SetChunkTexture(iter->chunkId,iter->nLayer,iter->prevTexPath);
	FlushColorMiniMap();
	TerrainTextureHandler::GetInstance()->UpdateSelectedChunkInfo();
}

void ChunkTextureOperation::redo()
{
	std::set<ChunkTextureOpData>::iterator iter;
	for(iter=m_DataSet.begin(); iter!=m_DataSet.end(); ++iter)
		WorldMapManager::GetInstance()->SetChunkTexture(iter->chunkId,iter->nLayer,iter->currTexPath);
	FlushColorMiniMap();
	TerrainTextureHandler::GetInstance()->UpdateSelectedChunkInfo();
}

void ChunkTextureOperation::SetChunkTexture(int chunkId,int nLayer,CString texPath)
{
	std::set<ChunkTextureOpData>::iterator iter;
	ChunkTextureOpData opData;
	opData.chunkId = chunkId;
	opData.nLayer = nLayer;
	iter = m_DataSet.find(opData);
	if(iter != m_DataSet.end())// existed before
		return ;
	else
		opData.prevTexPath = WorldMapManager::GetInstance()->GetChunkTexture(chunkId,nLayer);
	opData.currTexPath = texPath;
	if(opData.prevTexPath != opData.currTexPath)
	{
		m_DataSet.insert(opData);
		WorldMapManager::GetInstance()->SetChunkTexture(chunkId,nLayer,texPath);
	}
}

void ChunkTextureOperation::FlushColorMiniMap()
{
	uint32 size=m_ChunkIdSet.size();
	if(size!=0)
	{
		CWaitCursor cursor;
		uint32* pChunkVector=new uint32[size];
		uint32 index=0;
		std::set<int>::iterator iter;
		for(iter=m_ChunkIdSet.begin();iter!=m_ChunkIdSet.end();++iter)
			pChunkVector[index++]=(uint32)*iter;
		CSGPWorldMap* pWorldMap=WorldEditorRenderInterface::GetInstance()->GetWorldMap();
		uint32* pColorMinimapData = WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainChunkColorMinimapTexture(pChunkVector,size);
		memcpy(pWorldMap->m_WorldChunkColorMiniMapTextureData,pColorMinimapData,pWorldMap->m_Header.m_iChunkColorminiMapSize*pWorldMap->m_Header.m_iChunkColorminiMapSize*sizeof(uint32));
		delete [] pChunkVector;
	}
}

bool ChunkTextureOperation::IsContainChunk(int chunkId)
{
	std::set<int>::iterator iter = m_ChunkIdSet.find(chunkId);
	if(iter!=m_ChunkIdSet.end()) return true;
	else return false;
}

/*******************************************************************
	class TerrainTextureOperation Implement
*******************************************************************/
TerrainTextureOperation::TerrainTextureOperation()
{
	m_minx=1000000;
	m_maxx=0;
	m_minz=1000000;
	m_maxz=0;
	m_pChunkTexOp = NULL;
}

TerrainTextureOperation::~TerrainTextureOperation()
{
	if(m_pChunkTexOp != NULL)
	{
		delete m_pChunkTexOp;
		m_pChunkTexOp = NULL;
	}	
}

void TerrainTextureOperation::undo()
{
	if(m_pChunkTexOp != NULL) m_pChunkTexOp->undo();

	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	int alphaTextureSize = pRenderInterface->GetChunkSize()*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
	uint32* pAlphaData=pRenderInterface->GetWorldMap()->m_WorldChunkAlphaTextureData;
	int offset = (3-m_nLayer)*8;
	uint32 mask=(0xFF<<offset);
	std::set<TerrainTextureOpData>::iterator iter;
	for(iter=m_DataVector.begin();iter!=m_DataVector.end();++iter)
		pAlphaData[iter->index]=(pAlphaData[iter->index]|mask)&((~mask)|(iter->prevAlpha<<offset));

	WorldMapManager::GetInstance()->EditTerrainChunkBlendTexture(m_minx,alphaTextureSize-1-m_maxz,m_maxx-m_minx+1,m_maxz-m_minz+1);

	FlushColorMiniMap();
	TerrainTextureHandler::GetInstance()->UpdateSelectedChunkInfo();
}

void TerrainTextureOperation::redo()
{
	if(m_pChunkTexOp != NULL) m_pChunkTexOp->redo();

	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	int alphaTextureSize = pRenderInterface->GetChunkSize()*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
	uint32* pAlphaData=pRenderInterface->GetWorldMap()->m_WorldChunkAlphaTextureData;
	int offset = (3-m_nLayer)*8;
	uint32 mask=(0xFF<<offset);
	std::set<TerrainTextureOpData>::iterator iter;
	for(iter=m_DataVector.begin();iter!=m_DataVector.end();++iter)
		pAlphaData[iter->index]=(pAlphaData[iter->index]|mask)&((~mask)|(iter->currAlpha<<offset));

	WorldMapManager::GetInstance()->EditTerrainChunkBlendTexture(m_minx,alphaTextureSize-1-m_maxz,m_maxx-m_minx+1,m_maxz-m_minz+1);

	FlushColorMiniMap();
	TerrainTextureHandler::GetInstance()->UpdateSelectedChunkInfo();
}

bool TerrainTextureOperation::FindData(TerrainTextureOpData& data,int& prevAlpha,int& currAlpha)
{
	std::set<TerrainTextureOpData>::iterator iter=m_DataVector.find(data);
	if(iter!=m_DataVector.end())
	{
		prevAlpha=iter->prevAlpha;
		currAlpha=iter->currAlpha;
		m_DataVector.erase(iter);
		return true;
	}
	return false;
}

void TerrainTextureOperation::AddData(TerrainTextureOpData& data)
{
	m_DataVector.insert(data);
}

void TerrainTextureOperation::ContainRect(int minx,int maxx,int minz,int maxz)
{
	if(minx<m_minx) m_minx=minx;
	if(maxx>m_maxx) m_maxx=maxx;
	if(minz<m_minz) m_minz=minz;
	if(maxz>m_maxz) m_maxz=maxz;
}

void TerrainTextureOperation::FlushColorMiniMap()
{
	uint32 size=m_ChunkIdSet.size();
	if(size!=0)
	{
		CWaitCursor cursor;
		uint32* pChunkVector=new uint32[size];
		uint32 index=0;
		std::set<int>::iterator iter;
		for(iter=m_ChunkIdSet.begin();iter!=m_ChunkIdSet.end();++iter)
			pChunkVector[index++]=(uint32)*iter;
		CSGPWorldMap* pWorldMap=WorldEditorRenderInterface::GetInstance()->GetWorldMap();
		uint32* pColorMinimapData = WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainChunkColorMinimapTexture(pChunkVector,size);
		memcpy(pWorldMap->m_WorldChunkColorMiniMapTextureData,pColorMinimapData,pWorldMap->m_Header.m_iChunkColorminiMapSize*pWorldMap->m_Header.m_iChunkColorminiMapSize*sizeof(uint32));
		delete [] pChunkVector;
	}
}

void TerrainTextureOperation::CheckOpAfterDelTex()
{
	int offset=(3-m_nLayer)*8;
	uint32 mask=(0xFF<<offset);

	ASSERT(m_pChunkTexOp==NULL);
	m_pChunkTexOp = new ChunkTextureOperation;
	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int mapSize = chunkSize*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
	uint32* pAlphaData = WorldEditorRenderInterface::GetInstance()->GetWorldMap()->m_WorldChunkAlphaTextureData;
	if(!m_ChunkIdSet.empty())
	{
		std::set<int>::iterator iter;
		for(iter = m_ChunkIdSet.begin();iter!=m_ChunkIdSet.end();++iter)
		{
			int chunkId = *iter;
			int index_x = chunkId % chunkSize;
			int index_z = chunkId / chunkSize;
			int minx = index_x*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
			int maxx = (index_x+1)*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
			int minz = (chunkSize-index_z-1)*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
			int maxz = (chunkSize-index_z)*SGPTT_TILENUM*SGPTBD_BLENDTEXTURE_DIMISION;
			bool bZero=true;
			for(int i= minx;i<=maxx&&bZero;++i)
			{
				for(int j = minz;j<=maxz&&bZero;++j)
				{
					int index=(mapSize-j)*mapSize+i;
					if((pAlphaData[index]&mask)!=0) bZero=false;
				}
			}
			if(bZero)
			{
				ChunkTextureOpData chunkOpData;
				chunkOpData.chunkId = chunkId;
				chunkOpData.nLayer = m_nLayer;
				chunkOpData.prevTexPath = WorldMapManager::GetInstance()->GetChunkTexture(chunkId,m_nLayer);
				chunkOpData.currTexPath = "";
				if(chunkOpData.prevTexPath!=chunkOpData.currTexPath)
				{
					WorldMapManager::GetInstance()->SetChunkTexture(chunkId,m_nLayer,"");
					m_pChunkTexOp->m_DataSet.insert(chunkOpData);
				}
			}
		}
	}
}
