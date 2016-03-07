#pragma once

#include "ExpandButton.h"
#include "RoundButton.h"
#include <vector>
#include "ParticlePropertyGridCtrl.h"

namespace sgp
{
	struct ParticleSystemParam;
}
class CParticleCheckGridProperty;
class CParticleGroupGridProperty;

// CParticleSystemDlg 对话框

class CParticleSystemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleSystemDlg)

public:
	CParticleSystemDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleSystemDlg();

// 对话框数据
	enum { IDD = IDD_PARTICLE_SYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CExpandButton m_ExpandButton;
	CRect m_rc;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSystemSetting();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedParticleAddGroup();
	afx_msg LRESULT UpdateDetailList(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnParticleDetailChanged(WPARAM wParam,LPARAM lParam);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SetShowMode(BOOL bExpand);
	BOOL IsExpand(){return m_ExpandButton.m_bExpanded;}	
	void SetShowName(CString strName);

private:
	CFont m_Font;
	int m_NewGroupCount;
	CMFCPropertyGridCtrl m_DetailList;
	CParticleCheckGridProperty* m_BoundBoxComputeProperty;
	std::vector<CParticleGroupGridProperty*> m_GroupPropertyVector;
public:
	// Data Transformation related
	void LoadFromData(const sgp::ParticleSystemParam& data);
	BOOL CheckValidate();
	void SaveToData(sgp::ParticleSystemParam& data);
	void PreCloseAction();
};
