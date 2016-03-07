#pragma once

class CNewMapLoadingDlg;
// CNewMapConfigDlg �Ի���

class CNewMapConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewMapConfigDlg)

public:
	CNewMapConfigDlg(CNewMapLoadingDlg* loadingDlg,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNewMapConfigDlg();

// �Ի�������
	enum { IDD = IDD_NEWMAP_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBrowseLayer0Texture();
	afx_msg void OnBrowseMapFolder();
	virtual void OnOK();
	void CreateMap();
private:
	CString m_Layer0TexturePath;
	CNewMapLoadingDlg* m_loadingDlg;
	
};
