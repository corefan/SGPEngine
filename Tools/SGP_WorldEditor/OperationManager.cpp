#include "stdafx.h"
#include "OperationManager.h"

IMPLEMENT_SINGLETON(OperationManager)

OperationManager::OperationManager(void)
{
	m_bModifyedFlag=false;
	m_pLastOp=NULL;
}

OperationManager::~OperationManager(void)
{
	ClearAllOperation();
}

void OperationManager::CheckAndAdjust()
{
	if(m_UndoStack.size() >= MAX_OPERATION_STEP)
	{
		IOperation* outOp = m_UndoStack.front();
		m_UndoStack.pop_front();
		delete outOp;
	}
}

void OperationManager::AddNewOperation(IOperation* op)
{
	if(!m_UndoStack.empty())
	{
		ASSERT((int)op != (int)m_UndoStack.back());
	}

	for(OpStackInter iter = m_RedoStack.begin();iter != m_RedoStack.end();++iter)
		delete *iter;
	m_RedoStack.clear();

	CheckAndAdjust();
	m_UndoStack.push_back(op);

}

void OperationManager::Undo()
{
	if(!m_UndoStack.empty())
	{
		IOperation* undoOp = m_UndoStack.back();
		undoOp->undo();
		m_UndoStack.pop_back();
		m_RedoStack.push_back(undoOp);
	}
}

void OperationManager::Redo()
{
	if(!m_RedoStack.empty())
	{
		IOperation* redoOp = m_RedoStack.back();
		redoOp->redo();
		m_RedoStack.pop_back();
		CheckAndAdjust();
		m_UndoStack.push_back(redoOp);
	}
}

void OperationManager::ClearWaterOperation()
{
	OpStackInter iter;
	for(iter=m_UndoStack.begin();iter!=m_UndoStack.end();/*++iter*/)
	{
		if((*iter)->IsWaterOperation())
		{	
			delete *iter;
			m_UndoStack.erase(iter++);
		}
		else ++iter;
	}
	for(iter=m_RedoStack.begin();iter!=m_RedoStack.end();/*++iter*/)
	{
		if((*iter)->IsWaterOperation())
		{
			delete *iter;
			m_RedoStack.erase(iter++);
		}
		else ++iter;
	}
}

void OperationManager::ClearGrassOperation()
{
	OpStackInter iter;
	for(iter=m_UndoStack.begin();iter!=m_UndoStack.end();/*++iter*/)
	{
		if((*iter)->IsGrassOperation())
		{
			delete *iter;
			m_UndoStack.erase(iter++);
		}
		else ++iter;
	}
	for(iter=m_RedoStack.begin();iter!=m_RedoStack.end();/*++iter*/)
	{
		if((*iter)->IsGrassOperation())
		{
			delete *iter;
			m_RedoStack.erase(iter++);
		}
		else ++iter;
	}
}

void OperationManager::ClearAllOperation()
{
	OpStackInter iter;
	for(iter = m_UndoStack.begin();iter != m_UndoStack.end(); ++iter)
		delete *iter;
	m_UndoStack.clear();

	for(iter = m_RedoStack.begin();iter != m_RedoStack.end(); ++iter)
		delete *iter;
	m_RedoStack.clear();
}

void OperationManager::ClearModifyFlag()
{
	IOperation* pLastOp=NULL;
	std::list<IOperation*>::reverse_iterator iter=m_UndoStack.rbegin();
	if(iter!=m_UndoStack.rend()) pLastOp=*iter;
	m_pLastOp=pLastOp;
	m_bModifyedFlag=false;
}

bool OperationManager::NeedSave()
{
	IOperation* pLastOp=NULL;
	std::list<IOperation*>::reverse_iterator iter=m_UndoStack.rbegin();
	if(iter!=m_UndoStack.rend()) pLastOp=*iter;
	if(pLastOp!=m_pLastOp||m_bModifyedFlag) return true;
	else return false;
}

void OperationManager::CloseMap()
{
	ClearAllOperation();
	m_pLastOp=NULL;
	m_bModifyedFlag=false;
}