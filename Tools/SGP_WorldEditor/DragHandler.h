#pragma once

class DragHandler
{
public:
	DragHandler();
	~DragHandler();
public:
	virtual void BeginDrag(CPoint pt)=0;
	virtual void DragMove(CPoint pt)=0;
	virtual void EndDrag(CPoint pt)=0;
};