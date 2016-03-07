#pragma once

#include "..\SGPLibraryCode\SGPHeader.h"

class EditorExis
{
public:
	EditorExis();
	~EditorExis();
	void Set3DStart(Vector3D start){m_StartPos=start;}
	void SetDirection(Vector3D dir){m_Dir=dir;}
	void SetLength(float length){m_Length=length;}
public:
	Vector2D Get2DStart();
	Vector2D Get2DEnd();
	Vector3D Get3DStart(){return m_StartPos;}
	Vector3D Get3DEnd(){return m_StartPos+m_Dir*m_Length;}
	Vector3D GetDir(){return m_Dir;}
	float GetLength(){return m_Length;}
//	Vector2D Transform3DTo2D(Vector3D& origPos);
private:
	Vector3D m_StartPos;
	Vector3D m_Dir;
	float m_Length;
};