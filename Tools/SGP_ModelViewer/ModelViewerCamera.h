#pragma once

class ModelViewerCamera : public sgp::CMovementController 
{
public:
	ModelViewerCamera(void);
	virtual ~ModelViewerCamera(void) {}

	virtual void Update(float fElapsedTime);

	void Move(float x,float y);
	void Rotate(float x,float y);
	void Zoom(float a) { m_fZoom += a; }

	void SetRotationSpeedX(float f) { m_fPitchSpeed = f; }
	void SetRotationSpeedY(float f) { m_fYawSpeed   = f; }
	void SetPanX(float a)			{ m_fPanX		= a; }
	void SetPanY(float a)			{ m_fPanY		= a; }

	// set attributes directly (avoid this except for initialization)
	void SetRotation(float rx, float ry, float rz);
	void SetPos(float x, float y, float z)   { m_vcPos.x = x; m_vcPos.y = y; m_vcPos.z = z; }
	void SetLookAt(float x, float y, float z) { m_vLookAt.Set(x,y,z); }

private:
	float			m_fZoom;
	float			m_fPanX;
	float			m_fPanY;
	sgp::Vector4D	m_vLookAt;

	void RecalcAxes(void);
};
