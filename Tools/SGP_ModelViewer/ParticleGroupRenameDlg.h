#pragma once


// CParticleGroupRenameDlg �Ի���

class CParticleGroupRenameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CParticleGroupRenameDlg)

public:
	CParticleGroupRenameDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParticleGroupRenameDlg();

// �Ի�������
	enum { IDD = IDD_PARTICLE_GROUP_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	void SetProperty(CMFCPropertyGridProperty* pProperty){m_pProperty=pProperty;}
private:
	CMFCPropertyGridProperty* m_pProperty;
};
