#include "stdafx.h"
#include "TerrainWaterOperation.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"

TerrainWaterOperation::TerrainWaterOperation(bool bAdd)
{
	m_bAdd = bAdd;
}

TerrainWaterOperation::~TerrainWaterOperation()
{

}

void TerrainWaterOperation::undo()
{
	int size = m_ChunkIdVector.size();
	if(size != 0)
	{
		if(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater() == NULL)
		{
			BSTR waterPath = m_WaterTexPath.AllocSysString();
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->createWater(m_WaterHeight,String(waterPath));
			SysFreeString(waterPath);
		}

		int* chunkArray = new int[size];
		for(int i=0;i<size;++i) chunkArray[i]=m_ChunkIdVector[i];
		WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
		if(m_bAdd) pRenderInterface->GetWorldSystemManager()->removeWaterChunkForEditor(chunkArray,size);
		else pRenderInterface->GetWorldSystemManager()->addWaterChunkForEditor(chunkArray,size);
		pRenderInterface->GetWorldSystemManager()->initializeQuadTree();
		delete [] chunkArray;
	}
}

void TerrainWaterOperation::redo()
{
	int size = m_ChunkIdVector.size();
	if(size != 0)
	{
		if(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater() == NULL)
		{
			BSTR waterPath = m_WaterTexPath.AllocSysString();
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->createWater(m_WaterHeight,String(waterPath));
			SysFreeString(waterPath);
		}

		int* chunkArray = new int[size];
		for(int i=0;i<size;++i) chunkArray[i]=m_ChunkIdVector[i];
		WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
		if(m_bAdd) pRenderInterface->GetWorldSystemManager()->addWaterChunkForEditor(chunkArray,size);
		else pRenderInterface->GetWorldSystemManager()->removeWaterChunkForEditor(chunkArray,size);
		pRenderInterface->GetWorldSystemManager()->initializeQuadTree();
		delete [] chunkArray;
	}
}

void TerrainWaterOperation::SetWaterAttr(CString texPath,float height)
{
	m_WaterTexPath = texPath;
	m_WaterHeight = height;
}