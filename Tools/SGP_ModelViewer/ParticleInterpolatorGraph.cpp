// InterpolatorGraph.cpp : 实现文件
//

#include "stdafx.h"
#include "SGP_ModelViewer.h"
#include "ParticleInterpolatorGraph.h"
#include <limits.h>
#include "CommonMacroDef.h"
// CParticleInterpolatorGraph

IMPLEMENT_DYNAMIC(CParticleInterpolatorGraph, CStatic)

CParticleInterpolatorGraph::CParticleInterpolatorGraph()
{
	m_MaxData=1.0f;
	m_MinData=0.0f;
	m_HaveEntry=FALSE;
	m_rcClose.SetRectEmpty();
	m_bLBtnDown=false;
}

CParticleInterpolatorGraph::~CParticleInterpolatorGraph()
{
}

BEGIN_MESSAGE_MAP(CParticleInterpolatorGraph, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CParticleInterpolatorGraph 消息处理程序

void CParticleInterpolatorGraph::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc=lpDrawItemStruct->rcItem;
	Graphics graph(lpDrawItemStruct->hDC);
	graph.SetSmoothingMode(SmoothingModeHighQuality);

	SolidBrush brush(Color(255,255,255));
	graph.FillRectangle(&brush,rc.left,rc.top,rc.Width(),rc.Height());
	
	Pen penBorder(Color(73,170,249),4);
	graph.DrawRectangle(&penBorder,rc.left+1,rc.top+1,rc.Width()-3,rc.Height()-3);

	Pen penClose(Color(200,0,0),3);
	m_rcClose.right=rc.right-8;
	m_rcClose.top=rc.top+8;
	m_rcClose.left=m_rcClose.right-16;
	m_rcClose.bottom=m_rcClose.top+16;
	graph.DrawLine(&penClose,m_rcClose.left,m_rcClose.top,m_rcClose.right,m_rcClose.bottom);
	graph.DrawLine(&penClose,m_rcClose.right,m_rcClose.top,m_rcClose.left,m_rcClose.bottom);

	rc.DeflateRect(45,25,20,30);
	Pen pen(Color(0,0,0),3);
	pen.SetEndCap(LineCapArrowAnchor);
	graph.DrawLine(&pen,rc.left,rc.bottom,rc.right,rc.bottom);
	graph.DrawLine(&pen,rc.left,rc.bottom,rc.left,rc.top);

	Gdiplus::Font font(L"微软雅黑",10);
	SolidBrush brushText(Color(0,0,128));
	graph.DrawString(L"x",wcslen(L"x"),&font,PointF((Gdiplus::REAL)rc.right,\
		(Gdiplus::REAL)(rc.bottom-10)),&brushText);
	graph.DrawString(L"y",wcslen(L"y"),&font,PointF((Gdiplus::REAL)(rc.left-4),\
		(Gdiplus::REAL)(rc.top-20)),&brushText);

	graph.DrawString(L"0",wcslen(L"0"),&font,PointF((Gdiplus::REAL)(rc.left),\
		(Gdiplus::REAL)(rc.bottom)),&brushText);
	graph.DrawString(L"1",wcslen(L"1"),&font,PointF((Gdiplus::REAL)(rc.right-30),\
		(Gdiplus::REAL)(rc.bottom)),&brushText);

	int maxx=rc.right-rc.left-28;
	int maxy=rc.bottom-(rc.top+30);

	Pen penDash(Color(20,20,20),2);
	penDash.SetDashStyle(DashStyleDash);
	graph.DrawLine(&penDash,rc.left,rc.top+30,rc.right-28,rc.top+30);
	graph.DrawLine(&penDash,rc.right-28,rc.bottom,rc.right-28,rc.top+30);

	if(!m_HaveEntry)
	{
		RetrieveEntry();
		ProcessEntry();
		m_HaveEntry=TRUE;
	}

	Pen penGreen(Color(0,255,0),2);
	Pen penRed(Color(255,0,0),2);
	penRed.SetEndCap(LineCapRoundAnchor);
	penGreen.SetEndCap(LineCapRoundAnchor);

	float deltaVal=m_MaxData-m_MinData;

	CString str;
	str.Format("%.1f",m_MinData);
	if(str.Right(2)==".0") str=str.Left(str.GetLength()-2);
	BSTR flag=str.AllocSysString();
	graph.DrawString(flag,wcslen(flag),&font,PointF((float)(rc.left-40),\
		(float)(rc.bottom-10)),&brushText);
	SysFreeString(flag);

	str.Format("%.1f",m_MaxData);
	if(str.Right(2)==".0") str=str.Left(str.GetLength()-2);
	flag=str.AllocSysString();
	graph.DrawString(flag,wcslen(flag),&font,PointF((float)(rc.left-40),\
		(float)(rc.top+20)),&brushText);
	SysFreeString(flag);

	GraphicsPath path;

	int size=m_EntrySet.size();
	if(size!=0)
	{
		std::set<EntryNode>::iterator iter1,iter2;
		for(iter1=m_EntrySet.begin();iter1!=m_EntrySet.end();++iter1)
		{
			iter2=iter1;
			++iter2;
			
			if(iter1==m_EntrySet.begin())
			{
				float endx=rc.left+iter1->m_x*maxx;
				float endy=rc.bottom-(iter1->m_y0-m_MinData)*maxy/deltaVal;
				float startx=rc.left+iter1->m_x*maxx;
				float starty=rc.bottom-(iter1->m_y1-m_MinData)*maxy/deltaVal;
				path.AddLine(startx,starty,endx,endy);
			}
			if(iter2!=m_EntrySet.end())
			{
				float startx=rc.left+iter1->m_x*maxx;
				float starty0=rc.bottom-(iter1->m_y0-m_MinData)*maxy/deltaVal;
				float endx=rc.left+iter2->m_x*maxx;
				float endy0=rc.bottom-(iter2->m_y0-m_MinData)*maxy/deltaVal;
				path.AddLine(startx,starty0,endx,endy0);
			}
			else
			{
				float startx=rc.left+iter1->m_x*maxx;
				float starty=rc.bottom-(iter1->m_y0-m_MinData)*maxy/deltaVal;
				float endx=rc.left+iter1->m_x*maxx;
				float endy=rc.bottom-(iter1->m_y1-m_MinData)*maxy/deltaVal;
				path.AddLine(startx,starty,endx,endy);
			}
		}

		std::set<EntryNode>::reverse_iterator riter1,riter2;
		for(riter1=m_EntrySet.rbegin();riter1!=m_EntrySet.rend();++riter1)
		{
			riter2=riter1;
			++riter2;
			if(riter2!=m_EntrySet.rend())
			{
				float startx=rc.left+riter1->m_x*maxx;
				float starty=rc.bottom-(riter1->m_y1-m_MinData)*maxy/deltaVal;
				float endx=rc.left+riter2->m_x*maxx;
				float endy=rc.bottom-(riter2->m_y1-m_MinData)*maxy/deltaVal;
				path.AddLine(startx,starty,endx,endy);
			}
		}

		SolidBrush brush(Color(125,255,255,0));
		graph.FillPath(&brush,&path);
	}
	if(size!=0)
	{
		std::set<EntryNode>::iterator iter1,iter2;
		for(iter1=m_EntrySet.begin();iter1!=m_EntrySet.end();++iter1)
		{
			iter2=iter1;
			++iter2;
			if(iter2!=m_EntrySet.end())
			{
				float startx=rc.left+iter1->m_x*maxx;
				float starty0=rc.bottom-(iter1->m_y0-m_MinData)*maxy/deltaVal;
				float starty1=rc.bottom-(iter1->m_y1-m_MinData)*maxy/deltaVal;
				float endx=rc.left+iter2->m_x*maxx;
				float endy0=rc.bottom-(iter2->m_y0-m_MinData)*maxy/deltaVal;
				float endy1=rc.bottom-(iter2->m_y1-m_MinData)*maxy/deltaVal;
				graph.DrawLine(&penGreen,startx,starty0,endx,endy0);
				graph.DrawLine(&penRed,startx,starty1,endx,endy1);
			}
		}
	}
}

