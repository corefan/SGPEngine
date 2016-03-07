#pragma once

#include "IOperation.h"
#include <vector>
#include <set>
#include <list>

struct TerrainHeightOpData
{
	int index_x;
	int index_z;
	int prevHeight;
	int currHeight;
//	int dHeight;
	bool operator<(const TerrainHeightOpData& other) const
	{
		int value=index_x*(64*8+1)+index_z;
		int other_value=other.index_x*(64*8+1)+other.index_z;
		return value<other_value;
	}
	bool operator==(const TerrainHeightOpData& other) const
	{
		int value=index_x*(64*8+1)+index_z;
		int other_value=other.index_x*(64*8+1)+other.index_z;
		return value==other_value;
	}
	bool operator!=(const TerrainHeightOpData& other) const
	{
		int value=index_x*(64*8+1)+index_z;
		int other_value=other.index_x*(64*8+1)+other.index_z;
		return value!=other_value;
	}
};

class CVertexHeightOperation1 :public IOperation
{
public:
	CVertexHeightOperation1(void){}
	virtual ~CVertexHeightOperation1(void){}
public:
	virtual void undo();
	virtual void redo();
	void AddData(TerrainHeightOpData& data){m_DataVector.insert(data);}
	void Clear(){m_DataVector.clear();}
	bool IsValidate(){return m_DataVector.size()!=0;}
	bool FindData(TerrainHeightOpData& data,int& prevHeight,int& currHeight);
	void AddChunk(int chunkId){m_ChunkSet.insert(chunkId);}
	void FlushTerrainNormal();
	void FlushTerrainHeight();
private:// Data
	std::set<TerrainHeightOpData> m_DataVector;
	std::set<unsigned int> m_ChunkSet;
};

class CVertexHeightOperation2:public IOperation
{
	friend class TerrainHeightHandler;
public:
	CVertexHeightOperation2(){m_bWorld=false;}
	virtual ~CVertexHeightOperation2(){}
public:
	virtual void undo();
	virtual void redo();
	bool IsValidate();
	bool HaveData(){return m_DataVector.size()!=0;}
	void AddData(TerrainHeightOpData& data){m_DataVector.push_back(data);}
	void ReserveMemory(int size){m_DataVector.reserve(size);}
	void AddChunk(int chunkId){m_ChunkSet.insert(chunkId);}
	void FlushTerrainNormal();
	void FlushTerrainHeight();
private:// Data
	std::vector<TerrainHeightOpData> m_DataVector;
	std::set<unsigned int> m_ChunkSet;
	bool m_bWorld;
};

class CWorldHeightOperation:public IOperation
{
public:
	CWorldHeightOperation(){}
	virtual ~CWorldHeightOperation(){}
public:
	virtual void undo();
	virtual void redo();
	void SetDeltaHeight(int dHeight){m_dHeight=dHeight;}
	void FlushWorldNormal();
	void FlushWorldHeight();
private:
	int m_dHeight;
};