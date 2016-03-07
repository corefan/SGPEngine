#pragma once

class CObjGridCheckProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CObjGridCheckProperty)
public:
	CObjGridCheckProperty(const CString& strName,bool bChecked);
	virtual void OnDrawValue(CDC* pDC,CRect rect);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL PushChar(UINT nChar);
public:
	void SetCheck(bool bCheck);
	bool GetCheck(){return m_bCheck;}
	void CheckAction();
private:
	bool m_bCheck;
	DWORD m_dwStart;
};