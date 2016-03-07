#include "stdafx.h"
#include "TerrainHeightOperation.h"
#include "../SGPLibraryCode/SGPHeader.h"
#include "./Render Interface/WorldEditorRenderInterface.h"
#include "WorldMapManager.h"

/**********************************************************************************
	class CVertexHeightOperation1 implement
**********************************************************************************/

void CVertexHeightOperation1::undo()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	std::set<TerrainHeightOpData>::iterator iter;
	for(iter=m_DataVector.begin();iter!=m_DataVector.end();++iter)
		pRenderInterface->GetWorldSystemManager()->setHeightMap(iter->index_x,iter->index_z,iter->prevHeight);

	FlushTerrainHeight();
	FlushTerrainNormal();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void CVertexHeightOperation1::redo()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	std::set<TerrainHeightOpData>::iterator iter;
	for(iter=m_DataVector.begin();iter!=m_DataVector.end();++iter)
		pRenderInterface->GetWorldSystemManager()->setHeightMap(iter->index_x,iter->index_z,iter->currHeight);

	FlushTerrainHeight();
	FlushTerrainNormal();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

bool CVertexHeightOperation1::FindData(TerrainHeightOpData& data,int& prevHeight,int& currHeight)
{
	std::set<TerrainHeightOpData>::iterator iter;
	iter=m_DataVector.find(data);
	if(iter!=m_DataVector.end())
	{
		prevHeight=iter->prevHeight;
		currHeight=iter->currHeight;
		m_DataVector.erase(iter);
		return true;
	}
	return false;
}

void CVertexHeightOperation1::FlushTerrainHeight()
{
	uint32 size = m_ChunkSet.size();
	if(size>0)
	{
		uint32* pChunkVector = new uint32[size];
		int index=0;
		std::set<unsigned int>::iterator iter;
		for(iter=m_ChunkSet.begin();iter!=m_ChunkSet.end();++iter)
			pChunkVector[index++] = *iter;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainHeight(pChunkVector,size);
		delete [] pChunkVector;
	}
}

void CVertexHeightOperation1::FlushTerrainNormal()
{
	int size = m_ChunkSet.size();
	if(size > 0)
	{
		CWaitCursor cursor;

		uint32* pChunkVector = new uint32[size];
		std::set<uint32>::iterator iter;
		int index = 0;
		for(iter=m_ChunkSet.begin();iter!=m_ChunkSet.end();++iter)
			pChunkVector[index++] = *iter;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainNormal(pChunkVector,size);
		delete [] pChunkVector;
	}
	WorldMapManager::GetInstance()->NotifyRefreshCollSet();
}

/*************************************************************************************
	class CVertexHeightOperation2 implement
*************************************************************************************/
void CVertexHeightOperation2::undo()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	int size=m_DataVector.size();
	for(int i=0;i<size;++i)
		pRenderInterface->GetWorldSystemManager()->setHeightMap(m_DataVector[i].index_x,\
			m_DataVector[i].index_z,m_DataVector[i].prevHeight);
	
	FlushTerrainHeight();
	FlushTerrainNormal();
	pRenderInterface->GetWorldSystemManager()->initializeQuadTree();
}

