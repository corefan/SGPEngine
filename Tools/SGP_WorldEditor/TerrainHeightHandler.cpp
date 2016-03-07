#include "stdafx.h"
#include "TerrainHeightHandler.h"
#include "TerrainHeightOperation.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include "OperationManager.h"
#include "MouseMsgInterface.h"
#include <math.h>
#include <time.h>
#include <set>

IMPLEMENT_SINGLETON(TerrainHeightHandler)

TerrainHeightHandler::TerrainHeightHandler(void)
{
	m_OuterRadius=10.0f;
	m_InnerRadius=6.0f;
	m_Intensity=1;
	m_BrushType=HeightType_None;
	srand((unsigned)time(NULL));
	m_SettedVertexHeightVal=0;
	m_bLBtnDown = false;
	m_pVertexHeightOp1 = NULL;
	m_pVertexHeightOp2 = NULL;
	m_bValidate = false;
}

TerrainHeightHandler::~TerrainHeightHandler(void)
{
}

void TerrainHeightHandler::LButtonDown(CPoint pt,UINT nFlags)
{
	m_bValidate=WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain();
	switch(m_BrushType)
	{
	case HeightType_Adjust:
		m_pVertexHeightOp2=new CVertexHeightOperation2;
		RetrieveAdjustVertex();
		break;
	case HeightType_Up:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		RaiseTerrainHeight();
		break;
	case HeightType_Down:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		ReduceTerrainHeight();
		break;
	case HeightType_Smooth:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		SmoothTerrainHeight();
		break;
	case HeightType_Noise:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		NoiseTerrainHeight();
		break;
	case HeightType_Flatten:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		RetrieveFlattenHeight();
		FlattenTerrainHeight();
		break;
	case HeightType_Set:
		m_pVertexHeightOp1=new CVertexHeightOperation1;
		SetTerrainHeight();
		break;
	case HeightType_ChunkAdjust:
		{
			int chunkId=WorldEditorRenderInterface::GetInstance()->GetCurrPosChunkId();
			if(nFlags&MK_CONTROL)// CTRL key is pressed
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
			
		}
		break;
	case HeightType_CellAdjust:
		{
			int cellId=WorldEditorRenderInterface::GetInstance()->GetCurrPosCellId();
			if(nFlags&MK_CONTROL)
			{
				if(cellId!=-1)
				{
					int size=m_CellIdVector.size();
					bool bFinded=false;
					for(int i=0;i<size;++i)
					{
						if(m_CellIdVector[i]==cellId)
						{
							bFinded=true;
							std::vector<int>::iterator iter=m_CellIdVector.begin()+i;
							m_CellIdVector.erase(iter);
							break;
						}
					}
					if(!bFinded) m_CellIdVector.push_back(cellId);
				}
				
			}
			else
			{
				m_CellIdVector.clear();
				if(cellId!=-1) m_CellIdVector.push_back(cellId);
			}
		}
	}
	
	m_PrevPoint=pt;
	m_LDownPoint=pt;
	m_dwStart=GetTickCount();
	m_bLBtnDown = true;
}

void TerrainHeightHandler::LButtonUp(CPoint pt,UINT nFlags)
{
	m_AdjustPointVector.clear();
	switch(m_BrushType)
	{
	case HeightType_Adjust:
		if(m_pVertexHeightOp2!=NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp2->IsValidate())	
			{
				m_pVertexHeightOp2->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp2);
			}
			else delete m_pVertexHeightOp2;
			m_pVertexHeightOp2 = NULL;
		}
		break;
	case HeightType_Up:
		if(m_pVertexHeightOp1 != NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp1->IsValidate())	
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	case HeightType_Down:
		if(m_pVertexHeightOp1 != NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp1->IsValidate())
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	case HeightType_Smooth:
		if(m_pVertexHeightOp1 != NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp1->IsValidate())
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	case HeightType_Noise:
		if(m_pVertexHeightOp1 != NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp1->IsValidate())
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	case HeightType_Flatten:
		if(m_bValidate&&m_pVertexHeightOp1 != NULL)
		{
			if(m_pVertexHeightOp1->IsValidate())
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	case HeightType_Set:
		if(m_pVertexHeightOp1 != NULL)
		{
			if(m_bValidate&&m_pVertexHeightOp1->IsValidate())
			{
				m_pVertexHeightOp1->FlushTerrainNormal();
				WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->initializeQuadTree();
				OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
			}
			else delete m_pVertexHeightOp1;
			m_pVertexHeightOp1 = NULL;
		}
		break;
	}
	m_bLBtnDown = false;
}

