#pragma once

// CAnimProgressBar

class CAnimProgressBar : public CStatic
{
	DECLARE_DYNAMIC(CAnimProgressBar)

public:
	CAnimProgressBar();
	virtual ~CAnimProgressBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
public:
	void SetCurrPos(float fPos);
	void NotifyPosChanged();
private:
	BOOL m_bLBtnDown;
	int m_CurrPos;
};


