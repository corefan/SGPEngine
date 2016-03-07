#include "..\stdafx.h"

#include "WorldEditorRender.h"
#include "WorldEditorRenderInterface.h"
#include "WorldEditorCamera.h"
#include "..\WorldMapManager.h"
#include "..\WorldEditorConfig.h"
#include <set>
#include <limits.h>
#include "..\EditorAxis.h"
#include "..\CommonObjDef.h"
#include "..\HelpFunction.h"

extern ESGPTerrainChunkTexture ChunkLayer[7];

IMPLEMENT_SINGLETON(WorldEditorRenderInterface)

WorldEditorRenderInterface::WorldEditorRenderInterface()
{
	m_pRender = new WorldEditorRender;
	ASSERT(m_pRender != NULL);
}

WorldEditorRenderInterface::~WorldEditorRenderInterface()
{
	if(m_pRender != NULL)
	{
		delete m_pRender;
		m_pRender = NULL;
	}
}

void WorldEditorRenderInterface::SetCurPos(int x,int y)
{
	m_pRender->SetCurPos(x,y);
	m_pRender->GetWorldIntersectPoint();
}

int WorldEditorRenderInterface::GetTerrainHeight(int index_x,int index_z)
{
	const CSGPTerrain* pTerrain=m_pRender->m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	uint32 heightmap_index = index_z * (GetChunkSize()*SGPTT_TILENUM+1) + index_x;
	return ((CSGPTerrain*)pTerrain)->GetHeightMap()[heightmap_index];
}

float WorldEditorRenderInterface::GetTerrainHeight(float pos_x,float pos_z)
{
	return m_pRender->m_pRenderDevice->GetWorldSystemManager()->getTerrainHeight(pos_x,pos_z);
}

void WorldEditorRenderInterface::GetIntersectPoint(float &x,float &z)
{
	x=m_pRender->m_IntersectPoint.x;
	z=m_pRender->m_IntersectPoint.z;
}

float WorldEditorRenderInterface::GetIntersectHeight()
{
	return m_pRender->m_IntersectPoint.y;
}

int WorldEditorRenderInterface::GetChunkSize()
{
	const CSGPTerrain* pTerrain=m_pRender->m_pRenderDevice->GetWorldSystemManager()->getTerrain();
	uint32 chunkSize=((CSGPTerrain*)pTerrain)->GetTerrainChunkSize();
	return (int)chunkSize;
}

void WorldEditorRenderInterface::SetRotateCenter()
{
	m_RotateX=m_pRender->m_IntersectPoint.x;
	m_RotateY=m_pRender->m_IntersectPoint.y;
	m_RotateZ=m_pRender->m_IntersectPoint.z;
	m_pRender->m_pCamera->SetLookAt(m_RotateX,m_RotateY,m_RotateZ);
}

void WorldEditorRenderInterface::RenderRotateCenter()
{
	Vector3D center;
	center.x=m_RotateX;
	center.y=m_RotateY+1.0f;
	center.z=m_RotateZ;
	m_pRender->m_pRenderDevice->GetVertexCacheManager()->RenderCone(center,0.5f,-1.0f,4,Colour(0,0,255));
}

CSGPWorldMap* WorldEditorRenderInterface::GetWorldMap()
{
	return m_pRender->m_pRenderDevice->GetWorldSystemManager()->getWorldMap();
}

ISGPWorldSystemManager* WorldEditorRenderInterface::GetWorldSystemManager()
{
	return m_pRender->m_pRenderDevice->GetWorldSystemManager();
}

ISGPRenderDevice* WorldEditorRenderInterface::GetRenderDevice()
{
	return m_pRender->m_pRenderDevice;
}

SGPDevice* WorldEditorRenderInterface::GetDevice()
{
	return m_pRender->m_pDevice;
}

bool WorldEditorRenderInterface::IsIntersectTerrain()
{
	return m_pRender->m_bIntersectTerrain;
}

int WorldEditorRenderInterface::GetCurrPosChunkId()
{
	if(!IsIntersectTerrain()) return -1;
	float x,z;
	WorldEditorRenderInterface::GetInstance()->GetIntersectPoint(x,z);
	uint32 chunkId = ((CSGPTerrain*)(WorldEditorRenderInterface::GetInstance()->GetWorldSystemManager()->getTerrain()))->GetChunkIndex(x,z);
	if(chunkId!=0xFFFFFFFF) return (int)chunkId;
	else return -1;
}

int WorldEditorRenderInterface::GetCurrPosCellId()
{
	if(!IsIntersectTerrain()) return -1;
	int chunkSize=GetChunkSize();
	float point_x,point_z;
	GetIntersectPoint(point_x,point_z);
	int x=(int)(point_x/SGPTT_TILE_METER);
	int z=chunkSize*SGPTT_TILENUM-1-(int)(point_z/SGPTT_TILE_METER);

	if(x>=0&&x<chunkSize*SGPTT_TILENUM&&z>=0&&z<chunkSize*SGPTT_TILENUM)
		return (chunkSize*SGPTT_TILENUM+1)*z+x;
	else return -1;
}

