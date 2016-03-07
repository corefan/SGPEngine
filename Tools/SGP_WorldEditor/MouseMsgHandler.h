#pragma once

class MouseMsgHandler
{
public:
	MouseMsgHandler(void);
	virtual ~MouseMsgHandler(void);
	virtual void LButtonDown(CPoint pt,UINT nFlags)=0;
	virtual void LButtonUp(CPoint pt,UINT nFlags)=0;
	virtual void MouseMove(CPoint pt,UINT nFlags)=0;
	virtual void RButtonDown(CPoint pt,UINT nFlags){}
	virtual void RButtonUp(CPoint pt,UINT nFlags){}
	virtual void CheckUncompleteOp(){}
	virtual bool SetCursor(){return false;}

	virtual void DrawArea()=0;
	virtual void DrawFlag(){}
	virtual bool IsTerrainHandler(){return false;}
};
