#pragma once


// FocusButton

class CFocusButton : public CButton
{
	DECLARE_DYNAMIC(CFocusButton)

public:
	CFocusButton();
	virtual ~CFocusButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	BOOL m_bFocused;
	virtual void PreSubclassWindow();
};


