#include "stdafx.h"
#include "TerrainGrassHandler.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <math.h>
#include <time.h>
#include "TerrainGrassOperation.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(TerrainGrassHandler)

TerrainGrassHandler::TerrainGrassHandler()
{
	m_BrushRadius = 2.0f;
	srand((unsigned)time(NULL));
	m_nTiledTexIndex = -1;
	m_pGrassOp = NULL;
	m_bLbtnDown = false;
	m_TexTileX = 0;
	m_TexTileY = 0;
}

TerrainGrassHandler::~TerrainGrassHandler()
{

}

void TerrainGrassHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	if(IsReady())
	{
		switch(m_BrushType)
		{
		case GrassType_Add:
			m_pGrassOp = new TerrainGrassOperation;
			AddGrassAction();
			break;
		case GrassType_Delete:
			m_pGrassOp = new TerrainGrassOperation;
			DeleteGrassAction();
			break;
		}
	}
	
	m_bLbtnDown = true;	
}

void TerrainGrassHandler::LButtonUp(CPoint pt,UINT nFlags)
{
	if(IsReady())
	{
		switch(m_BrushType)
		{
		case GrassType_Add:
		case GrassType_Delete:
			if(m_pGrassOp!=NULL)
			{
				m_pGrassOp->SaveCurrGrassData();
				if(m_pGrassOp->CheckValidate()) OperationManager::GetInstance()->AddNewOperation(m_pGrassOp);
				else delete m_pGrassOp;
				m_pGrassOp = NULL;
			}
			break;
		}
	}
	
	m_bLbtnDown = false;
}

void TerrainGrassHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if((nFlags&MK_LBUTTON)&&m_bLbtnDown)
	{
		if(IsReady())
		{
			switch(m_BrushType)
			{
			case GrassType_Add:
				AddGrassAction();
				break;
			case GrassType_Delete:
				DeleteGrassAction();
				break;
			}
		}
	}
}

void TerrainGrassHandler::DrawArea()
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	switch(m_BrushType)
	{
	case GrassType_Add:
	case GrassType_Delete:
		if(IsReady())
			WorldEditorRenderInterface::GetInstance()->RenderSquareIntersectArea(m_BrushRadius,0.0f,1.0f,1.0f);
		break;
	}
}

