#pragma once


// CFindResultList

class CFindResultList : public CListBox
{
	DECLARE_DYNAMIC(CFindResultList)
	static CFindResultList* s_pResultList;
public:
	CFindResultList();
	virtual ~CFindResultList();
	static CFindResultList* GetInstance(){ASSERT(s_pResultList!=NULL);return s_pResultList;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	CFont m_Font;
public:
	void FindObjectsBySceneName(CString strContent);
	void FindObjectsByID(uint32 id);
	void FindObjectsByMF1Name(CString strContent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	std::vector<CommonObject> m_FindObjectVec;
	void PurgeResource();
};


