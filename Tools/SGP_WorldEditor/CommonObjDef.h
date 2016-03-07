#pragma once

enum EditorObjectType
{
	ObjType_None,
	ObjType_MF1,
	ObjType_Light
};

namespace sgp
{
	struct ISGPObject;
	struct ISGPLightObject;
}

struct CommonObject
{
	EditorObjectType m_type;
	union
	{
		sgp::ISGPObject* m_pObj;
		sgp::ISGPLightObject* m_pLightObj;
	};
	CommonObject(){m_pObj=NULL;m_pLightObj=NULL;m_type=ObjType_None;}
	CommonObject(sgp::ISGPObject* pObj){m_pObj=pObj;m_type=ObjType_MF1;}
	CommonObject(sgp::ISGPLightObject* pLightObj){m_pLightObj=pLightObj;m_type=ObjType_Light;}
	bool IsMF1() const {return m_type==ObjType_MF1;}
	bool IsLight() const {return m_type==ObjType_Light;}
	void SetNULL(){m_type=ObjType_None;m_pObj=NULL;m_pLightObj=NULL;}
	bool IsNULL()
	{
		if(m_type==ObjType_MF1&&m_pObj!=NULL) return false;
		if(m_type==ObjType_Light&&m_pLightObj!=NULL) return false;
		return true;
	}
	bool operator==(const CommonObject& other) const
	{
		if(m_type!=other.m_type) return false;
		if(m_type==ObjType_MF1&&m_pObj==other.m_pObj) return true;
		if(m_type==ObjType_Light&&m_pLightObj==other.m_pLightObj) return true;
		return false;
	}
};