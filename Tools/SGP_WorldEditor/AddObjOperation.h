#pragma once

#include <vector>
#include "..\SGPLibraryCode\SGPHeader.h"
#include "IOperation.h"
#include "CommonObjDef.h"

namespace sgp
{
	struct ISGPObject;
}

class AddObjOperation : public IOperation
{
	friend class DeleteObjOperation;
public:
	AddObjOperation();
	virtual ~AddObjOperation();

	virtual void undo();
	virtual void redo();
	void AddObject(const CommonObject& obj){m_ObjVector.push_back(obj);}
	void AddObject(const std::vector<CommonObject>& objVector);
	void SetDeleteFlag(bool bDelete){m_bDelete=bDelete;}
private:
	std::vector<CommonObject> m_ObjVector;
	bool m_bDelete;
};

class DeleteObjOperation : public IOperation
{
public:
	DeleteObjOperation(){m_AddObjOp.SetDeleteFlag(true);}
	virtual ~DeleteObjOperation(){}

	virtual void undo(){m_AddObjOp.redo();m_AddObjOp.SetDeleteFlag(false);}
	virtual void redo(){m_AddObjOp.undo();m_AddObjOp.SetDeleteFlag(true);}
	void AddObject(const CommonObject& obj){m_AddObjOp.AddObject(obj);}
	void AddObject(const std::vector<CommonObject>& objVector){m_AddObjOp.AddObject(objVector);}
private:
	AddObjOperation m_AddObjOp;
};