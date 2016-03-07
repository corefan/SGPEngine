#pragma once


// CTextureInfoDlg 对话框
#include "ImageShowDlg.h"
#include "RoundButton.h"

#include <vector>

namespace sgp
{
	class ISGPTexture;
}

class CTextureInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTextureInfoDlg)
	friend class CImageShowDlg;
public:
	CTextureInfoDlg(CString strPath,CString strFile,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTextureInfoDlg();
// 对话框数据
	enum { IDD = IDD_TEXTURE_INFO_DLG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:// message response function
	afx_msg LRESULT OnKickIdle(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnMipmapLevelChanged();
	afx_msg void OnChannelChanged();
	afx_msg void OnCubemapFaceChanged();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void LoadTexture();
	void UnLoadTexture();
	void DrawTexture(CDC* pDC);
	void UpdateImage();
	void SetPixelFormatInfo(WCHAR* wPath);
	virtual void OnOK();
public:// control variables
	CComboBox m_MipmapComboBox;
	CComboBox m_CubemapComboBox;
	CComboBox m_ChannelComboBox;
	CImageShowDlg m_ImageDlg;
	CRoundButton m_ButtonClose1;
	CRoundButton m_ButtonClose2;
public:// memory bitmap
	CDC m_MemDC;
	CBitmap m_MemBmp;
private:
	// texture info
	unsigned char* m_pData;
	CString m_TexturePath;
	CString m_FileName;
	int m_Height;
	int m_Width;
	sgp::ISGPTexture *m_pTexture;
	unsigned int m_TextureID;
	CString m_sMipmapLevel;
	CString m_sChannel;
	CString m_sCubemapFace;
	std::vector<int> m_MipmapDepthVector;
private:// configuration
	BOOL m_bShowLog;
	CFont m_TitleFont;
public:
	afx_msg void OnPaint();
	CBrush m_BorderBrush;
};
