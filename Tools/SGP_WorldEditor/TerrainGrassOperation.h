#pragma once

#include <vector>
#include <set>
#include "IOperation.h"
#include "..\SGPLibraryCode\SGPHeader.h"

// stl::map ??

struct GrassOpData
{
	int chunkIndex;
	SGPGrassCluster* m_pGrassPrevData;
	SGPGrassCluster* m_pGrassCurrData;

	GrassOpData():chunkIndex(-1),m_pGrassPrevData(NULL),m_pGrassCurrData(NULL){}
	~GrassOpData(){}
};

class TerrainGrassOperation : public IOperation
{
public:
	TerrainGrassOperation();
	virtual ~TerrainGrassOperation();
public:
	virtual void undo();
	virtual void redo();
	virtual bool IsGrassOperation(){return true;}
	bool CheckValidate();
	void SavePrevGrassData(int pos);
	void SaveCurrGrassData();
	bool GetChunkDataPosition(int chunkId,int& pos);
private:
	bool m_Add;
	std::vector<GrassOpData> m_DataVector;
};
