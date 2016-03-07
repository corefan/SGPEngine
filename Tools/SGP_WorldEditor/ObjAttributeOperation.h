#pragma once

#include "IOperation.h"
#include "CommonObjDef.h"
#include "..\SGPLibraryCode\SGPHeader.h"
#include <vector>

struct ObjAttributeData
{
	CommonObject m_Obj;
	union
	{
		ISGPObject* m_pPrevObj;
		ISGPLightObject* m_pPrevLightObj;
	};
	union
	{
		ISGPObject* m_pCurrObj;
		ISGPLightObject* m_pCurrLightObj;
	};
};

class ObjAttributeOperation:public IOperation
{
public:
	ObjAttributeOperation();
	virtual ~ObjAttributeOperation();
	virtual void undo();
	virtual void redo();

	uint32 AddObject(ISGPObject* pObj);
	uint32 AddObject(ISGPLightObject* pLightObj);
	void SavePrevState(uint32 index);
	void SaveCurrState(uint32 index);
	uint32 AddObject(const CommonObject& obj);
	void AddObjects(const std::vector<CommonObject>& objVector);
	void AddMF1Objects(const std::vector<CommonObject>& objVector);
	void AddLightObjects(const std::vector<CommonObject>& objVector);
	void SaveAllPrevState();
	void SaveAllCurrState();
private:
	std::vector<ObjAttributeData> m_DataVector;

	void SetAttribute(ISGPObject* pDestObj,ISGPObject* pSourceObj);
	void SetAttribute(ISGPLightObject* pDestObj,ISGPLightObject* pSourceObj);
};