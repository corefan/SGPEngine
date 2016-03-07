#pragma once

#include "DragHandler.h"
class ObjAttributeOperation;

class DragRotateHandler:public DragHandler
{
	DragRotateHandler();
	~DragRotateHandler();
	DECLARE_SINGLETON(DragRotateHandler)
public:
	virtual void BeginDrag(CPoint pt);
	virtual void DragMove(CPoint pt);
	virtual void EndDrag(CPoint pt);
	void SetAxis(int index){m_nAxisIndex=index;}
	void SetDir(float x,float y){m_DirX=x;m_DirY=y;}
private:
	int m_nAxisIndex;
	float m_DirX;
	float m_DirY;
	CPoint m_LastPtPos;
	ObjAttributeOperation* m_pObjAttrOp;
};