void WorldEditorRenderInterface::RenderBoxSelectArea(int left,int top,int right,int bottom,float r,float g,float b)
{
	m_pRender->RenderBoxSelectArea(left,top,right,bottom,r,g,b);
}

bool WorldEditorRenderInterface::GetCurrPosObject(int pos_x,int pos_y,CommonObject& obj)
{
	Vector4D vcOrig,vcDir;
	m_pRender->m_pRenderDevice->Transform2Dto3D(SDimension2D(pos_x,pos_y),&vcOrig,&vcDir);
	Ray mouseRay;
	mouseRay.m_vcOrig.x=vcOrig.x;
	mouseRay.m_vcOrig.y=vcOrig.y;
	mouseRay.m_vcOrig.z=vcOrig.z;
	mouseRay.m_vcDir.x=vcDir.x;
	mouseRay.m_vcDir.y=vcDir.y;
	mouseRay.m_vcDir.z=vcDir.z;

	COpenGLWorldSystemManager* pWorldSystemManager=(COpenGLWorldSystemManager*)GetWorldSystemManager();
	Array<ISGPObject*>& objArray=pWorldSystemManager->getVisibleObjectArray();

	bool bFind=false;
	float mindistance=(float)INT_MAX;
	float distance;
	for(int i=0;i<objArray.size();++i)
	{
		ISGPObject* pObj=objArray[i];
		AABBox boundingBox;
		boundingBox.Construct(&(pObj->getBoundingBox()));
		if(mouseRay.Intersects(boundingBox,&distance))
		{
			CStaticMeshInstance* pMeshInstance=GetWorldSystemManager()->getMeshInstanceBySceneID(pObj->getSceneObjectID());
			Matrix4x4 modelMatrix = pMeshInstance->getModelMatrix();
			CMF1FileResource *pMF1Res = m_pRender->m_pRenderDevice->GetModelManager()->getModelByID(pMeshInstance->getMF1ModelResourceID());
			CSGPModelMF1 *pMF1Model = (pMF1Res != NULL) ? pMF1Res->pModelMF1 : NULL;
			ASSERT(pMF1Model!=NULL);
			for( uint32 i=0; i<pMF1Model->m_Header.m_iNumMeshes; i++ )
			{
				for( uint32 j=0; j<pMF1Model->m_pLOD0Meshes[i].m_iNumIndices; j += 3 )
				{
					Vector3D v0(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j]].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j]].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j]].vPos[2] );
					Vector3D v1(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+1]].vPos[2] );
					Vector3D v2(
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]].vPos[0],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]].vPos[1],
						pMF1Model->m_pLOD0Meshes[i].m_pVertex[pMF1Model->m_pLOD0Meshes[i].m_pIndices[j+2]].vPos[2] );

					v0 = v0 * modelMatrix;
					v1 = v1 * modelMatrix;
					v2 = v2 * modelMatrix;

					if(mouseRay.Intersects(v0,v1,v2,false,&distance))
					{
						if(distance<mindistance)
						{
							obj.m_type=ObjType_MF1;
							obj.m_pObj=pObj;
							mindistance=distance;
							bFind=true;
						}
					}
					else if(mouseRay.Intersects(v1,v0,v2,false,&distance))
					{
						if(distance<mindistance)
						{
							obj.m_type=ObjType_MF1;
							obj.m_pObj=pObj;
							mindistance=distance;
							bFind=true;
						}
					}
				}
			}
		}
	}

	std::vector<ISGPLightObject*>& lightVector=WorldMapManager::GetInstance()->m_LightObjVector;
	for(uint32 i=0;i<lightVector.size();++i)
	{
		Vector3D center(lightVector[i]->m_fPosition[0],lightVector[i]->m_fPosition[1],lightVector[i]->m_fPosition[2]);
		if(RayIntersectSphere(mouseRay.m_vcOrig,mouseRay.m_vcDir,center,lightVector[i]->m_fLightSize,distance))
		{
			if(distance<mindistance)
			{
				obj.m_type=ObjType_Light;
				obj.m_pLightObj=lightVector[i];
				mindistance=distance;
				bFind=true;
			}
		}
	}

	return bFind;
}

void WorldEditorRenderInterface::RenderOBBox(const OBBox& obbox,int r,int g,int b)
{
	m_pRender->m_pRenderDevice->GetVertexCacheManager()->RenderBox(obbox,Colour(r,g,b,1.0f));
}

