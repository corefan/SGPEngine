// ParticleGroupRenameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "SGP_ModelViewer.h"
#include "ParticleGroupRenameDlg.h"
#include "afxdialogex.h"

// CParticleGroupRenameDlg 对话框

IMPLEMENT_DYNAMIC(CParticleGroupRenameDlg, CDialogEx)

CParticleGroupRenameDlg::CParticleGroupRenameDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CParticleGroupRenameDlg::IDD, pParent)
{

}

CParticleGroupRenameDlg::~CParticleGroupRenameDlg()
{
}

void CParticleGroupRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CParticleGroupRenameDlg, CDialogEx)
	
END_MESSAGE_MAP()

// CParticleGroupRenameDlg 消息处理程序

void CParticleGroupRenameDlg::OnOK()
{
	CString str;
	str.TrimLeft();
	str.TrimRight();
	GetDlgItemText(IDE_GROUP_NAME,str);
	if(str.GetLength()>15) str=str.Left(15);
	str="Group "+str;
	m_pProperty->SetName(str);

	CDialogEx::OnOK();
}
