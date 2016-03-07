#pragma once

class MouseMsgHandler;

class MouseMsgInterface
{
private:
	MouseMsgInterface(void);
	~MouseMsgInterface(void);
	DECLARE_SINGLETON(MouseMsgInterface)
public:
	void LButtonDown(CPoint pt,UINT nFlags);
	void LButtonUp(CPoint pt,UINT nFlags);
	void MouseMove(CPoint pt,UINT nFlags);
	void RButtonDown(CPoint pt,UINT nFlags);
	void RButtonUp(CPoint pt,UINT nFlags);
	void CheckUncompleteOp();
	bool SetCursor();
private:
	MouseMsgHandler* m_CurrHandler;
	bool m_bRBtnDown;
	HWND m_Hwnd;
public:
	void SwitchToHeightEditor();
	void SwitchToTextureEditor();
	void SwitchToGrassEditor();
	void SwitchToWaterEditor();
	void SwitchToSelectEditor();
	void SwitchToNULL(){m_CurrHandler=NULL;}
	void DrawArea();
	void DrawFlag();
	void SetRenderWindow(HWND hwnd){m_Hwnd=hwnd;};
	BOOL CursorInRenderWindow();
	void SwitchToAddObjEditor();
	void SwitchToAddLightEditor();
	void SwitchToCloneObjEditor();
	bool IsSelectObjHandler();

	void StopInsertObjects();
};
