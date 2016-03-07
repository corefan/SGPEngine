#pragma once

#include <list>
#include "IOperation.h"

#define MAX_OPERATION_STEP 30

class OperationManager
{
private:
	OperationManager(void);
	~OperationManager(void);
	DECLARE_SINGLETON(OperationManager)
public:
	void AddNewOperation(IOperation* op);
	void Undo();
	void Redo();
	bool HasUndo(){return !m_UndoStack.empty();}
	bool HasRedo(){return !m_RedoStack.empty();}
	void ClearWaterOperation();
	void ClearGrassOperation();

	void ClearAllOperation();
	bool NeedSave();
	void SetModifyFlag(){m_bModifyedFlag=true;}
	void ClearModifyFlag();
	void CloseMap();
private:
	void CheckAndAdjust();

	typedef std::list<IOperation*> OpStack;
	typedef std::list<IOperation*>::iterator OpStackInter;
	OpStack m_UndoStack;
	OpStack m_RedoStack;
	bool m_bModifyedFlag;
	IOperation* m_pLastOp;
};