void TerrainHeightHandler::MouseMove(CPoint pt,UINT nFlags)
{
	if((nFlags&MK_LBUTTON)&&m_bLBtnDown)
	{
		switch(m_BrushType)
		{
		case HeightType_Adjust:
			AdjustVertexHeight(m_LDownPoint.y-pt.y);
			break;
		case HeightType_Up:
			if(GetTickCount()-m_dwStart>100)
			{
				m_dwStart=GetTickCount();
				RaiseTerrainHeight();
			}
			break;
		case HeightType_Down:
			if(GetTickCount()-m_dwStart>100)
			{
				m_dwStart=GetTickCount();
				ReduceTerrainHeight();
			}
			break;
		case HeightType_Smooth:
			if(GetTickCount()-m_dwStart>100)
			{
				m_dwStart=GetTickCount();
				SmoothTerrainHeight();
			}
			break;
		case HeightType_Noise:
			if(GetTickCount()-m_dwStart>100)
			{
				m_dwStart=GetTickCount();
				NoiseTerrainHeight();
			}
			break;
		case HeightType_Flatten:
			FlattenTerrainHeight();
			break;
		case HeightType_Set:
			SetTerrainHeight();
			break;
		}
	}
	m_PrevPoint=pt;
}

void TerrainHeightHandler::RetrieveAdjustVertex()
{
	if(!m_bValidate) return ;

	float x,z;
	int i,j;
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	// Inner Circle
	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_InnerRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_InnerRadius)/SGPTT_TILE_METER+1));

	float squareInnerRadius = m_InnerRadius*m_InnerRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz) <= squareInnerRadius)
			{
				AdjustedPoint adjustedPoint;
				adjustedPoint.index_x=i;
				adjustedPoint.index_z=j;
				adjustedPoint.ratio=1.0f;
				m_AdjustPointVector.push_back(adjustedPoint);

				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int height=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				opData.prevHeight=opData.currHeight=height;
				m_pVertexHeightOp2->AddData(opData);

				// Add chunk info
				HeightOp2AddChunk(i,j);
			}
		}
	}

	if(m_OuterRadius-m_InnerRadius <= 0.02f) return ;

	/******************************In Outer Circle******************************/
	min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	float squareOuterRadius = m_OuterRadius * m_OuterRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			float distance = (float)sqrt((x-posx)*(x-posx)+(z-posz)*(z-posz));
			if(distance<=m_OuterRadius&&distance>m_InnerRadius)
			{
				AdjustedPoint adjustPoint;
				adjustPoint.index_x=i;
				adjustPoint.index_z=j;
				float ratio=(m_OuterRadius-distance)/(m_OuterRadius-m_InnerRadius);
				adjustPoint.ratio=ratio;
				m_AdjustPointVector.push_back(adjustPoint);

				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int height=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				opData.prevHeight=opData.currHeight=height;
				m_pVertexHeightOp2->AddData(opData);

				// Add chunk info
				HeightOp2AddChunk(i,j);
			}
		}
	}
}

void TerrainHeightHandler::AdjustVertexHeight(int deltaY)
{
	if(!m_bValidate) return ;

	std::vector<TerrainHeightOpData>& DataVector=m_pVertexHeightOp2->m_DataVector;
	int size=DataVector.size();
	float intensityRatio=(0.2f-0.03f)/(100.0f-1.0f)*(m_Intensity-1.0f)+0.03f;
	for(int i=0;i<size;++i)
	{
		int dHeight=(int)(deltaY*m_AdjustPointVector[i].ratio*intensityRatio);
		int currHeight=DataVector[i].prevHeight+dHeight;
		DataVector[i].currHeight=jlimit(0,65535,currHeight);
		WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->setHeightMap(DataVector[i].index_x,\
			DataVector[i].index_z,DataVector[i].currHeight);
	}
	m_pVertexHeightOp2->FlushTerrainHeight();
}

