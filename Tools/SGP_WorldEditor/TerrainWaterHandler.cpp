#include "stdafx.h"
#include "TerrainWaterHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "TerrainWaterOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(TerrainWaterHandler)

TerrainWaterHandler::TerrainWaterHandler()
{
	m_WaterHeight = -1.0f;
	m_bLbtnDown = false;
	m_pWaterOp = NULL;
}

TerrainWaterHandler::~TerrainWaterHandler()
{

}

void TerrainWaterHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	CreateWaterIfNeed();
	if(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater() == NULL) return ;
	m_bLbtnDown = true;

	switch(m_BrushType)
	{
	case WaterType_Add:
		{
			m_pWaterOp = new TerrainWaterOperation(true);
			m_pWaterOp->SetWaterAttr(m_WaterTexPath,m_WaterHeight);
			AddWater();
		}
		break;
	case WaterType_Delete:
		{
			m_pWaterOp = new TerrainWaterOperation(false);
			m_pWaterOp->SetWaterAttr(m_WaterTexPath,m_WaterHeight);
			DeleteWater();
		}
		break;
	}
}

void TerrainWaterHandler::LButtonUp(CPoint pt,UINT nFlags)
{
	if(m_bLbtnDown)
	{
		switch (m_BrushType)
		{
		case WaterType_Add:
		case WaterType_Delete:
			{
				if(m_pWaterOp!=NULL)
				{
					if(m_pWaterOp->IsValidate())
					{
						WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
						OperationManager::GetInstance()->AddNewOperation(m_pWaterOp);
					}
					else delete m_pWaterOp;
					m_pWaterOp = NULL;
				}
			}
			break;
		}
	}
	m_bLbtnDown = false;
}

void TerrainWaterHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater() == NULL) return ;

	if((nFlags & MK_LBUTTON)&&m_bLbtnDown)
	{
		switch(m_BrushType)
		{
		case WaterType_Add:
			AddWater();
			break;
		case WaterType_Delete:
			DeleteWater();
			break;
		}
	}
}

void TerrainWaterHandler::DrawArea()
{
	switch(m_BrushType)
	{
	case WaterType_Add:
	case WaterType_Delete:
		if((!m_WaterTexPath.IsEmpty())&&m_WaterHeight!= -1.0f)
		{
			int chunkId=WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
			if(chunkId!=-1)	WorldEditorRenderInterface::GetInstance()->RenderSelectedChunk(chunkId,m_WaterHeight,0.0f,1.0f,1.0f);
		}
		break;
	}
}

bool TerrainWaterHandler::IsNewWaterChunk(int chunkId)
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	CSGPWater* pWater = (CSGPWater*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater());
	if(pWater != NULL)
	{
		for(int i=0;i<pWater->m_TerrainWaterChunks.size();++i)
		{
			if(pWater->m_TerrainWaterChunks[i]->GetTerrainChunkIndex()==(uint16)chunkId) return false;
		}
	}
	return true;
}

void TerrainWaterHandler::SetWaterAttr(CString texPath,float height)
{
	m_WaterTexPath = texPath;
	m_WaterHeight = height;
}

void TerrainWaterHandler::CreateWaterIfNeed()
{
	if(m_WaterHeight!=-1.0f &&(!m_WaterTexPath.IsEmpty())&&m_BrushType!=WaterType_None)
	{
		if(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getWater()==NULL)
		{
			BSTR texPath = m_WaterTexPath.AllocSysString();
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->createWater(m_WaterHeight,String(texPath));
			SysFreeString(texPath);
		}
	}
}

void TerrainWaterHandler::AddWater()
{
	int chunkId = WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
	if(chunkId!=-1)
	{
		if(IsNewWaterChunk(chunkId))// don't exist previous
		{
			WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->addWaterChunkForEditor(&chunkId,1);
			if(!IsNewWaterChunk(chunkId))// add successful
				m_pWaterOp->AddChunkIndex(chunkId);
		}
	}	
}

void TerrainWaterHandler::DeleteWater()
{
	int chunkId=WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
	if(!IsNewWaterChunk(chunkId))
	{
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->removeWaterChunkForEditor(&chunkId,1);
		ASSERT(IsNewWaterChunk(chunkId));
		m_pWaterOp->AddChunkIndex(chunkId);
	}
}
