#pragma once


// CRoundButton
enum RoundButtonImageType
{
	PlayRoundButton,
	StopRoundButton,
	PauseRoundButton,
	PrevFrameButton,
	NextFrameButton,
	PrevAnimationButton,
	NextAnimationButton,
	CloseDlgButton,
	NotAnyButton
};

class CRoundButton : public CButton
{
	DECLARE_DYNAMIC(CRoundButton)

public:
	CRoundButton();
	virtual ~CRoundButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	RoundButtonImageType m_nImageType;
	void SetHaveText(BOOL bText);
	BOOL m_bText;
	BOOL m_bRound;
	virtual void PreSubclassWindow();
	BOOL m_bCustomBkColor;
	COLORREF m_BkColor;
};


