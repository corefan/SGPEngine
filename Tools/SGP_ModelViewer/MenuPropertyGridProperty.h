#pragma once

enum MenuPropertyType
{
	TextureMenu,
	ParticleMenu
};

class CMenuPropertyGridProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMenuPropertyGridProperty)
public:
	CMenuPropertyGridProperty(const CString& strName, const COleVariant& varValue);
public:
	virtual void OnRClickValue(CPoint C,BOOL B);
	virtual BOOL OnEdit(LPPOINT lptClick){return FALSE;}
	virtual BOOL PushChar(UINT nChar){return TRUE;}
	virtual BOOL OnDblClk(CPoint point);
public:
	void SetModifiedFlag(){CMFCPropertyGridProperty::SetModifiedFlag();}
	void SetMenuType(MenuPropertyType nType){m_MenuType=nType;}
	void SetRange(int currIndex,int totalCount);
	int GetCurrIndex(){return m_currIndex;}
	void SetIndex(int index){m_currIndex=index;}
private:
	MenuPropertyType m_MenuType;
	int m_currIndex;
	int m_TotalCount;
};

inline void CMenuPropertyGridProperty::SetRange(int currIndex,int totalCount)
{
	m_currIndex=currIndex;
	m_TotalCount=totalCount;
}