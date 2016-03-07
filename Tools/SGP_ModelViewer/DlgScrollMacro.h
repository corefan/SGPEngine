#ifndef _ModelViewer_DlgScrollMacro_h__
#define _ModelViewer_DlgScrollMacro_h__

/*******************************************************************************
These macro is used to add horizonal and vertical scrollbar to dialogs
To realise this goal,the steps are as follows:
	1.put Declare_DlgScroll() in the header of dialog class declaration
	2.put Implement_DlgScroll(MyClass) in the cpp files of class implemention,
	  MyClass should replace with your own class name
	3.put DlgScroll_OnInit() in Dilaog Initialize function,such as OnInitDialog
	4.put DlgScroll_OnSize() in Dilaog size change Message response function
	5.put DlgScroll_MessageMap() in Dialog Message Maps:between BEGIN_MESSAGE_MAP
	  and END_MESSAGE_MAP.
You can refer to CModelControlDlg class to see how it is implemented.
*******************************************************************************/

#define Declare_DlgScroll() \
	void ScrollClient(int nBar, int nPos);\
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);\
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);\
	void HVScrool(int nBar, int nSBCode);\
	int GetScrollPos(int nBar, int nSBCode);

#define Implement_DlgScroll(MyClass) \
	void MyClass::ScrollClient(int nBar, int nPos)\
	{\
		static int s_prevx = 1;\
		static int s_prevy = 1;\
		int cx = 0; \
		int cy = 0; \
		int& delta = (nBar == SB_HORZ ? cx : cy); \
		int& prev = (nBar == SB_HORZ ? s_prevx : s_prevy); \
		delta = prev - nPos; \
		prev = nPos;\
		if(cx || cy) ScrollWindow(cx, cy, NULL, NULL);\
	}\
	void MyClass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)\
	{\
		HVScrool(SB_HORZ,nSBCode);\
		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);\
	}\
	void MyClass::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)\
	{\
		HVScrool(SB_VERT,nSBCode);\
		CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);\
	}\
	void MyClass::HVScrool(int nBar, int nSBCode)\
	{\
		const int scrollPos = GetScrollPos(nBar,nSBCode); \
		if(scrollPos == -1) return ;\
		SetScrollPos(nBar, scrollPos, TRUE); \
		ScrollClient(nBar, scrollPos); \
	}\
	int MyClass::GetScrollPos(int nBar, int nSBCode)\
	{\
		SCROLLINFO si;\
		si.cbSize = sizeof(SCROLLINFO); \
		si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;\
		GetScrollInfo(nBar, &si); \
		const int minPos = si.nMin;\
		const int maxPos = si.nMax - (si.nPage - 1);\
		int result = -1; \
		switch(nSBCode)\
		{\
		case SB_LINEUP: \
			result =(si.nPos-1)>minPos?(si.nPos-1):minPos; \
			break;\
		case SB_LINEDOWN:\
			result =(si.nPos+1)<maxPos?(si.nPos+1):maxPos; \
			break;\
		case SB_PAGEUP: \
			result =(si.nPos-(int)si.nPage)>minPos?(si.nPos-(int)si.nPage):minPos;\
			break;\
		case SB_PAGEDOWN:\
			result =(si.nPos+(int)si.nPage)<maxPos?(si.nPos+(int)si.nPage):maxPos;\
			break;\
		case SB_THUMBPOSITION: \
			break;\
		case SB_THUMBTRACK:\
			result = si.nTrackPos;\
			break;\
		case SB_TOP: \
			result = minPos;\
			break;\
		case SB_BOTTOM:\
			result = maxPos; \
			break;\
		case SB_ENDSCROLL: \
			break;\
		}\
		return result;\
	}
	
#define DlgScroll_OnInit() \
	{ModifyStyle(0,WS_HSCROLL|WS_VSCROLL);\
	CRect rect;\
	GetClientRect(rect);\
	SCROLLINFO si;\
	si.cbSize=sizeof(si);\
	si.fMask=SIF_PAGE|SIF_POS|SIF_RANGE;\
	si.nPos=si.nMin=1;\
	si.nMax=rect.Width();\
	si.nPage=rect.Width();\
	SetScrollInfo(SB_HORZ,&si,FALSE);\
	si.nMax=rect.Height();\
	si.nPage=rect.Height();\
	SetScrollInfo(SB_VERT,&si,FALSE);}
	
#define DlgScroll_OnSize() \
	{if(nType != SIZE_RESTORED && nType != SIZE_MAXIMIZED)\
	return ;\
	SCROLLINFO si;\
	si.cbSize = sizeof(SCROLLINFO);\
	CRect rc;\
	GetClientRect(rc);\
	const int bar[] = { SB_HORZ, SB_VERT };\
	const int page[] = { rc.Width(), rc.Height() };\
	for(size_t i = 0; i < ARRAYSIZE(bar); ++i) \
	{\
		si.fMask = SIF_PAGE;\
		si.nPage = page[i];\
		SetScrollInfo(bar[i], &si, TRUE);\
		si.fMask = SIF_RANGE | SIF_POS;\
		GetScrollInfo(bar[i], &si);\
		const int maxScrollPos = si.nMax - (page[i] - 1);\
		const bool needToScroll =(si.nPos != si.nMin && si.nPos == maxScrollPos) ||(nType == SIZE_MAXIMIZED); \
		if(needToScroll) ScrollClient(bar[i], si.nPos);\
	}}

#define DlgScroll_MessageMap() \
	ON_WM_HSCROLL() ON_WM_VSCROLL()

#endif