#pragma once

#include "IOperation.h"
#include <vector>

class TerrainWaterOperation : public IOperation
{
	friend class TerrainWaterHandler;
public:
	TerrainWaterOperation(bool bAdd);
	virtual ~TerrainWaterOperation();
public:
	virtual void undo();
	virtual void redo();
	void AddChunkIndex(int index){m_ChunkIdVector.push_back(index);}
	bool IsValidate(){return m_ChunkIdVector.size()!=0;}
	void SetWaterAttr(CString texPath,float height);
	virtual bool IsWaterOperation(){return true;}
private:
	std::vector<int> m_ChunkIdVector;
	CString m_WaterTexPath;
	float m_WaterHeight;
	bool m_bAdd;
};