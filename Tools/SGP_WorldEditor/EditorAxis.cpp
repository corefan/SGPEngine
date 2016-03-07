#include "stdafx.h"

#include "WorldEditor.h"
#include "EditorAxis.h"
#include ".\Render Interface\WorldEditorRenderInterface.h"

#include "MainFrame.h"
#include "HelpFunction.h"

EditorExis::EditorExis()
{
	m_Length=10.0f;
}

EditorExis::~EditorExis()
{

}

Vector2D EditorExis::Get2DStart()
{
	return Transform3DTo2D(m_StartPos);
}

Vector2D EditorExis::Get2DEnd()
{
	return Transform3DTo2D(m_StartPos+m_Dir*m_Length);
}