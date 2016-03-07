#pragma once

// CObjectTreeCtrl

class CObjectTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CObjectTreeCtrl)

public:
	CObjectTreeCtrl();
	virtual ~CObjectTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenFolder();
public:
	void SetPath(CString strPath){m_DirPath=strPath;}
	HTREEITEM MyInsertItem(CString text,HTREEITEM hParent,BOOL bExpand,int index);
	void Insert(CString path,HTREEITEM hItem=TVI_ROOT);
	void Init();
	void DeleteChild(HTREEITEM hItem);
	CString GetItemPath(HTREEITEM hItem);
private:
	CString m_DirPath;
	CImageList m_ImageList;
public:
	HTREEITEM m_SelectedItem;
	CString GetMF1FilePath();
};
