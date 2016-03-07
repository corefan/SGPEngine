#ifndef __SGP_ModelViewer_RichEditLogger_HEADER__
#define __SGP_ModelViewer_RichEditLogger_HEADER__

#include "../SGPLibraryCode/SGPHeader.h"

//richedit logger
class SGP_API RichEditLogger:public Logger
{
private:
	CRichEditCtrl* m_pEditCtrl;
public:
	RichEditLogger(CRichEditCtrl* pCtrl);
	RichEditLogger(){};
	void SetEditCtrl(CRichEditCtrl* pCtrl);
	void logMessage(const String& message, ESGPLOG_LEVEL ll/* =ELL_INFORMATION */);
	void setLogLevel(ESGPLOG_LEVEL _level);
};

#endif
