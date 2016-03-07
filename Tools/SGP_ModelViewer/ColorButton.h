#pragma once


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT);

	void SetColor(float r,float g,float b);
	void SetColor(COLORREF col);
	void SetColor(float* pRGB);
	void ShowText(BOOL bShow);
public:
	COLORREF m_Col;
	BOOL m_bText;
};
