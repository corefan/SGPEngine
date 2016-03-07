#pragma once


// CGrassListCtrl

class CGrassListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGrassListCtrl)

public:
	CGrassListCtrl();
	virtual ~CGrassListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	void LoadFromGrassTexture();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
