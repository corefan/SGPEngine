#include "../SGPLibraryCode/SGPHeader.h"//engine header

#include "WorldEditorCamera.h"

WorldEditorCamera::WorldEditorCamera()
{
	CMovementController::Init();
	m_fZoom = 5.0f;
	m_fPanX = m_fPanY = 0;
	m_vLookAt.Set(0, 0, 0);

	m_Matrix.Identity();
}

void WorldEditorCamera::Update(float fElapsedTime)
{
	sgp::Vector4D vcZoom, vcPan, VcNewLookAt;

	// recalculate axes
	RecalcAxes();

	sgp::Vector4D VcPos(GetPos());
	sgp::Vector4D VcDir(GetDir());
	sgp::Vector4D VcUp(GetUp());
	sgp::Vector4D VcRight(GetRight());

	VcPos+=VcDir*m_fZoom;
	vcPan=VcRight*m_fPanX+VcUp*m_fPanY;
	VcPos+=vcPan;
//	m_vLookAt=VcPos+VcDir;

	m_vcPos.Set(VcPos.x,VcPos.y,VcPos.z);

	m_fZoom=0.0f;
	m_fPanX=0.0f;
	m_fPanY=0.0f;
	//m_fRotX=0.0f;
	//m_fRotY=0.0f;
//	m_vcPos = m_vLookAt - VcDir * m_fZoom;
}

void WorldEditorCamera::Zoom(float a)
{
//	m_fZoom += a;
	m_fZoom=a;
}

void WorldEditorCamera::Move(float a,float b)
{
/*	sgp::Vector4D vcPan;
	vcPan=m_vcRight*a+m_vcUp*b;
	m_vLookAt += vcPan;*/
	m_fPanX=a;
	m_fPanY=b;
}

void WorldEditorCamera::Rotate(float x,float y)
{
	m_fRotX+=x;
	m_fRotY+=y;
	
	Vector4D vDir=m_vLookAt-m_vcPos;

	Matrix4x4 mat;
	mat.RotationY(y);
	vDir.RotateWith(mat);
	mat.RotationAxis(m_vcRight,x);
	vDir.RotateWith(mat);
	
	m_vcPos=m_vLookAt-vDir;
}

// set attributes directly (avoid this except for initialization)
void WorldEditorCamera::SetRotation(float rx, float ry, float rz)
{
	m_fRotX = rx;
	m_fRotY = ry;
	m_fRotZ = rz;

	RecalcAxes();
}

void WorldEditorCamera::RecalcAxes(void)
{
	sgp::Matrix4x4  mat;

	// keep in range of 360 degree
	if(m_fRotY > float_2pi) 
		m_fRotY -= float_2pi;
	else if(m_fRotY < -float_2pi) 
		m_fRotY += float_2pi;

	// dont look up/down more than 80 degrees
	if(m_fRotX > 1.4f) 
		m_fRotX = 1.4f;
	else if(m_fRotX < -1.4f) 
		m_fRotX = -1.4f;

	// initialize axes
	m_vcRight.Set(1.0f, 0.0f, 0.0f);
	m_vcUp.Set(0.0f, 1.0f, 0.0f);
	m_vcDir.Set(0.0f, 0.0f, 1.0f);

	sgp::Vector4D VcRight(GetRight());
	sgp::Vector4D VcDir(GetDir());
	sgp::Vector4D VcUp(GetUp());

	// rotate around Y axis
	mat.RotationY(m_fRotY);
	VcRight.RotateWith(mat);
	VcDir.RotateWith(mat);

	// rotate around X axis
	mat.RotationAxis(VcRight, m_fRotX);
	VcUp.RotateWith(mat);
	VcDir.RotateWith(mat);

	// fight rounding errors
	VcDir.Normalize();
	VcRight.Cross(VcUp, VcDir);
	VcRight.Normalize();
	VcUp.Cross(VcDir, VcRight);
	VcUp.Normalize();

	m_vcRight.Set(VcRight.x, VcRight.y, VcRight.z);
	m_vcUp.Set(VcUp.x, VcUp.y, VcUp.z);
	m_vcDir.Set(VcDir.x, VcDir.y, VcDir.z);

/*	Matrix4x4 mat1;
	Matrix4x4 mat2;
	mat1.RotationY(m_fRotX);
	mat2.RotationX(m_fRotY);
	Matrix4x4 matTest=mat1*mat2;
	m_vcRight=m_vcRight*matTest;
	m_vcUp=m_vcUp*matTest;
	m_vcDir=m_vcDir*matTest;*/
/*	Matrix4x4 matTemp;
	Quaternion qRotation;
	qRotation.MakeFromEuler(m_fRotX,-m_fRotY,0);
	qRotation.GetMatrix(&matTemp);
	m_vcRight=m_vcRight*matTemp;
	m_vcUp=m_vcUp*matTemp;
	m_vcDir=m_vcDir*matTemp;*/
/*	Matrix4x4 mat=m_Matrix;
	Matrix4x4 mat1;
	Matrix4x4 mat2;
	mat1.RotationY(m_fRotY);
	mat2.RotationAxis(Vector4D(mat._11,mat._12,mat._13,1.0f),-m_fRotX);
	Matrix4x4 matTmp=mat1*mat2;
	mat=mat*matTmp;
	m_Matrix=mat;

	m_vcRight=m_vcRight*m_Matrix;
	m_vcUp=m_vcUp*m_Matrix;
	m_vcDir=m_vcDir*m_Matrix;*/
}

void WorldEditorCamera::FocusObject(const CommonObject& obj)
{
	AABBox aabbox;
	GetObjectAABox(obj,aabbox);
	Vector3D vec=aabbox.vcMax-aabbox.vcMin;
	float length=vec.GetLength();
	length+=10.0f;
	Vector3D center=aabbox.vcCenter;
	center-=Vector3D(m_vcDir.x,m_vcDir.y,m_vcDir.z)*length;
	m_vcPos.Set(center.x,center.y,center.z);
}