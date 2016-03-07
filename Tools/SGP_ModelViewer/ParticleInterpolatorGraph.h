#pragma once

#include "../SGPLibraryCode/SGPHeader.h"
#include <set>
#include "HelpFunction.h"

struct EntryNode
{
	float m_x;
	float m_y0;
	float m_y1;
	EntryNode(float x,float y0,float y1):m_x(x),m_y0(y0),m_y1(y1){}
	bool operator<(const EntryNode& other) const{return m_x<other.m_x;}
	bool operator==(const EntryNode& other) const{return m_x==other.m_x;}
	bool operator!=(const EntryNode& other) const{return m_x!=other.m_x;}
};

class CParticleInterpolatorGraph : public CStatic
{
	DECLARE_DYNAMIC(CParticleInterpolatorGraph)

public:
	CParticleInterpolatorGraph();
	virtual ~CParticleInterpolatorGraph();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	
	ParticleInterpolatorParam m_InterpolatorParam;

	float m_DefaultVal;
	float m_DefaultValue;
	float m_LifeTime;

	float m_MaxData;
	float m_MinData;
	BOOL m_HaveEntry;
	std::set<EntryNode> m_EntrySet;
	CRect m_rcClose;
	
	void RetrieveEntry();
	void ProcessEntry();
	bool m_bLBtnDown;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
