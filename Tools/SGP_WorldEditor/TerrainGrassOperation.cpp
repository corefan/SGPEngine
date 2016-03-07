#include "stdafx.h"
#include "TerrainGrassOperation.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "HelpFunction.h"

TerrainGrassOperation::TerrainGrassOperation()
{

}

TerrainGrassOperation::~TerrainGrassOperation()
{
	int size = m_DataVector.size();
	for(int i=0;i < size ;++i)
	{
		if(m_DataVector[i].m_pGrassCurrData != NULL)
		{
			delete [] m_DataVector[i].m_pGrassCurrData;
			m_DataVector[i].m_pGrassCurrData = NULL;
		}
		if(m_DataVector[i].m_pGrassPrevData != NULL)
		{
			delete [] m_DataVector[i].m_pGrassPrevData;
			m_DataVector[i].m_pGrassPrevData = NULL;
		}
	}
}

void TerrainGrassOperation::undo()
{
	CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** ppChunkGrassCluster = pWorldMap->m_GrassData.m_ppChunkGrassCluster;
	int size = m_DataVector.size();
	for(int i=0; i<size; ++i)
	{
		int index = m_DataVector[i].chunkIndex;
		if(ppChunkGrassCluster[index] != NULL)
		{
			if(m_DataVector[i].m_pGrassPrevData != NULL)
				memcpy(ppChunkGrassCluster[index]->m_GrassLayerData,m_DataVector[i].m_pGrassPrevData,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
			else
				memset(ppChunkGrassCluster[index],0,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
		}
	}
}

void TerrainGrassOperation::redo()
{
	CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** ppChunkGrassCluster = pWorldMap->m_GrassData.m_ppChunkGrassCluster;
	int size = m_DataVector.size();
	for(int i=0; i<size; ++i)
	{
		int index = m_DataVector[i].chunkIndex;
		if(ppChunkGrassCluster[index] != NULL)
		{
			if(m_DataVector[i].m_pGrassCurrData != NULL)
				memcpy(ppChunkGrassCluster[index]->m_GrassLayerData,m_DataVector[i].m_pGrassCurrData,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
			else
				memset(ppChunkGrassCluster[index]->m_GrassLayerData,0,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
		}
	}
}

bool TerrainGrassOperation::GetChunkDataPosition(int chunkId,int& pos)
{
	int size = m_DataVector.size();
	for(int i=0;i<size;++i)
	{
		if(m_DataVector[i].chunkIndex==chunkId)
		{
			pos = i;
			return true;
		}
	}
	
	GrassOpData opData;
	opData.chunkIndex = chunkId;
	m_DataVector.push_back(opData);
	pos = size;

	return false;
}

void TerrainGrassOperation::SavePrevGrassData(int pos)
{
	CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* pChunkGrassCluster = pWorldMap->m_GrassData.m_ppChunkGrassCluster[m_DataVector[pos].chunkIndex];
	if(pChunkGrassCluster != NULL)
	{
		m_DataVector[pos].m_pGrassPrevData = new SGPGrassCluster[SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION];
		memcpy(m_DataVector[pos].m_pGrassPrevData,pChunkGrassCluster->m_GrassLayerData,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
	}
}

void TerrainGrassOperation::SaveCurrGrassData()
{
	CSGPWorldMap* pWorldMap = WorldEditorRenderInterface::GetInstance()->GetWorldMap();
	SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** ppChunkGrassCluster = pWorldMap->m_GrassData.m_ppChunkGrassCluster;

	int size = m_DataVector.size();
	for(int i=0; i<size; ++i)
	{
		SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* pChunkGrassCluster = ppChunkGrassCluster[m_DataVector[i].chunkIndex];
		if(pChunkGrassCluster != NULL)
		{
			m_DataVector[i].m_pGrassCurrData = new SGPGrassCluster[SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION];
			memcpy(m_DataVector[i].m_pGrassCurrData,pChunkGrassCluster->m_GrassLayerData,SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*sizeof(SGPGrassCluster));
		}
	}
}

bool TerrainGrassOperation::CheckValidate()
{
	int size = m_DataVector.size();
	for(int i=0;i<size;++i)
	{
		if(m_DataVector[i].m_pGrassPrevData!=NULL&&m_DataVector[i].m_pGrassCurrData!=NULL)
		{
			for(int j=0;j<SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION;++j)
			{
				if(m_DataVector[i].m_pGrassPrevData[j]!=m_DataVector[i].m_pGrassCurrData[j]) return true;
			}
		}
		else if(m_DataVector[i].m_pGrassPrevData!=NULL || m_DataVector[i].m_pGrassCurrData!=NULL)
			return true;
	}
	return false;

//	return true;
}
