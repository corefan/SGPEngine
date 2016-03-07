#pragma once

class CSettingCheckProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CSettingCheckProperty)
public:
	CSettingCheckProperty(const CString& strName,BOOL bChecked,int index);
	virtual void OnDrawValue(CDC* pDC,CRect rect);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL PushChar(UINT nChar);
public:
	void SetCheck(BOOL bCheck);
	BOOL GetCheck(){return m_bCheck;}
	void CheckAction();
private:
	BOOL m_bCheck;
	DWORD m_dwStart;
	int m_index;
};