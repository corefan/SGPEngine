#pragma once

#include "DragHandler.h"

class EditorExis;
class ObjAttributeOperation;

class DragMoveHandler:public DragHandler
{
	DragMoveHandler();
	~DragMoveHandler();
	DECLARE_SINGLETON(DragMoveHandler)
public:
	virtual void BeginDrag(CPoint pt);
	virtual void DragMove(CPoint pt);
	virtual void EndDrag(CPoint pt);
	void SetAxis(EditorExis* axis);
private:
	CPoint m_LastPtPos;
	EditorExis* m_pAxis;
	ObjAttributeOperation* m_pObjAttrOp;
};