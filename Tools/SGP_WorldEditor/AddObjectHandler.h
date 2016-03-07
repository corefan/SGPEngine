#pragma once

#include "MouseMsgHandler.h"

namespace sgp
{
	struct ISGPObject;
}

class AddObjOperation;

class AddObjectHandler:public MouseMsgHandler
{
	AddObjectHandler();
	virtual ~AddObjectHandler();
	DECLARE_SINGLETON(AddObjectHandler)
public:
	virtual void LButtonDown(CPoint pt,UINT nFlags);
	virtual void LButtonUp(CPoint pt,UINT nFlags){}
	virtual void MouseMove(CPoint pt,UINT nFlags);
	virtual void DrawArea(){}
	virtual void RButtonDown(CPoint pt,UINT nFlags){}
	virtual void RButtonUp(CPoint pt,UINT nFlags);
	virtual void CheckUncompleteOp();
public:
	sgp::ISGPObject* m_pObj;
	bool m_bBeginAdd;
	CString m_MF1Path;
	float m_fScale;
	AddObjOperation* m_pAddObjOp;
public:
	void AddSceneObject();
	void SetObjectAttribute(CString absolutePath,float fScale);
	void BeginAdd();
	void EndAdd(){m_bBeginAdd=false;}
};