void CParticleInterpolatorGraph::RetrieveEntry()
{
	switch(m_InterpolatorParam.m_InterpolatorType)
	{
	case Interpolator_SelfDefine:
		{
			ParticleSelfDefInterpolatorData& selfDefData=m_InterpolatorParam.m_SelfDefData;
			for(uint32 i=0;i<selfDefData.m_count;++i)
			{
				m_EntrySet.insert(EntryNode(selfDefData.m_pEntry[i].m_x,\
					selfDefData.m_pEntry[i].m_y0,selfDefData.m_pEntry[i].m_y1));
				if(selfDefData.m_pEntry[i].m_y0>m_MaxData) m_MaxData=selfDefData.m_pEntry[i].m_y0;
				if(selfDefData.m_pEntry[i].m_y1>m_MaxData) m_MaxData=selfDefData.m_pEntry[i].m_y1;
				if(selfDefData.m_pEntry[i].m_y0<m_MinData) m_MinData=selfDefData.m_pEntry[i].m_y0;
				if(selfDefData.m_pEntry[i].m_y1<m_MinData) m_MinData=selfDefData.m_pEntry[i].m_y1;
			}
		}
		break;
	case Interpolator_Sinusoidal:
		{
			ParticleSinInterpolatorData& sinData=m_InterpolatorParam.m_SinData;
			for(uint32 i=0;i<sinData.m_nbSamples;++i)
			{
				float x=sinData.m_startX+i*sinData.m_period*sinData.m_length/(sinData.m_nbSamples-1);
				float sin=std::sin((x+sinData.m_offsetX)*2*float_Pi/sinData.m_period);
				float y0=sinData.m_amplitudeMin*sin+sinData.m_offsetY;
				float y1=sinData.m_amplitudeMax*sin+sinData.m_offsetY;
				if(y0<m_MinData) m_MinData=y0;
				if(y0>m_MaxData) m_MaxData=y0;
				if(y1<m_MinData) m_MinData=y1;
				if(y1>m_MaxData) m_MaxData=y1;
				if(y0>y1)
				{
					float tmp=y0;
					y0=y1;
					y1=tmp;
				}
				m_EntrySet.insert(EntryNode(x,y0,y1));
			}
		}
		break;
	case Interpolator_Polynomial:
		{
			ParticlePolyInterpolatorData& polyData=m_InterpolatorParam.m_PolyData;
			for(uint32 i=0;i<polyData.m_nbSamples;++i)
			{
				float x=polyData.m_startX+i*(polyData.m_endX-polyData.m_startX)/(polyData.m_nbSamples-1);
				float x2=x*x;
				float x3=x2*x;
				float y0=polyData.m_constant+x*polyData.m_linear+x2*polyData.m_quadratic+x3*polyData.m_cubic;
				if(y0<m_MinData) m_MinData=y0;
				if(y0>m_MaxData) m_MaxData=y0;
				m_EntrySet.insert(EntryNode(x,y0,y0));
			}
		}
		break;
	}
}