void TerrainHeightHandler::RaiseTerrainHeight()
{
	if(!m_bValidate) return ;
	if(!WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()) return;

	float x,z;
	int i,j;
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	ISGPWorldSystemManager* pWorldManager = WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();

	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_InnerRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_InnerRadius)/SGPTT_TILE_METER+1));

	int strength=(8-1)*(m_Intensity-1)/(100-1)+1;
	float squareInnerRadius = m_InnerRadius*m_InnerRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareInnerRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
					currHeight=prevHeight;
				}
				
				opData.prevHeight=prevHeight;
				opData.currHeight=jlimit(0,65535,currHeight+strength);
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.currHeight!=opData.prevHeight)
				{
					m_pVertexHeightOp1->AddData(opData);	
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}

	// In Outer Circle
	min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			float distance= sqrt(squareDistance(x,z,posx,posz));
			if(distance<=m_OuterRadius&&distance>m_InnerRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
					currHeight=prevHeight;
				}

				opData.prevHeight=prevHeight;
				float ratio=(m_OuterRadius-distance)/(m_OuterRadius-m_InnerRadius);
				opData.currHeight=jlimit(0,65535,currHeight+(int)(ratio*strength));
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight)
				{
					m_pVertexHeightOp1->AddData(opData);	
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::ReduceTerrainHeight()
{
	if(!m_bValidate) return ;
	if(!WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()) return;

	float x,z;
	int i,j;
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	ISGPWorldSystemManager* pWorldManager = WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	// In Inner Circle
	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_InnerRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_InnerRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_InnerRadius)/SGPTT_TILE_METER+1));

	int strength=(8-1)*(m_Intensity-1)/(100-1)+1;
	float squareInnerRadius = m_InnerRadius*m_InnerRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareInnerRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
					currHeight=prevHeight;
				}

				opData.prevHeight=prevHeight;
				opData.currHeight=jlimit(0,65535,currHeight-strength);
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.currHeight!=opData.prevHeight)
				{
					m_pVertexHeightOp1->AddData(opData);	
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}

	// In Outer Circle
	min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			float distance=sqrt(squareDistance(x,z,posx,posz));
			if(distance<=m_OuterRadius&&distance>m_InnerRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
					currHeight=prevHeight;
				}

				opData.prevHeight=prevHeight;
				float ratio=(m_OuterRadius-distance)/(m_OuterRadius-m_InnerRadius);
				opData.currHeight=jlimit(0,65535,currHeight-(int)(ratio*strength));
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight)
				{
					m_pVertexHeightOp1->AddData(opData);			
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::SmoothTerrainHeight()
{
	if(!m_bValidate) return ;
	if(!WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()) return;

	float x,z;
	int i,j;
	ISGPWorldSystemManager* pWorldManager= WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int pointCount=chunkcount*SGPTT_TILENUM;
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	int min_intx = jlimit(0,pointCount,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,pointCount,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,pointCount,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,pointCount,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	float squareRadius=m_OuterRadius*m_OuterRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				}
				int count=1;
				int height=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				if(j>0)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j-1);
					++count;
				}
				if(i<pointCount&&j>0)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i+1,j-1);
					++count;
				}
				if(i>0)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i-1,j);
					++count;
				}
				if(i>0&&j>0)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i-1,j-1);
					++count;
				}
				if(i<pointCount)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i+1,j);
					++count;
				}
				if(i>0&&j<pointCount)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i-1,j+1);
					++count;
				}
				if(j<pointCount)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j+1);
					++count;
				}
				if(i<pointCount&&j<pointCount)
				{
					height+=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i+1,j+1);
					++count;
				}
				
				opData.prevHeight=prevHeight;
				opData.currHeight=jlimit(0,65535,height/count);

				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight)
				{
					m_pVertexHeightOp1->AddData(opData);
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::NoiseTerrainHeight()
{
	if(!m_bValidate) return ;
	if(!WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()) return;

	float x,z;
	int i,j;
	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	float squareRadius=m_OuterRadius*m_OuterRadius;
	int randRange=(10-2)*(m_Intensity-1)/(100-1)+2;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				int dHeight=rand()%randRange;
				int prevHeight,currHeight;
				if(!m_pVertexHeightOp1->FindData(opData,prevHeight,currHeight))
				{
					prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
					currHeight=prevHeight;
				}
				opData.prevHeight=prevHeight;
				opData.currHeight=jlimit(0,65535,currHeight+dHeight);
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight)
				{
					m_pVertexHeightOp1->AddData(opData);
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
				
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::RetrieveFlattenHeight()
{
	if(!m_bValidate) return ;
	
	float x,z;
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	float centerHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(x,z);
	m_FlattenHeight = jlimit(0,65535,(int)centerHeight);
}

void TerrainHeightHandler::FlattenTerrainHeight()
{
	if(!m_bValidate) return ;

	float x,z;
	int i,j;
	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	float squareRadius=m_OuterRadius*m_OuterRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareRadius)
			{
				TerrainHeightOpData opData;
				int height=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				opData.index_x=i;
				opData.index_z=j;
				opData.prevHeight=height;
				opData.currHeight=m_FlattenHeight;
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight)
				{
					m_pVertexHeightOp1->AddData(opData);
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::SetTerrainHeight()
{
	if(!m_bValidate) return ;
	if(!WorldEditorRenderInterface::GetInstance()->IsIntersectTerrain()) return;

	float x,z;
	int i,j;
	ISGPWorldSystemManager* pWorldManager=WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager();
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	int chunkcount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	float maxRange=(float)chunkcount*SGPTT_TILENUM*SGPTT_TILE_METER;

	int min_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intx = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((x+m_OuterRadius)/SGPTT_TILE_METER+1));
	int min_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z-m_OuterRadius)/SGPTT_TILE_METER-1));
	int max_intz = jlimit(0,chunkcount*SGPTT_TILENUM,(int)((maxRange-z+m_OuterRadius)/SGPTT_TILE_METER+1));

	int settedHeightVal=jlimit(0,65535,m_SettedVertexHeightVal);
	float squareRadius=m_OuterRadius*m_OuterRadius;
	for(i=min_intx;i<=max_intx;++i)
	{
		for(j=min_intz;j<=max_intz;++j)
		{
			float posx=(float)i*SGPTT_TILE_METER;
			float posz=maxRange-(float)j*SGPTT_TILE_METER;
			if(squareDistance(x,z,posx,posz)<=squareRadius)
			{
				TerrainHeightOpData opData;
				opData.index_x=i;
				opData.index_z=j;
				opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(i,j);
				opData.currHeight=settedHeightVal;
				pWorldManager->setHeightMap(opData.index_x,opData.index_z,opData.currHeight);
				if(opData.currHeight!=opData.prevHeight)
				{
					m_pVertexHeightOp1->AddData(opData);
					// Add chunk info
					HeightOp1AddChunk(i,j);
				}
			}
		}
	}
	m_pVertexHeightOp1->FlushTerrainHeight();
}

