#pragma once

namespace sgp
{
	struct SGPMF1ConfigSetting;
	struct SGPMF1ParticleTag;
	struct ParticleSystemParam;
	struct ParticleGroupParam;
}

// CModelDetailPane
#include "DetailPropertyGridCtrl.h"
#include "CommonDockPane.h"
#include "ModelDetailConfigDlg.h"
#include <vector>
class CPropertyGridCheckProperty;

class CModelDetailPane : public /*CDockablePane*/CCommonDockPane
{
	DECLARE_DYNAMIC(CModelDetailPane)
	friend class CParticleConfigPane;
public:
	CModelDetailPane();
	virtual ~CModelDetailPane();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnDetailChanged(WPARAM wParam,LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void SetCtrlPos();
public:
	static CModelDetailPane* s_ModelDetailPane;
	static void NotifyModelChanged();
	void ModelChanged();
	void ModelClosed();
private:
	CDetailPropertyGridCtrl m_DetailList;
	CFont m_Font;
public:
	void ChangeBoundBox(CMFCPropertyGridProperty* pProp);
	
public:
	// config setting
	CModelDetailConfigDlg m_ConfigDlg;
	void PurgeAllConfig();
	void BuildConfigSetting();
	
	void ShowMesh(int index,BOOL bShow);
	void ShowParticle(int index,BOOL bShow);
	void ChangeTexture(int index,CString texturePath);
	sgp::SGPMF1ConfigSetting* m_pConfigSetting;
	std::vector<void*> m_MeshConfigVector;
	std::vector<void*> m_TextureConfigVector;
	std::vector<void*> m_ParticleConfigVector;
	std::vector<void*> m_RibbonConfigVector;
	std::vector<CPropertyGridCheckProperty*> m_MeshPropertyVector;
	std::vector<CPropertyGridCheckProperty*> m_ParticlePropertyVector;
	std::vector<CMFCPropertyGridProperty*> m_TexturePropertyVector;
	void LoadConfigSetting(sgp::SGPMF1ConfigSetting* configSetting);
public:
	void ParticleConfigure(int index);
	void ParticleConfigureSameTo(int index,int goal);
	std::vector<int> m_ParticleIndexVector;
	std::vector<sgp::ParticleGroupParam*> m_ParticleGroupVector;
	void AddParticleIndex(int index);
	int RetrieveParticlePos(int index);
	void ReleaseParticleTag();
public:
//	void RegisterTexture(sgp::SGPMF1ParticleTag& particleTag);
//	void UnRegisterTexture(sgp::SGPMF1ParticleTag& particleTag);
	void ReleaseAllocatedMF1();
};