void CParticleInterpolatorGraph::ProcessEntry()
{
	std::set<EntryNode>::iterator iter=m_EntrySet.end(),iter1=m_EntrySet.end(),iter2=m_EntrySet.end();
	EntryNode node(0.0f,1.0f,1.0f);
	if(m_EntrySet.find(node)==m_EntrySet.end())
	{
		for(iter=m_EntrySet.begin();iter!=m_EntrySet.end();++iter)
		{
			if(iter->m_x<0.0f) iter1=iter;
		}
		for(iter=m_EntrySet.begin();iter!=m_EntrySet.end();++iter)
		{
			if(iter->m_x>0.0f)
			{
				iter2=iter;
				break;
			}
		}
		if(iter1!=m_EntrySet.end()&&iter2!=m_EntrySet.end())
		{
			node.m_x=0.0f;
			node.m_y0=iter1->m_y0+(iter2->m_y0-iter1->m_y0)*(0-iter1->m_x)/(iter2->m_x-iter1->m_x);
			node.m_y1=iter1->m_y1+(iter2->m_y1-iter1->m_y1)*(0-iter1->m_x)/(iter2->m_x-iter1->m_x);
			m_EntrySet.insert(node);
		}
		else if(iter1!=m_EntrySet.end()&&iter2==m_EntrySet.end())
		{
			node.m_x=0.0f;
			node.m_y0=iter1->m_y0;
			node.m_y1=iter1->m_y1;
			m_EntrySet.insert(node);
		}
		else if(iter1==m_EntrySet.end()&&iter2!=m_EntrySet.end())
		{
			node.m_x=0.0f;
			node.m_y0=iter2->m_y0;
			node.m_y1=iter2->m_y1;
			m_EntrySet.insert(node);
		}
	}
	iter1=m_EntrySet.end();
	iter2=m_EntrySet.end();

	node.m_x=1.0f;
	if(m_EntrySet.find(node)==m_EntrySet.end())
	{
		for(iter=m_EntrySet.begin();iter!=m_EntrySet.end();++iter)
		{
			if(iter->m_x<1.0f) iter1=iter;
		}
		for(iter=m_EntrySet.begin();iter!=m_EntrySet.end();++iter)
		{
			if(iter->m_x>1.0f)
			{
				iter2=iter;
				break;
			}
		}
		if(iter1!=m_EntrySet.end()&&iter2!=m_EntrySet.end())
		{
			node.m_x=1.0f;
			node.m_y0=iter1->m_y0+(iter2->m_y0-iter1->m_y0)*(1.0f-iter1->m_x)/(iter2->m_x-iter1->m_x);
			node.m_y1=iter1->m_y1+(iter2->m_y1-iter1->m_y1)*(1.0f-iter1->m_x)/(iter2->m_x-iter1->m_x);
			m_EntrySet.insert(node);
		}
		else if(iter1!=m_EntrySet.end()&&iter2==m_EntrySet.end())
		{
			node.m_x=1.0f;
			node.m_y0=iter1->m_y0;
			node.m_y1=iter1->m_y1;
			m_EntrySet.insert(node);
		}
		else if(iter1==m_EntrySet.end()&&iter2!=m_EntrySet.end())
		{
			node.m_x=1.0f;
			node.m_y0=iter2->m_y0;
			node.m_y1=iter2->m_y1;
			m_EntrySet.insert(node);
		}
	}

	//clear Node that isn't between [0,1]
	for(iter=m_EntrySet.begin();iter!=m_EntrySet.end();)
	{
		if(iter->m_x<0.0f||iter->m_x>1.0f) m_EntrySet.erase(iter++);
		else ++iter;
	}
}

void CParticleInterpolatorGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_rcClose.PtInRect(point)) GetParent()->SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,0);
	m_bLBtnDown=true;
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CParticleInterpolatorGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bLBtnDown)
	{
		if(m_rcClose.PtInRect(point)) GetParent()->SendMessage(WM_CLOSE);
	}
	m_bLBtnDown=false;

	CStatic::OnLButtonUp(nFlags, point);
}
