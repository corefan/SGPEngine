#pragma once

#include "../SGPLibraryCode/SGPHeader.h"

class RichEditLogger : public Logger
{
	CRichEditCtrl* m_pEditCtrl;
public:
	RichEditLogger(CRichEditCtrl* pCtrl);
	RichEditLogger(){};
	void SetEditCtrl(CRichEditCtrl* pCtrl);
	void logMessage(const String& message, ESGPLOG_LEVEL ll/* =ELL_INFORMATION */);
	void setLogLevel(ESGPLOG_LEVEL _level);
};