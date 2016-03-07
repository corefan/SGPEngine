#pragma once


// CLogViewerEdit

class CLogViewerEdit : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CLogViewerEdit)

public:
	CLogViewerEdit();
	virtual ~CLogViewerEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CFont m_Font;
public:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};


