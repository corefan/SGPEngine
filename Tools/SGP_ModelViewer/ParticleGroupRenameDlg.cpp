// ParticleGroupRenameDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "SGP_ModelViewer.h"
#include "ParticleGroupRenameDlg.h"
#include "afxdialogex.h"

// CParticleGroupRenameDlg �Ի���

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

// CParticleGroupRenameDlg ��Ϣ�������

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
