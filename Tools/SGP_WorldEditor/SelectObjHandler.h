#pragma once

#include "MouseMsgHandler.h"
#include <vector>
#include "EditorAxis.h"
#include "CommonObjDef.h"

namespace sgp
{
	struct ISGPObject;
	class Vector3D;
}

enum DragObjMode
{
	Drag_None,
	Drag_Move,
	Drag_Rotate,
	Drag_Scale
};

class DragHandler;

class SelectObjHandler:public MouseMsgHandler
{
	SelectObjHandler();
	~SelectObjHandler();
	DECLARE_SINGLETON(SelectObjHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea();
	virtual void DrawFlag();
	virtual void CheckUncompleteOp();
	virtual bool SetCursor();
	bool InLine(float x0,float y0,float x1,float y1,float x,float y);
	void GetCurrAxis(CPoint pt);

	void SetPos(float x,float y,float z);
	void SetRotate(float x,float y,float z);
	void SetScale(float fScale);
	void SetAlpha(float fAlpha);
	void SetConfigSetting(int index);


	void SetDragMode(DragObjMode mode){m_DragMode=mode;}
	DragObjMode GetDragMode(){return m_DragMode;}

	std::vector<CommonObject>& GetSelectedObj(){return m_SelObjectVector;}
	int GetSelectedObjCount(){return m_SelObjectVector.size();}
	CommonObject& GetLastObj(){return m_LastObj;}
	void SetSelectedObjects(std::vector<CommonObject>& objVector);
	void SetLastObj(const CommonObject& obj);
	void ClearSelection();
	void SelectedObjectsChanged();

	void FocusObject(const CommonObject& obj);
public:
	bool m_bLBtnDown;
	CPoint m_BtnDownPt;
	int m_Left;
	int m_Top;
	int m_Right;
	int m_Bottom;
	CommonObject m_LastObj;
	std::vector<CommonObject> m_SelObjectVector;
	EditorExis* m_pCurrAxis;

	DragHandler* m_pDragHandler;
	bool m_bInDragMode;
	DragObjMode m_DragMode;

	float m_DirX;
	float m_DirY;
	float m_fRatio;

	EditorExis m_XAxis;
	EditorExis m_YAxis;
	EditorExis m_ZAxis;
};