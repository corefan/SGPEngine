#include "stdafx.h"
#include "WorldEditor.h"
#include "HelpFunction.h"
#include "MainFrame.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"
#include <algorithm>

CString GetRelativePath(CString absolutePath,CString workDir)
{
	char relativePath[MAX_PATH];
	if(!PathRelativePathTo(relativePath,(LPCTSTR)workDir,FILE_ATTRIBUTE_DIRECTORY,(LPCTSTR)absolutePath,0))
		ASSERT(FALSE);
	//	return FALSE;
	CString result = relativePath;
	if(result.Left(2) == _T(".\\"))
		result=result.Right(result.GetLength()-2);
	return result;
}

bool FileInFolder(CString filePath,CString dirPath)
{
	if(filePath.GetLength() <= dirPath.GetLength()) return false;
	if(filePath.Left(dirPath.GetLength()) == dirPath) return true;
	return false;
}

bool operator!=(const SGPGrassCluster& cluster1,const SGPGrassCluster& cluster2)
{
	if(cluster1.fPositionX!=cluster2.fPositionX || cluster1.fPositionY!=cluster2.fPositionY ||\
		cluster1.fPositionZ!=cluster2.fPositionZ || cluster1.nData!=cluster2.nData || \
		cluster1.nPackedNormal!=cluster2.nPackedNormal)
		return true;
	else
		return false;
}

Vector2D Transform3DTo2D(Vector3D &origPos)
{
	Matrix4x4 matView,matProj;
	WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->getViewMatrix(matView);
	WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->getProjMatrix(matProj);
	Vector4D pos(origPos.x,origPos.y,origPos.z);
	pos=pos*matView*matProj;
	pos.x/=pos.w;
	pos.y/=pos.w;
	pos.z/=pos.w;

	CRect rc;
	CMainFrame* pMainFrame=(CMainFrame*)AfxGetMainWnd();
	pMainFrame->GetActiveView()->GetClientRect(rc);

	Vector2D result;
	result.x=(pos.x+1)*rc.Width()/2;
	result.y=(1-pos.y)*rc.Height()/2;

	return result;
}

bool RayIntersectSphere(Vector3D& origin,Vector3D& dir,Vector3D& center,float radius,float& distance)
{
	Vector3D dirToSphere=center-origin;
	dir.Normalize();
	float dot=dir*dirToSphere;
	float length=dirToSphere.GetLength();
	Vector3D closestPt=origin+dir*dot;
	float closestDis=(closestPt-center).GetLength();

	if(closestDis>radius) return false;

	distance=dot-sqrtf(radius*radius-closestDis*closestDis);

	return true;
}

void CopyObject(ISGPObject* pDest,ISGPObject* pSource)
{
	uint32 iObjectInChunkIndexNum=pDest->m_iObjectInChunkIndexNum;
	int32* pObjectInChunkIndex=pDest->m_pObjectInChunkIndex;

	memcpy(pDest,pSource,sizeof(*pSource));
	
	pDest->m_iObjectInChunkIndexNum=iObjectInChunkIndexNum;
	pDest->m_pObjectInChunkIndex=pObjectInChunkIndex;
}

void CopyObject(ISGPLightObject* pDest,ISGPLightObject* pSource)
{
	memcpy(pDest,pSource,sizeof(*pDest));
}

BOOL ContainMF1(const std::vector<CommonObject>& objVector)
{
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(objVector[i].IsMF1()) return true;
	}
	return false;
}

uint32 GetMF1Count(const std::vector<CommonObject>& objVector)
{
	uint32 count=0;
	for(uint32 i=0;i<objVector.size();++i)
	{
		if(objVector[i].IsMF1()) count++;
	}
	return count;
}

void BuildFrustum(Frustum& frustum,int left,int right,int top,int bottom)
{
	int width,height;
	WorldEditorRenderInterface::GetInstance()->GetRenderWndSize(width,height);
	int w = width/2;
	int h = height/2;
	float fLeft=(float)(left-w)/w;
	float fRight=(float)(right-w)/w;
	float fBottom=(float)(-top+h)/h;
	float fTop=(float)(-bottom+h)/h;

	Vector3D points[8];
	points[0].Set(fLeft,fBottom,0.0f);
	points[1].Set(fRight,fBottom,0.0f);
	points[2].Set(fLeft,fTop,0.0f);
	points[3].Set(fRight,fTop,0.0f);
	points[4].Set(fLeft,fBottom,1.0f);
	points[5].Set(fRight,fBottom,1.0f);
	points[6].Set(fLeft,fTop,1.0f);
	points[7].Set(fRight,fTop,1.0f);

	Matrix4x4 matViewProj;
	WorldEditorRenderInterface::GetInstance()->GetRenderDevice()->getViewProjMatrix(matViewProj);
	Matrix4x4 matInverse;
	matInverse.InverseOf(matViewProj);

	for(uint32 i=0;i<8;++i) points[i]=points[i]*matInverse;

	frustum.planes[Frustum::VF_NEAR_PLANE].Set(points[0],points[1],points[2]);
	frustum.planes[Frustum::VF_FAR_PLANE].Set(points[6],points[5],points[7]);
	frustum.planes[Frustum::VF_LEFT_PLANE].Set(points[2],points[6],points[4]);
	frustum.planes[Frustum::VF_RIGHT_PLANE].Set(points[7],points[3],points[5]);
	frustum.planes[Frustum::VF_BOTTOM_PLANE].Set(points[1],points[0],points[4]);
	frustum.planes[Frustum::VF_TOP_PLANE].Set(points[2],points[3],points[6]);
}

