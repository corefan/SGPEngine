#pragma once

/*#include "ModelViewerConfig.h"
typedef void (ModelViewerConfig::*Func)(int,BOOL);*/
enum CheckShowType
{
	Check_ShowMesh,
	Check_ShowParticle,
	Check_ShowBoundBox,
	Check_None
};

class CPropertyGridCheckProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CPropertyGridCheckProperty)
public:
	CPropertyGridCheckProperty(const CString& strName, const COleVariant& varValue,CheckShowType type,int index);
	virtual ~CPropertyGridCheckProperty();
public:
	// override
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL PushChar(UINT nChar);
protected:
	BOOL m_bChecked;
public:
	void SetCheck(BOOL bCheck);
	void Check();
	DWORD m_dwStart;
	CheckShowType m_Type;
//	Func m_ResponseFunc;
	BOOL m_Index;
};
