#pragma once

enum EditPropertyType
{
	Property_MinBox_X,
	Property_MinBox_Y,
	Property_MinBox_Z,
	Property_MaxBox_X,
	Property_MaxBox_Y,
	Property_MaxBox_Z
};

class CEditPropertyGridProperty: public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CEditPropertyGridProperty)
public:
	CEditPropertyGridProperty(const CString& strName, const COleVariant& varValue,int index,EditPropertyType nType);
	virtual ~CEditPropertyGridProperty();
private:
	int m_Index;
	EditPropertyType m_nType;
public:
	int GetIndex(){return m_Index;}
	EditPropertyType GetType(){return m_nType;}
};
