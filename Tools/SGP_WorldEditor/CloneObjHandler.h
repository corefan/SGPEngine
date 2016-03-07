#pragma once

#include "CommonObjDef.h"
#include "MouseMsgHandler.h"
class AddObjOperation;

class CloneObjHandler : public MouseMsgHandler
{
	CloneObjHandler();
	~CloneObjHandler();
	DECLARE_SINGLETON(CloneObjHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags){}
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea(){}
	virtual void RButtonUp(CPoint pt,UINT nFlags);
	virtual void CheckUncompleteOp();
public:
	CommonObject m_OrigObj;
	bool m_bBeginAdd;
	CommonObject m_CurrObj;
	AddObjOperation* m_pAddObjOp;
public:
	void AddSceneObject();
	void BeginAdd(const CommonObject& obj);
	void EndAdd(){m_bBeginAdd=false;}
};