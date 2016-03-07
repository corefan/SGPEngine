#pragma once


// CGrassImageCtrl

class CWaterImageCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWaterImageCtrl)

public:
	CWaterImageCtrl();
	virtual ~CWaterImageCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	CString m_ImgPath;
	CString m_TexPath;
public:
	void SetImage(CString imgPath,CString texPath);
	void SetImageTileX(int tileX);
	void SetImageTileY(int tileY);
	void CheckExist();
};
