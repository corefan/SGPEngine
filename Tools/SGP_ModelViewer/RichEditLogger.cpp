#include "stdafx.h"
#include "RichEditLogger.h"

using namespace sgp;

RichEditLogger::RichEditLogger(CRichEditCtrl* pCtrl)\
	:m_pEditCtrl(pCtrl)
{}

void RichEditLogger::SetEditCtrl(CRichEditCtrl* pCtrl)
{
	m_pEditCtrl=pCtrl;
}

void RichEditLogger::logMessage(const String& message, ESGPLOG_LEVEL ll)
{
	CString str;
	COLORREF col;
	CTime time=CTime::GetCurrentTime();
	str=time.Format("%Y-%m-%d %H:%M:%S ");
	switch(ll)
	{
	case ELL_DEBUG:
		str+="[Debug] ";
		col=RGB(0,0,0);
		break;
	case ELL_INFORMATION:
		str+="[Info] ";
		col=RGB(0,0,0);
		break;
	case ELL_WARNING:
		str+="[Warning] ";
		col=RGB(0,0,255);
		break;
	case ELL_ERROR:
		str+="[Error] ";
		col=RGB(255,0,0);
		break;
	default:
		break;
	}

	str+=message.toUTF8().getAddress();
	str+="\r\n";
	
	m_pEditCtrl->SetSel(-1,-1);

	CHARFORMAT2 cFormat;
	cFormat.cbSize=sizeof(cFormat);
	cFormat.dwMask=CFM_COLOR;
	cFormat.dwEffects &= ~CFE_AUTOCOLOR;
	cFormat.crTextColor=col;
	m_pEditCtrl->SetSelectionCharFormat(cFormat);

	m_pEditCtrl->ReplaceSel(str);

	//scroll to bottom
	m_pEditCtrl->PostMessage(WM_VSCROLL,SB_BOTTOM,0);
}

void RichEditLogger::setLogLevel(ESGPLOG_LEVEL _level)
{
	m_LogLevel=_level;
}

/*****************************************************
output message
Logger::getCurrentLogger()->writeToLog(String(""),);
*****************************************************/