bool BoxInRect(const OBBox& obbox,int left,int right,int top,int bottom)
{
	Vector3D vec[8];
	Vector3D vcA0=obbox.vcA0*obbox.fA0;
	Vector3D vcA1=obbox.vcA0*obbox.fA1;
	Vector3D vcA2=obbox.vcA2*obbox.fA2;
	Vector3D center=obbox.vcCenter;

	vec[0]=center+vcA0-vcA1+vcA2;
	vec[1]=center+vcA0+vcA1-vcA2;
	vec[2]=center+vcA0-vcA1+vcA2;
	vec[3]=center+vcA0-vcA1-vcA2;
	vec[4]=center-vcA0+vcA1+vcA2;
	vec[5]=center-vcA0+vcA1-vcA2;
	vec[6]=center-vcA0-vcA1+vcA2;
	vec[7]=center-vcA0-vcA1-vcA2;

	int count=0;

	for(uint32 i=0;i<8;++i)
	{
		Vector2D pt=Transform3DTo2D(vec[i]);
		if(pt.x>=left&&pt.x<=right&&pt.y>=top&&pt.y<=bottom) count++;
	}
	if(count>=4) return true;
	else return false;
}

void GetObjectOBBox(const CommonObject& obj,OBBox& obbox)
{
	if(obj.IsMF1()) obbox=obj.m_pObj->m_ObjectOBBox;
	else if(obj.IsLight())
	{
		ISGPLightObject* pLightObj=obj.m_pLightObj;
		obbox.vcCenter.Set(pLightObj->m_fPosition[0],pLightObj->m_fPosition[1],pLightObj->m_fPosition[2]);
		obbox.vcA0.Set(1.0f,0.0f,0.0f);
		obbox.vcA1.Set(0.0f,1.0f,0.0f);
		obbox.vcA2.Set(0.0f,0.0f,1.0f);
		obbox.fA0=obbox.fA1=obbox.fA2=pLightObj->m_fLightSize;
	}
	else ASSERT(FALSE);
}

void GetObjectAABox(const CommonObject& obj,AABBox& aabbox)
{
	OBBox obbox;
	GetObjectOBBox(obj,obbox);
	aabbox.Construct(&obbox);
}

bool operator==(const SGPWorldMapChunkTextureNameTag& data1,const SGPWorldMapChunkTextureNameTag& data2)
{
	if(strcmp(data1.m_ChunkTextureFileName,data2.m_ChunkTextureFileName)==0) return true;
	else return false;
}

BOOL CheckCreateDirectory(CString strPath)
{
	if(PathIsDirectory(strPath)) return TRUE;

	int pos=strPath.ReverseFind('\\');
	if(pos==-1)	return PathIsDirectory(strPath);
	else
	{
		if(CheckCreateDirectory(strPath.Left(pos)))
		{
			if(!PathIsDirectory(strPath)) return CreateDirectory(strPath,NULL);
			else return TRUE;
		}
		else return FALSE;
	}
}

CString FileTitleFromPath(CString strPath)
{
	CString strName=PathFindFileName(strPath);
	int pos=strName.ReverseFind('.');
	if(pos==-1) return strName;
	else return strName.Left(pos);
}

CString FileFolderFromPath(CString strPath)
{
	int pos = strPath.ReverseFind('\\');
	if(pos!=-1) return strPath.Left(pos);
	else return CString("");
}

void ReversePathSeparator(CString &strPath)
{
	for(int i=0;i<strPath.GetLength();++i)
		if(strPath.GetAt(i)=='\\') strPath.SetAt(i,'/');
}

bool ContainString(std::vector<CString>& stringVec,CString str)
{
	std::vector<CString>::iterator iter=std::find(stringVec.begin(),stringVec.end(),str);
	return iter!=stringVec.end();
}