void TerrainHeightHandler::AdjustChunkHeight(int height,bool bSet)
{
	int chunkCount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();

	m_pVertexHeightOp1=new CVertexHeightOperation1;
	int size=m_ChunkIdVector.size();
	for(int i=0;i<size;++i)
	{
		int chunkId=m_ChunkIdVector[i];
		int z=chunkId/chunkCount;
		int x=chunkId%chunkCount;
		for(int m=0;m<=SGPTT_TILENUM;++m)
		{
			for(int n=0;n<=SGPTT_TILENUM;++n)
			{
				int index_x=x*SGPTT_TILENUM+m;
				int index_z=/*chunkCount*SGPTT_TILENUM-*/z*SGPTT_TILENUM+n;
				TerrainHeightOpData opData;
				opData.index_x=index_x;
				opData.index_z=index_z;
				opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(index_x,index_z);
				if(bSet) opData.currHeight = height;
				else opData.currHeight=opData.prevHeight+height;

				opData.currHeight=jlimit(0,65535,opData.currHeight);
				if(opData.prevHeight!=opData.currHeight) m_pVertexHeightOp1->AddData(opData);
				// Add chunk info
				HeightOp1AddChunk(index_x,index_z);
			}
		}
	}
	if(m_pVertexHeightOp1->IsValidate())
	{
		m_pVertexHeightOp1->redo();
		OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
	}
	else
		delete m_pVertexHeightOp1;
}