void CVertexHeightOperation2::redo()
{
	WorldEditorRenderInterface* pRenderInterface=WorldEditorRenderInterface::GetInstance();
	int size=m_DataVector.size();
	for(int i=0;i<size;++i)
		pRenderInterface->GetWorldSystemManager()->setHeightMap(m_DataVector[i].index_x,\
			m_DataVector[i].index_z,m_DataVector[i].currHeight);

	FlushTerrainHeight();
	FlushTerrainNormal();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void CVertexHeightOperation2::FlushTerrainNormal()
{
	if(m_bWorld)
	{
		CWaitCursor cursor;

		int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
		int chunkCount =chunkSize * chunkSize;
		uint32* pChunkVector = new uint32[chunkCount];
		for(int i=0;i < chunkCount;++i) pChunkVector[i] = (int)i;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainNormal(pChunkVector,chunkCount);
		delete [] pChunkVector;
	}
	else
	{
		CWaitCursor cursor;

		int size = m_ChunkSet.size();
		if(size > 0)
		{
			uint32* pChunkVector = new uint32[size];
			std::set<uint32>::iterator iter;
			int index = 0;
			for(iter=m_ChunkSet.begin();iter!=m_ChunkSet.end();++iter)
				pChunkVector[index++] = *iter;
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainNormal(pChunkVector,size);
			delete [] pChunkVector;
		}
	}
	WorldMapManager::GetInstance()->NotifyRefreshCollSet();
}

void CVertexHeightOperation2::FlushTerrainHeight()
{
	if(m_bWorld)
	{
		int chunkSize=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
		uint32 chunkCount=chunkSize*chunkSize;
		uint32* pChunkVector= new uint32[chunkCount];
		for(uint32 i=0;i<chunkCount;++i) pChunkVector[i]=i;
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainHeight(pChunkVector,chunkCount);
		delete [] pChunkVector;
	}
	else
	{
		uint32 size = m_ChunkSet.size();
		if(size>0)
		{
			uint32* pChunkVector = new uint32[size];
			int index=0;
			std::set<unsigned int>::iterator iter;
			for(iter=m_ChunkSet.begin();iter!=m_ChunkSet.end();++iter)
				pChunkVector[index++] = *iter;
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainHeight(pChunkVector,size);
			delete [] pChunkVector;
		}
	}
}

bool CVertexHeightOperation2::IsValidate()
{
	bool bValid=false;
	int size=m_DataVector.size();	
	for(int i=0;i<size;++i)
	{
		if(m_DataVector[i].prevHeight!=m_DataVector[i].currHeight)
		{
			bValid=true;
			break;
		}
	}
	return bValid;
}

/*****************************************************************************
	class CWorldHeightOperation implement
*****************************************************************************/

void CWorldHeightOperation::undo()
{
	WorldEditorRenderInterface* pInterface=WorldEditorRenderInterface::GetInstance();
	int chunkCount=pInterface->GetChunkSize();
	int cellCount=chunkCount*8;
	for(int i=0;i<=cellCount;++i)
	{
		for(int j=0;j<=cellCount;++j)
		{
			int height=pInterface->GetTerrainHeight(i,j);
			pInterface->GetWorldSystemManager()->setHeightMap(i,j,height-m_dHeight);
		}
	}

	FlushWorldHeight();
	FlushWorldNormal();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void CWorldHeightOperation::redo()
{
	WorldEditorRenderInterface* pInterface=WorldEditorRenderInterface::GetInstance();
	int chunkCount=pInterface->GetChunkSize();
	int cellCount=chunkCount*8;
	for(int i=0;i<=cellCount;++i)
	{
		for(int j=0;j<=cellCount;++j)
		{
			int height=pInterface->GetTerrainHeight(i,j);
			pInterface->GetWorldSystemManager()->setHeightMap(i,j,height+m_dHeight);
		}
	}
	
	FlushWorldHeight();
	FlushWorldNormal();
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
}

void CWorldHeightOperation::FlushWorldNormal()
{
	CWaitCursor cursor;

	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int chunkCount =chunkSize * chunkSize;
	uint32* pChunkVector = new uint32[chunkCount];
	for(int i=0;i < chunkCount;++i) pChunkVector[i] = (uint32)i;
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainNormal(pChunkVector,chunkCount);
	delete [] pChunkVector;

	WorldMapManager::GetInstance()->NotifyRefreshCollSet();
}

void CWorldHeightOperation::FlushWorldHeight()
{
	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	uint32 chunkCount =chunkSize * chunkSize;
	uint32* pChunkVector = new uint32[chunkCount];
	for(uint32 i=0;i<chunkCount;++i) pChunkVector[i]=i;
	WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->flushTerrainHeight(pChunkVector,chunkCount);
	delete [] pChunkVector;
}