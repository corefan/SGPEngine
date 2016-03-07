#pragma once

enum TexLayerType
{
	VertexLayer0,
	VertexLayer1,
	VertexLayer2,
	VertexLayer3,
	ChunkLayer0,
	ChunkLayer1,
	ChunkLayer2,
	ChunkLayer3,
	ChunkNormalMap,
	ChunkDetailMap,
	ChunkSlopeMap
};
// CImageCtrl

class CImageCtrl : public CStatic
{
	DECLARE_DYNAMIC(CImageCtrl)

public:
	CImageCtrl();
	virtual ~CImageCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
public:
	void SetImage(CString imgPath,CString texPath);
	bool GetLayerSel(){return m_bSel;}
	void SetLayerSel(bool bSel);
	void SetLayer(TexLayerType nLayer){m_nLayer=nLayer;}
	TexLayerType GetLayer(){return m_nLayer;}
	void CheckImageExist();
private:
	bool m_bSel;
	TexLayerType m_nLayer;
	CString m_ImagePath;
	CString m_TexturePath;
public:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDeleteTexture();
};