void TerrainHeightHandler::AdjustCellHeight(int height,bool bSet)
{
	int chunkCount=WorldEditorRenderInterface::GetInstance()->GetChunkSize();

	m_pVertexHeightOp1=new CVertexHeightOperation1;

	int size=m_CellIdVector.size();
	for(int i=0;i<size;++i)
	{
		int cellId=m_CellIdVector[i];
		int x=cellId%(chunkCount*SGPTT_TILENUM+1);
		int z=cellId/(chunkCount*SGPTT_TILENUM+1);

		TerrainHeightOpData opData;
		opData.index_x=x;
		opData.index_z=z+1;
		opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(opData.index_x,opData.index_z);
		if(bSet) opData.currHeight = height;
		else opData.currHeight=opData.prevHeight+height;
		opData.currHeight=jlimit(0,65535,opData.currHeight);
		if(opData.currHeight!=opData.prevHeight) m_pVertexHeightOp1->AddData(opData);
		// Add chunk info
		HeightOp1AddChunk(opData.index_x,opData.index_z);
		
		opData.index_x=x+1;
		opData.index_z=z+1;
		opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(opData.index_x,opData.index_z);
		if(bSet) opData.currHeight = height;
		else opData.currHeight=opData.prevHeight+height;
		opData.currHeight=jlimit(0,65535,opData.currHeight);
		if(opData.currHeight!=opData.prevHeight) m_pVertexHeightOp1->AddData(opData);
		// Add chunk info
		HeightOp1AddChunk(opData.index_x,opData.index_z);

		opData.index_x=x+1;
		opData.index_z=z;
		opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(opData.index_x,opData.index_z);
		if(bSet) opData.currHeight = height;
		else opData.currHeight = opData.prevHeight+height;
		opData.currHeight=jlimit(0,65535,opData.currHeight);
		if(opData.currHeight!=opData.prevHeight) m_pVertexHeightOp1->AddData(opData);
		// Add chunk info
		HeightOp1AddChunk(opData.index_x,opData.index_z);

		opData.index_x=x;
		opData.index_z=z;
		opData.prevHeight=WorldEditorRenderInterface::GetInstance()->GetTerrainHeight(opData.index_x,opData.index_z);
		if(bSet) opData.currHeight = height;
		else opData.currHeight = opData.prevHeight+height;
		opData.currHeight=jlimit(0,65535,opData.currHeight);
		if(opData.currHeight!=opData.prevHeight) m_pVertexHeightOp1->AddData(opData);
		// Add chunk info
		HeightOp1AddChunk(opData.index_x,opData.index_z);
	}

	if(m_pVertexHeightOp1->IsValidate())
	{
		m_pVertexHeightOp1->redo();
		OperationManager::GetInstance()->AddNewOperation(m_pVertexHeightOp1);
	}
	else
		delete m_pVertexHeightOp1;
}

void TerrainHeightHandler::AdjustChunkCellHeight(int height,bool bSet)
{
	if(m_BrushType==HeightType_ChunkAdjust) AdjustChunkHeight(height,bSet);
	if(m_BrushType==HeightType_CellAdjust) AdjustCellHeight(height,bSet);
}

void TerrainHeightHandler::AdjustWorldHeight(int height)
{
	CVertexHeightOperation2* pHeightOperation=new CVertexHeightOperation2;
	pHeightOperation->m_bWorld = true;

	WorldEditorRenderInterface* pInterface=WorldEditorRenderInterface::GetInstance();
	int chunkCount=pInterface->GetChunkSize();
	int cellCount=chunkCount*SGPTT_TILENUM;
	pHeightOperation->ReserveMemory((cellCount+1)*(cellCount+1));

	BOOL bWorldHeight=TRUE;

	for(int i=0;i<=cellCount;++i)
	{
		for(int j=0;j<=cellCount;++j)
		{
			TerrainHeightOpData opData;
			opData.index_x=i;
			opData.index_z=j;
			opData.prevHeight=pInterface->GetTerrainHeight(i,j);
			int currHeight=opData.prevHeight+height;
			opData.currHeight=jlimit(0,65535,currHeight);
			if(opData.currHeight-opData.prevHeight!=height) bWorldHeight=FALSE;
			if(opData.prevHeight!=opData.currHeight) pHeightOperation->AddData(opData);
		}
	}

	if(bWorldHeight)
	{
		CWorldHeightOperation* pWorldOperation=new CWorldHeightOperation;
		pWorldOperation->SetDeltaHeight(height);
		pWorldOperation->redo();
		OperationManager::GetInstance()->AddNewOperation(pWorldOperation);
		
		delete pHeightOperation;
	}
	else
	{
		if(pHeightOperation->HaveData())
		{
			pHeightOperation->redo();
			OperationManager::GetInstance()->AddNewOperation(pHeightOperation);
		}
		else
			delete pHeightOperation;
	}
}

