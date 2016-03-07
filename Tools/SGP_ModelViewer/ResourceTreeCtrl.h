#pragma once

// CResourceTreeCtrl

class CResourceTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CResourceTreeCtrl)

public:
	CResourceTreeCtrl();
	virtual ~CResourceTreeCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void SetPath(CString strPath);
	void Insert(CString path,HTREEITEM hItem=TVI_ROOT);
	HTREEITEM MyInsertItem(CString text,HTREEITEM hParent,BOOL bExpand,int index);
	void DeleteChild(HTREEITEM hItem);
	CString GetItemPath(HTREEITEM hItem);
public:
	CString m_DirPath;//root path
	CImageList m_ImageList;
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_PrevModel;
	CFont m_Font;
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMenuOpenFolder();

	static void OpenMF1File(CString szPath,CString strName);
};