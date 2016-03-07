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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	CFont m_Font;
};
