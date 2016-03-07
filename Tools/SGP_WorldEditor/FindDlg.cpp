// FindDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WorldEditor.h"
#include "FindDlg.h"
#include "afxdialogex.h"

// CFindDlg �Ի���

IMPLEMENT_DYNAMIC(CFindDlg, CDialogEx)

CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFindDlg::IDD, pParent)
{

}

CFindDlg::~CFindDlg()
{
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialogEx)
END_MESSAGE_MAP()

// CFindDlg ��Ϣ�������

BOOL CFindDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_OPTION_SCENE_NAME,BST_CHECKED);

	return TRUE;
}

void CFindDlg::OnOK()
{
	GetDlgItemText(IDE_FIND_CONTENT,m_FindContent);
	m_FindContent.Trim();
	if(m_FindContent.IsEmpty())
	{
		MessageBox("Please input find content","WorldEditor",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDE_FIND_CONTENT)->SetFocus();
		return ;
	}

	if(IsDlgButtonChecked(IDC_OPTION_SCENE_NAME)==BST_CHECKED) m_FindOption=IDC_OPTION_SCENE_NAME;
	else if(IsDlgButtonChecked(IDC_OPTION_ID)==BST_CHECKED) m_FindOption=IDC_OPTION_ID;
	else if(IsDlgButtonChecked(IDC_OPTION_MF1NAME)==BST_CHECKED) m_FindOption=IDC_OPTION_MF1NAME;
	else ASSERT(FALSE);

	if(m_FindOption==IDC_OPTION_ID)
	{
		for(int i=0;i<m_FindContent.GetLength();++i)
		{
			char c=m_FindContent.GetAt(i);
			if(c<'0'||c>'9')
			{
				MessageBox("Please input ID in correct format!","WorldEditor",MB_OK|MB_ICONERROR);
				GetDlgItem(IDE_FIND_CONTENT)->SetFocus();
				((CEdit*)GetDlgItem(IDE_FIND_CONTENT))->SetSel(0,-1);
				return ;
			}
		}
	}

	CDialogEx::OnOK();
}
