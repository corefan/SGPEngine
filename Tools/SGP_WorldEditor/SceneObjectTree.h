#pragma once

#include <vector>
#include "CommonObjDef.h"

// CSceneObjectTree

class CSceneObjectTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CSceneObjectTree)
	static CSceneObjectTree* s_pSceneObjTree;
public:
	CSceneObjectTree();
	virtual ~CSceneObjectTree();
	static CSceneObjectTree* GetInstance(){return s_pSceneObjTree;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CFont m_Font;
	HTREEITEM m_RootItem;
	HTREEITEM m_StaticMeshItem;
	HTREEITEM m_LightItem;
	HTREEITEM m_hItemFirstSel;
	CImageList m_ImageList;
public:
	void Init();
	void AddSceneObject(const CommonObject& obj);
	void DeleteSceneObject(const CommonObject& obj);
	void SceneObjNameChanged(const CommonObject& obj);
	HTREEITEM ItemFromObject(const CommonObject& obj);
	CommonObject ObjectFromItem(HTREEITEM hItem);
	void ClearSelection();
	void ClearSelection(HTREEITEM hItem);
	BOOL SelectItems(HTREEITEM hItemFrom,HTREEITEM hItemTo);
	HTREEITEM GetFirstSelectedItem();
	void SelectedObjectsChanged(const std::vector<CommonObject>& objVector);
	void NotifyRetrieveSelectedItems();
	bool CanTrackPopupMenu(CPoint pt,bool& bHaveMF1,bool& bHaveLight,std::vector<CommonObject>& objVector);

	void ResetAllItems();
	bool SearchObject(CString strKeyWord);
	bool SearchObject(int id);

	void FocusObject(const CommonObject& obj);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