void TerrainGrassHandler::AddGrassAction()
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	float center_x,center_z;
	pRenderInterface->GetIntersectPoint(center_x,center_z);
	float mapMeter = (float)pRenderInterface->GetChunkSize()*SGPTT_TILENUM*SGPTT_TILE_METER;
	float minx = jlimit(0.0f,mapMeter,center_x-m_BrushRadius);
	float maxx = jlimit(0.0f,mapMeter,center_x+m_BrushRadius);
	float minz = jlimit(0.0f,mapMeter,center_z-m_BrushRadius);
	float maxz = jlimit(0.0f,mapMeter,center_z+m_BrushRadius);

	SGPGrassCluster grassCluster;
	Vector3D terrainNorm;
	for(float i=minx;i<=maxx;i+=0.5f)
	{
		for(float j=minz;j<=maxz;j+=0.5f)
		{
			if((rand()%120) < m_GrassDensity)
			{
				int chunkId = ((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(i,j);
				if(chunkId!=0xFFFFFFFF)
				{
					int pos;
					if(!m_pGrassOp->GetChunkDataPosition(chunkId,pos))
						m_pGrassOp->SavePrevGrassData(pos);
				}

				grassCluster.fPositionX = i;
				grassCluster.fPositionZ = j;
				grassCluster.fPositionY = pRenderInterface->GetTerrainHeight(i,j);
				terrainNorm = pRenderInterface->GetWorldSystemManager()->getTerrainNormal(i,j);
				grassCluster.nPackedNormal =(uint32((terrainNorm.x * 0.5f + 0.5f) * 255) << 24) +
					(uint32((terrainNorm.y * 0.5f + 0.5f) * 255) << 16) +
					(uint32((terrainNorm.z * 0.5f + 0.5f) * 255) << 8);
				uint32 grassHeight = rand()%(m_GrassMaxSize-m_GrassMinSize+1) + m_GrassMinSize;
				grassCluster.nData = (grassHeight<<24) + (m_nTiledTexIndex<<16) + (m_WindOffsetX<<8) + m_WindOffsetZ;
				pRenderInterface->GetWorldSystemManager()->setGrassCluster(i, j, grassCluster);			
			}
		}
	}
}

void TerrainGrassHandler::SetGrassInfo(CString texPath,int TexAtlasX,int TexAtlasY)
{
	m_GrassTexPath = texPath;
	m_TexTileX = TexAtlasX;
	m_TexTileY = TexAtlasY;
}

void TerrainGrassHandler::DeleteGrassAction()
{
	WorldEditorRenderInterface* pRenderInterface = WorldEditorRenderInterface::GetInstance();
	float center_x,center_z;
	int chunkSize = pRenderInterface->GetChunkSize();
	float mapMeter = (float)pRenderInterface->GetChunkSize()*SGPTT_TILENUM*SGPTT_TILE_METER;
	pRenderInterface->GetIntersectPoint(center_x,center_z);
	float minx = jlimit(0.0f,mapMeter,center_x-m_BrushRadius);
	float maxx = jlimit(0.0f,mapMeter,center_x+m_BrushRadius);
	float minz = jlimit(0.0f,mapMeter,center_z-m_BrushRadius);
	float maxz = jlimit(0.0f,mapMeter,center_z+m_BrushRadius);

	uint8 chunkId_x[4],chunkId_z[4];
	((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(chunkId_x[0],chunkId_z[0],minx,minz);
	((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(chunkId_x[1],chunkId_z[1],minx,maxz);
	((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(chunkId_x[2],chunkId_z[2],maxx,minz);
	((CSGPTerrain*)(pRenderInterface->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(chunkId_x[3],chunkId_z[3],maxx,maxz);

	int i,j,k;
	int chunkMin_x,chunkMax_x,chunkMin_z,chunkMax_z;
	chunkMin_x=chunkMax_x=(int)chunkId_x[0];
	chunkMin_z=chunkMax_z=(int)chunkId_z[0];
	for(i=1;i<4;++i)
	{
		if((int)chunkId_x[i] < chunkMin_x) chunkMin_x = (int)chunkId_x[i];
		if((int)chunkId_x[i] > chunkMax_x) chunkMax_x = (int)chunkId_x[i];
		if((int)chunkId_z[i] < chunkMin_z) chunkMin_z = (int)chunkId_z[i];
		if((int)chunkId_z[i] > chunkMax_z) chunkMax_z = (int)chunkId_z[i];
	}

	CSGPWorldMap* pWorldMap = pRenderInterface->GetWorldMap();
	SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag** ppChunkGrassCluster = pWorldMap->m_GrassData.m_ppChunkGrassCluster;

	for(i=chunkMin_x;i<=chunkMax_x;++i)
	{
		for(j=chunkMin_z;j<=chunkMax_z;++j)
		{
			int currIndex = j * chunkSize + i;
			SGPWorldMapGrassTag::SGPWorldMapChunkGrassClusterTag* pChunkGrassCluster=ppChunkGrassCluster[currIndex];
			if(pChunkGrassCluster!=NULL)
			{
				for(k=0;k<SGPTT_TILENUM*SGPTGD_GRASS_DIMISION*SGPTT_TILENUM*SGPTGD_GRASS_DIMISION;++k)
				{
					SGPGrassCluster& cluster = pChunkGrassCluster->m_GrassLayerData[k];
					if(cluster.fPositionX>=minx&&cluster.fPositionX<=maxx&&cluster.fPositionZ>=minz&&cluster.fPositionZ<=maxz)
					{
						int pos;
						if(!m_pGrassOp->GetChunkDataPosition(currIndex,pos))
							m_pGrassOp->SavePrevGrassData(pos);
						memset(&cluster,0,sizeof(SGPGrassCluster));
					}
				}
			}
		}
	}
}

BOOL TerrainGrassHandler::IsReady()
{
	if(m_nTiledTexIndex!=-1&&WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getGrass()!=NULL)
		return TRUE;
	else return FALSE;
}