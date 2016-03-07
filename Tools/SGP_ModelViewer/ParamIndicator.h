#pragma once


// CParamIndicator

class CParamIndicator : public CStatic
{
	DECLARE_DYNAMIC(CParamIndicator)

public:
	CParamIndicator();
	virtual ~CParamIndicator();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
public:
	float m_CurPos;
	BOOL m_bLButtonDown;
};
