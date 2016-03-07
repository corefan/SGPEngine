#pragma once

#include "DragHandler.h"

class EditorExis;
class ObjAttributeOperation;

class DragScaleHandler:public DragHandler
{
	DragScaleHandler();
	~DragScaleHandler();
	DECLARE_SINGLETON(DragScaleHandler)
public:
	virtual void BeginDrag(CPoint pt);
	virtual void DragMove(CPoint pt);
	virtual void EndDrag(CPoint pt);
	void SetAxis(EditorExis* axis){m_pAxis=axis;}
private:
	CPoint m_LastPtPos;
	EditorExis* m_pAxis;
	ObjAttributeOperation* m_pObjAttrOp;
};