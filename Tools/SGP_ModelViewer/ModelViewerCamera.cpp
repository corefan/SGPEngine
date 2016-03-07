#include "../SGPLibraryCode/SGPHeader.h"//engine header
#include "ModelViewerCamera.h"

ModelViewerCamera::ModelViewerCamera()
{
	CMovementController::Init();
	m_fZoom = 15.0f;
	m_fPanX = m_fPanY = 0;
	m_vLookAt.Set(0, 0, 0);
}

void ModelViewerCamera::Update(float fElapsedTime)
{
	sgp::Vector4D vcZoom, vcPan, VcNewLookAt;

	// add rotation speed
	m_fRotX += (m_fPitchSpeed * fElapsedTime);
	m_fRotY += (m_fYawSpeed   * fElapsedTime);
	m_fRotZ += (m_fRollSpeed  * fElapsedTime);

	// recalculate axes
	RecalcAxes();


	sgp::Vector4D VcPos(GetPos());
	sgp::Vector4D VcDir(GetDir());
	sgp::Vector4D VcUp(GetUp());
	sgp::Vector4D VcRight(GetRight());

/*	vcPan = VcRight * m_fPanX + VcUp * m_fPanY;
	m_vLookAt += vcPan;

	if( m_fZoom < 0.01f )
		m_fZoom = 0.01f;
	VcPos = m_vLookAt - VcDir * m_fZoom;

	// move position according to velocity vector
	m_vcPos.Set( VcPos.x, VcPos.y, VcPos.z );*/
	m_vcPos = m_vLookAt - VcDir * m_fZoom;
}

void ModelViewerCamera::Move(float a,float b)
{
	sgp::Vector4D VcRight(GetRight());
	sgp::Vector4D VcUp(GetUp());
	sgp::Vector4D vcPan;
	vcPan=VcRight*a+VcUp*b;
	m_vLookAt += vcPan;
}

void ModelViewerCamera::Rotate(float x,float y)
{
	m_fRotX+=x;
	m_fRotY+=y;
}

// set attributes directly (avoid this except for initialization)
void ModelViewerCamera::SetRotation(float rx, float ry, float rz)
{
	m_fRotX = rx;
	m_fRotY = ry;
	m_fRotZ = rz;

	RecalcAxes();
}

void ModelViewerCamera::RecalcAxes(void)
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
}