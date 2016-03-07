#pragma once

#include "SkydomeSettingDlg.h"
#include "SceneObjectDlg.h"
#include "ObjectEditorDlg.h"
// CSceneEditorDlg 对话框

class CSceneEditorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneEditorDlg)

public:
	CSceneEditorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSceneEditorDlg();

// 对话框数据
	enum { IDD = IDD_SCENE_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CSkydomeSettingDlg m_SkydomeDlg;
	CSceneObjectDlg m_ObjectDlg;
	CObjectEditorDlg m_ObjEditorDlg;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetCtrlPos();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void EnableAllCtrls(BOOL bEnable);
};
