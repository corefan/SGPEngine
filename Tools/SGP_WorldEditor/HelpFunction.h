#pragma once

#include "../SGPLibraryCode/SGPHeader.h"
#include <vector>
#include "CommonObjDef.h"

//Vector3D UnProject(Vector3D &screenPos,Matrix4x4& modelView,Matrix4x4& project,Vector4D viewport);
//bool IntersectTriangle(const Vector3D& orig,const Vector3D& dir,const Vector3D& v0,Vector3D& v1,Vector3D& v2,Vector3D& result);

CString GetRelativePath(CString absolutePath,CString workDir);
bool FileInFolder(CString filePath,CString dirPath);
bool operator!=(const SGPGrassCluster& cluster1,const SGPGrassCluster& cluster2);

Vector2D Transform3DTo2D(Vector3D &origPos);

bool RayIntersectSphere(Vector3D& origin,Vector3D& dir,Vector3D& center,float radius,float& distance);

void CopyObject(ISGPObject* pDest,ISGPObject* pSource);
void CopyObject(ISGPLightObject* pDest,ISGPLightObject* pSource);

BOOL ContainMF1(const std::vector<CommonObject>& objVector);
uint32 GetMF1Count(const std::vector<CommonObject>& objVector);

void BuildFrustum(Frustum& frustum,int left,int right,int top,int bottom);

bool BoxInRect(const OBBox& obbox,int left,int right,int top,int bottom);

void GetObjectOBBox(const CommonObject& obj,OBBox& obbox);
void GetObjectAABox(const CommonObject& obj,AABBox& aabbox);

bool operator==(const SGPWorldMapChunkTextureNameTag& data1,const SGPWorldMapChunkTextureNameTag& data2);

BOOL CheckCreateDirectory(CString strPath);

CString FileTitleFromPath(CString strPath);
CString FileFolderFromPath(CString strPath);

void ReversePathSeparator(CString & strPath);

bool ContainString(std::vector<CString>& stringVec,CString str);