void TerrainHeightHandler::SetBrushType(HeightBrushType type)
{
	m_BrushType=type;
	if(m_BrushType!=HeightType_ChunkAdjust) m_ChunkIdVector.clear();
	if(m_BrushType!=HeightType_CellAdjust) m_CellIdVector.clear();
}

void TerrainHeightHandler::ChangeSettedVertexHeightVal(int val)
{
	if(val<0) val=0;
	m_SettedVertexHeightVal=val;
}

float TerrainHeightHandler::squareDistance(float x1,float y1,float x2,float y2)
{
	return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

void TerrainHeightHandler::DrawArea()
{
	WorldEditorRenderInterface* pInterface=WorldEditorRenderInterface::GetInstance();
	switch(m_BrushType)
	{
	case HeightType_Adjust:
	case HeightType_Up:
	case HeightType_Down:
		if(pInterface->IsIntersectTerrain())
		{
			pInterface->RenderRoundIntersectArea(m_OuterRadius,1.0f,0.0f,0.0f);
			pInterface->RenderRoundIntersectArea(m_InnerRadius,0.0f,1.0f,0.0f);
		}
		break;
	case HeightType_Smooth:
	case HeightType_Noise:
	case HeightType_Flatten:
	case HeightType_Set:
		if(pInterface->IsIntersectTerrain())
			pInterface->RenderRoundIntersectArea(m_OuterRadius,1.0f,0.0f,0.0f);
		break;
	case HeightType_ChunkAdjust:
		{
			int size=m_ChunkIdVector.size();
			for(int i=0;i<size;++i) pInterface->RenderSelectedChunk(m_ChunkIdVector[i],1.0f,1.0f,1.0f);
			if(MouseMsgInterface::GetInstance()->CursorInRenderWindow())
			{
				int chunkId = pInterface->GetCurrPosChunkId();
				if(chunkId!=-1) pInterface->RenderSelectedChunk(chunkId,0.0f,1.0f,1.0f);
			}
		}
		break;
	case HeightType_CellAdjust:
		{
			int size=m_CellIdVector.size();
			for(int i=0;i<size;++i) pInterface->RenderSelectedCell(m_CellIdVector[i],255,255,255);
			if(MouseMsgInterface::GetInstance()->CursorInRenderWindow())
			{
				int cellId = pInterface->GetCurrPosCellId();
				if(cellId!=-1) pInterface->RenderSelectedCell(cellId,0,255,255);
			}
		}
		break;
	}
}

void TerrainHeightHandler::HeightOp1AddChunk(int index_x,int index_z)
{
	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int chunkId_x = index_x / SGPTT_TILENUM;
	int chunkId_z = index_z / SGPTT_TILENUM;
	ASSERT(chunkId_x>=0&&chunkId_x<=chunkSize&&chunkId_z>=0&&chunkId_z<=chunkSize);

	if(chunkId_x < chunkSize &&chunkId_z < chunkSize)
		m_pVertexHeightOp1->AddChunk(chunkId_z*chunkSize+chunkId_x);
/*	if(index_x%SGPTT_TILENUM == 0&&chunkId_x > 0&&chunkId_z < chunkSize)
		m_pVertexHeightOp1->AddChunk(chunkId_z*chunkSize+chunkId_x-1);
	if(index_z%SGPTT_TILENUM == 0&&chunkId_z > 0&&chunkId_x < chunkSize)
		m_pVertexHeightOp1->AddChunk((chunkId_z-1)*chunkSize+chunkId_x);
	if(index_x%SGPTT_TILENUM == 0&&chunkId_x > 0&&index_z%SGPTT_TILENUM == 0&&chunkId_z > 0)
		m_pVertexHeightOp1->AddChunk((chunkId_z-1)*chunkSize+chunkId_x-1);*/
	if(chunkId_x > 0 && chunkId_z > 0)
		m_pVertexHeightOp1->AddChunk((chunkId_z-1)*chunkSize+chunkId_x-1);
	if(chunkId_x < chunkSize && chunkId_z > 0)
		m_pVertexHeightOp1->AddChunk((chunkId_z-1)*chunkSize+chunkId_x);
	if(chunkId_x+1 < chunkSize && chunkId_z > 0)
		m_pVertexHeightOp1->AddChunk((chunkId_z-1)*chunkSize+chunkId_x+1);

	if(chunkId_x > 0 && chunkId_z < chunkSize)
		m_pVertexHeightOp1->AddChunk(chunkId_z*chunkSize+chunkId_x-1);
	if(chunkId_x+1 < chunkSize && chunkId_z < chunkSize)
		m_pVertexHeightOp1->AddChunk(chunkId_z*chunkSize+chunkId_x+1);

	if(chunkId_x > 0 && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp1->AddChunk((chunkId_z+1)*chunkSize+chunkId_x-1);
	if(chunkId_x < chunkSize && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp1->AddChunk((chunkId_z+1)*chunkSize+chunkId_x);
	if(chunkId_x+1 < chunkSize && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp1->AddChunk((chunkId_z+1)*chunkSize+chunkId_x+1);
}

void TerrainHeightHandler::HeightOp2AddChunk(int index_x,int index_z)
{
	int chunkSize = WorldEditorRenderInterface::GetInstance()->GetChunkSize();
	int chunkId_x = index_x / SGPTT_TILENUM;
	int chunkId_z = index_z / SGPTT_TILENUM;
	ASSERT(chunkId_x>=0&&chunkId_x<=chunkSize&&chunkId_z>=0&&chunkId_z<=chunkSize);

	if(chunkId_x < chunkSize &&chunkId_z < chunkSize)
		m_pVertexHeightOp2->AddChunk(chunkId_z*chunkSize+chunkId_x);
/*	if(index_x%SGPTT_TILENUM == 0&&chunkId_x > 0&&chunkId_z < chunkSize)
		m_pVertexHeightOp2->AddChunk(chunkId_z*chunkSize+chunkId_x-1);
	if(index_z%SGPTT_TILENUM == 0&&chunkId_z > 0&&chunkId_x < chunkSize)
		m_pVertexHeightOp2->AddChunk((chunkId_z-1)*chunkSize+chunkId_x);
	if(index_x%SGPTT_TILENUM == 0&&chunkId_x > 0&&index_z%SGPTT_TILENUM == 0&&chunkId_z > 0)
		m_pVertexHeightOp2->AddChunk((chunkId_z-1)*chunkSize+chunkId_x-1);*/
	if(chunkId_x > 0 && chunkId_z > 0)
		m_pVertexHeightOp2->AddChunk((chunkId_z-1)*chunkSize+chunkId_x-1);
	if(chunkId_x < chunkSize && chunkId_z > 0)
		m_pVertexHeightOp2->AddChunk((chunkId_z-1)*chunkSize+chunkId_x);
	if(chunkId_x+1 < chunkSize && chunkId_z > 0)
		m_pVertexHeightOp2->AddChunk((chunkId_z-1)*chunkSize+chunkId_x+1);

	if(chunkId_x > 0 && chunkId_z < chunkSize)
		m_pVertexHeightOp2->AddChunk(chunkId_z*chunkSize+chunkId_x-1);
	if(chunkId_x+1 < chunkSize && chunkId_z < chunkSize)
		m_pVertexHeightOp2->AddChunk(chunkId_z*chunkSize+chunkId_x+1);

	if(chunkId_x > 0 && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp2->AddChunk((chunkId_z+1)*chunkSize+chunkId_x-1);
	if(chunkId_x < chunkSize && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp2->AddChunk((chunkId_z+1)*chunkSize+chunkId_x);
	if(chunkId_x+1 < chunkSize && chunkId_z+1 < chunkSize)
		m_pVertexHeightOp2->AddChunk((chunkId_z+1)*chunkSize+chunkId_x+1);
}