#pragma once

#include "..\SGP_ModelViewer\ExpandButton.h"
#include "ObjectTreeCtrl.h"
#include "afxcmn.h"
// CSceneObjectDlg �Ի���

class CSceneObjectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSceneObjectDlg)

public:
	CSceneObjectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSceneObjectDlg();
	static CSceneObjectDlg* s_pSceneObjDlg;
	static CSceneObjectDlg* GetInstance(){ASSERT(s_pSceneObjDlg!=NULL);return s_pSceneObjDlg;}
// �Ի�������
	enum { IDD = IDD_SCENE_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CExpandButton m_ExpandButton;
	CRect m_rc;
//	CObjectTreeCtrl m_ObjTreeCtrl;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void SetExpand(BOOL bExpand);
	afx_msg void OnExpand();
	CObjectTreeCtrl m_ObjTreeCtrl;

	afx_msg void OnDeleteObject();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAddObject();
	afx_msg void OnAddLightObj();

	void EnableAllCtrls(BOOL bEnable);
	void EnableObjectCtrls(BOOL bEnable);
	void EnableLightCtrls(BOOL bEnable);
};
