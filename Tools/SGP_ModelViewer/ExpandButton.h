#pragma once


// CExpandButton

class CExpandButton : public CButton
{
	DECLARE_DYNAMIC(CExpandButton)

public:
	CExpandButton();
	virtual ~CExpandButton();

protected:
	DECLARE_MESSAGE_MAP()
private:
	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	BOOL m_bExpanded;
	void SetExpand(BOOL bExpand);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