void WorldEditorRenderInterface::RenderAABBox(const AABBox& aabbox,int r,int g,int b)
{
	m_pRender->m_pRenderDevice->GetVertexCacheManager()->RenderBox(aabbox,Colour(r,g,b,1.0f));
}

void WorldEditorRenderInterface::RenderAxis(EditorExis& xAxis,EditorExis& yAxis,EditorExis& zAxis,EditorExis* selectedAxis)
{
	SGPVertex_UPOS_VERTEXCOLOR vertex[6];
	Vector3D startpos,endpos;
	startpos=xAxis.Get3DStart();
	endpos=xAxis.Get3DEnd();
	m_pRender->SetVertexPos(vertex[0],startpos.x,startpos.y,startpos.z);
	if(selectedAxis!=&xAxis) m_pRender->SetVertexColor(vertex[0],1.0f,0.0f,0.0f);
	else m_pRender->SetVertexColor(vertex[0],1.0f,1.0f,0.0f);
	m_pRender->SetVertexPos(vertex[1],endpos.x,endpos.y,endpos.z);
	if(selectedAxis!=&xAxis) m_pRender->SetVertexColor(vertex[1],1.0f,0.0f,0.0f);
	else m_pRender->SetVertexColor(vertex[1],1.0f,1.0f,0.0f);

	startpos=yAxis.Get3DStart();
	endpos=yAxis.Get3DEnd();
	m_pRender->SetVertexPos(vertex[2],startpos.x,startpos.y,startpos.z);
	if(selectedAxis!=&yAxis) m_pRender->SetVertexColor(vertex[2],0.0f,1.0f,0.0f);
	else m_pRender->SetVertexColor(vertex[2],1.0f,1.0f,0.0f);
	m_pRender->SetVertexPos(vertex[3],endpos.x,endpos.y,endpos.z);
	if(selectedAxis!=&yAxis) m_pRender->SetVertexColor(vertex[3],0.0f,1.0f,0.0f);
	else m_pRender->SetVertexColor(vertex[3],1.0f,1.0f,0.0f);

	startpos=zAxis.Get3DStart();
	endpos=zAxis.Get3DEnd();
	m_pRender->SetVertexPos(vertex[4],startpos.x,startpos.y,startpos.z);
	if(selectedAxis!=&zAxis) m_pRender->SetVertexColor(vertex[4],0.0f,0.0f,1.0f);
	else m_pRender->SetVertexColor(vertex[4],1.0f,1.0f,0.0f);
	m_pRender->SetVertexPos(vertex[5],endpos.x,endpos.y,endpos.z);
	if(selectedAxis!=&zAxis) m_pRender->SetVertexColor(vertex[5],0.0f,0.0f,1.0f);
	else m_pRender->SetVertexColor(vertex[5],1.0f,1.0f,0.0f);

	m_pRender->m_pRenderDevice->GetVertexCacheManager()->RenderLines(6,vertex,false);
}

void WorldEditorRenderInterface::RenderLine(const Vector3D& ptStart,const Vector3D& ptEnd,float r,float g,float b)
{
	SGPVertex_UPOS_VERTEXCOLOR vertex[2];
	m_pRender->SetVertexPos(vertex[0],ptStart.x,ptStart.y,ptStart.z);
	m_pRender->SetVertexColor(vertex[0],r,g,b);
	m_pRender->SetVertexPos(vertex[1],ptEnd.x,ptEnd.y,ptEnd.z);
	m_pRender->SetVertexColor(vertex[1],r,g,b);
	m_pRender->m_pRenderDevice->GetVertexCacheManager()->RenderLines(2,vertex,false);
}

void WorldEditorRenderInterface::GetRenderWndSize(int& width,int& height)
{
	width = m_pRender->m_WndWidth;
	height = m_pRender->m_WndHeight;
}

void WorldEditorRenderInterface::FocusObject(const CommonObject& obj)
{
	m_pRender->m_pCamera->FocusObject(obj);
}

void WorldEditorRenderInterface::SetCameraFov(float fFov)
{
	m_pRender->m_pRenderDevice->setFov(fFov);
	m_pRender->m_pRenderDevice->onResize(m_pRender->m_WndWidth,m_pRender->m_WndHeight);
}

void WorldEditorRenderInterface::SetCameraNearPlane(float fNear)
{
	float fFar = ((COpenGLRenderDevice*)m_pRender->m_pRenderDevice)->getOpenGLCamera()->m_fFar;
	m_pRender->m_pRenderDevice->setNearFarClipPlane(fNear,fFar);
}

void WorldEditorRenderInterface::SetCameraFarPlane(float fFar)
{
	float fNear = ((COpenGLRenderDevice*)m_pRender->m_pRenderDevice)->getOpenGLCamera()->m_fNear;
	m_pRender->m_pRenderDevice->setNearFarClipPlane(fNear,fFar);
}
