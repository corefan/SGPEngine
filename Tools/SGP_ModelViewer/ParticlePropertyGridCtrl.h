#pragma once


// CParticlePropertyGridCtrl

class CParticlePropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CParticlePropertyGridCtrl)

public:
	CParticlePropertyGridCtrl();
	virtual ~CParticlePropertyGridCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
