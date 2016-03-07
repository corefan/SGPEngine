#pragma once


// CImageShowDlg �Ի���
#include "ImageStatic.h"

class CImageShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CImageShowDlg)

public:
	CImageShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CImageShowDlg();

// �Ի�������
	enum { IDD = IDD_SHOW_IMAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	void SetScrollSize(int width,int height);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	CImageStatic m_Image;
	BOOL m_bInitialized;
	int m_PicWidth;
	int m_PicHeight;

	void SliderScroll(int cx,int cy);
};
