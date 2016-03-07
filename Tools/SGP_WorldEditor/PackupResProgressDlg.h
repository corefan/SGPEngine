#pragma once

#include <vector>
#include "afxcmn.h"

// CPackupResProgressDlg �Ի���

class CPackupResProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPackupResProgressDlg)

public:
	CPackupResProgressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPackupResProgressDlg();

// �Ի�������
	enum { IDD = IDD_PACKUP_RES_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);

	std::vector<CString> m_srcPathVec;

	BOOL m_bReady;
	int m_nCount;
	int m_nIndex;

	void UpdateCtrls();
	CProgressCtrl m_Progress;
public:
	// Config Data
	CString m_MapPath;
	CString m_FolderPath;
	int m_nMode;
	CString m_srcDir;
	CString m_destDir;
	EPVRTPixelFormat m_PixelFormat;
	pvrtexture::ECompressorQuality m_CompressQuality;
	
	void GatherInformation();
	void ProcessFolder(CString strPath);
};
