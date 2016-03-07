#pragma once

#include "MouseMsgHandler.h"

class AddObjOperation;

namespace sgp
{
	struct ISGPLightObject;
}

class AddLightObjHandler:public MouseMsgHandler
{
	AddLightObjHandler();
	~AddLightObjHandler();
	DECLARE_SINGLETON(AddLightObjHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags);
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea(){}
	virtual void CheckUncompleteOp();
public:
	sgp::ISGPLightObject* m_pLightObj;
	bool m_bBeginAdd;
	void BeginAdd();
	void EndAdd(){m_bBeginAdd=false;}
	void AddLightObject();
	AddObjOperation* m_pAddObjOp;
};