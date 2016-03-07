#pragma once

namespace sgp
{
	struct ISGPObject;
	struct ISGPLightObject;
}

#include "CommonObjDef.h"
#include <vector>

class CAttributeGridProperty;
class CAttributeGroupGridProperty;
// CObjAttributePane

class CObjAttributePane : public CCommonDockablePane
{
	DECLARE_DYNAMIC(CObjAttributePane)
	static CObjAttributePane* s_pAttributePane;
public:
	CObjAttributePane();
	virtual ~CObjAttributePane();
	static CObjAttributePane* GetInstance(){ASSERT(s_pAttributePane!=NULL);return s_pAttributePane;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CMFCPropertyGridCtrl m_DetailList;
	CFont m_Font;
	EditorObjectType m_ObjectType;
	CAttributeGridProperty* m_PositionProperty[3];
	CAttributeGridProperty* m_RotationProperty[3];
	CAttributeGridProperty* m_ScaleProperty;
	CAttributeGroupGridProperty* m_PositionGroup;
	CAttributeGroupGridProperty* m_RotationGroup;
public:
	void LoadAttribute(const CommonObject& obj);
	void LoadAttribute(const sgp::ISGPObject* pObj);
	void LoadAttribute(const sgp::ISGPLightObject* pLightObj);
	afx_msg LRESULT OnAttributeChanged(WPARAM wParam,LPARAM lParam);
	void DealLightAttrChanged(CMFCPropertyGridProperty* pProperty);
	void DealObjAttrChanged(CMFCPropertyGridProperty* pProperty);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	void NotifyAttributeChanged(const std::vector<CommonObject>& objVector);
	void NotifyAttributeChanged(const CommonObject& obj);
	void NotifyPositionChanged(const std::vector<CommonObject>& objVector);
	void NotifyRotationChanged(const std::vector<CommonObject>& objVector);
	void NotifyScaleChanged(const std::vector<CommonObject>& objVector);
};
