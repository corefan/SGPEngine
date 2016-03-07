#pragma once

#include "IOperation.h"
#include <set>
#include <vector>
#include <map>

struct ChunkTextureOpData
{
	int chunkId;
	int nLayer;
	CString prevTexPath;
	CString currTexPath;
	bool operator < (const ChunkTextureOpData& other) const{return chunkId < other.chunkId;}
	bool operator == (const ChunkTextureOpData& other) const{return chunkId == other.chunkId;}
	bool operator != (const ChunkTextureOpData& other) const{return chunkId != other.chunkId;}
};

struct TerrainTextureOpData
{
	int index;
	int prevAlpha;
	int currAlpha;
	bool operator<(const TerrainTextureOpData& other) const{return index<other.index;}
	bool operator==(const TerrainTextureOpData& other) const{return index==other.index;}
	bool operator!=(const TerrainTextureOpData& other) const{return index!=other.index;}
};

// ChunkTextureOperation
class ChunkTextureOperation : public IOperation
{
	friend class TerrainTextureOperation;
public:
	ChunkTextureOperation();
	~ChunkTextureOperation();
public:
	virtual void undo();
	virtual void redo();
	void SetChunkTexture(int chunkId,int nLayer,CString texPath);
	bool IsValidate(){return m_DataSet.size()!=0;}
	void FlushColorMiniMap();
	void AddChunk(int chunkId){m_ChunkIdSet.insert(chunkId);}
	bool IsContainChunk(int chunkId);
private:
	std::set<ChunkTextureOpData> m_DataSet;
	std::set<int> m_ChunkIdSet;
};

// TerrainTextureOperation
class TerrainTextureOperation : public IOperation
{
public:
	TerrainTextureOperation();
	virtual ~TerrainTextureOperation();
public:
	virtual void undo();
	virtual void redo();
	bool FindData(TerrainTextureOpData& data,int& prevAlpha,int& currAlpha);
	void AddData(TerrainTextureOpData& data);
	void ContainRect(int minx,int maxx,int minz,int maxz);
	void SetLayer(int nLayer){m_nLayer=nLayer;}
	void SetChunkTexOperation(ChunkTextureOperation* pChunkTexOp){m_pChunkTexOp=pChunkTexOp;}
	void FlushColorMiniMap();
	void AddChunk(int chunkId){m_ChunkIdSet.insert(chunkId);}
	void CheckOpAfterDelTex();
	bool IsValidate(){return !m_DataVector.empty();}
private:
	std::set<TerrainTextureOpData> m_DataVector;
	std::set<int> m_ChunkIdSet;
	ChunkTextureOperation* m_pChunkTexOp;
	int m_minx;
	int m_maxx;
	int m_minz;
	int m_maxz;
	int m_nLayer;
};
