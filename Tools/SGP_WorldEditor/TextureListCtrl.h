#pragma once


// CTextureListCtrl

class CTextureListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTextureListCtrl)

public:
	CTextureListCtrl();
	virtual ~CTextureListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowRelatedChunk();
private:
	BOOL m_bDraging;
	int m_nSelectedItem;
	BOOL m_bHaveItem;
};
