#pragma once

class CMenuPropertyGridProperty;

#define PARTICLE_MENU_START 2000
#define PARTICLE_MENU_END	2100
#define PARTICLE_MENU_CONFIG 2101
// CDetailPropertyGridCtrl

class CDetailPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CDetailPropertyGridCtrl)

public:
	CDetailPropertyGridCtrl();
	virtual ~CDetailPropertyGridCtrl();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	void OnSelectTexture();
	void OnParticleSameTo(UINT nID);
	void OnParticleConfig();
	virtual void OnChangeSelection(CMFCPropertyGridProperty* pNewSel, CMFCPropertyGridProperty* pOldSel);
	void SetParticlePropertyInfo(int index);
private:
	CMenuPropertyGridProperty* m_MenuProperty;
};
