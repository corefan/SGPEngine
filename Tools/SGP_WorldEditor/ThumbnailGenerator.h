#pragma once

class ThumbnailGenerator
{
public:
	ThumbnailGenerator(CWnd* pWnd);
	~ThumbnailGenerator(){};
	BOOL GenerateThumbnail(CString filePath,CString destPath,unsigned int imgWidth=64,unsigned int imgHeight=64);
	void BuildMemDC(unsigned int width,unsigned int height);
	BOOL GenerateGrassThumbnail(CString filePath,int xTile,int yTile);
private:
	CDC m_MemDC;
	CBitmap m_MemBmp;
//	CImage m_Image;
	unsigned int m_Width,m_Height;
	CWnd *m_pWnd;
};