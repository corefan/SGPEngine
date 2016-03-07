#pragma once


// CParticleGroupRenameDlg 对话框

class CParticleGroupRenameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleGroupRenameDlg)

public:
	CParticleGroupRenameDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParticleGroupRenameDlg();

// 对话框数据
	enum { IDD = IDD_PARTICLE_GROUP_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	void SetProperty(CMFCPropertyGridProperty* pProperty){m_pProperty=pProperty;}
private:
	CMFCPropertyGridProperty* m_pProperty;
};
