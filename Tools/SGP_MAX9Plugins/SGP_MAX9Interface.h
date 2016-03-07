#ifndef __SGP_MAX9Interface__H
#define __SGP_MAX9Interface__H

#include "SGP_MAX9Plugins.h"
#include "MeshNormalSpec.h"



enum
{
	MODIFIER_NONE,
	MODIFIER_SKIN,
	MODIFIER_PHYSIQUE
};

struct SGP_ColorKey
{
	float dr, dg, db, da;
	float uoffset;
	float voffset;
	SGP_ColorKey()
	{
		dr = dg = db = da = 1.0f;
		uoffset = voffset = 0.0f;
	}
	bool operator == (const SGP_ColorKey& CK) const noexcept
	{
		return (sgp::FloatCmp(dr, CK.dr) == 0) && (sgp::FloatCmp(dg, CK.dg) == 0) &&
			(sgp::FloatCmp(db, CK.db) == 0) && (sgp::FloatCmp(da, CK.da) == 0) &&
			(sgp::FloatCmp(uoffset, CK.uoffset) == 0) &&
			(sgp::FloatCmp(voffset, CK.voffset) == 0);
	}
};


class SGP_MaxInterface
{
public:
	struct ColorTrack
	{
		bool bTiling;
		bool bUVMoving;
		int nUTile, nVTile;
		int nStartFrame;
		int nLoopMode;
		float fPlaybackRate;
		float fUSpeed;
		float fVSpeed;


		sgp::Array<SGP_ColorKey> ColorKeyFrame;

		ColorTrack()
		{
			bTiling = false;
			bUVMoving = false;
			nUTile = 1;
			nVTile = 1;
			nStartFrame = 0;
			nLoopMode = 0;
			fPlaybackRate = 1.0f;
			fUSpeed = 0.0f;
			fVSpeed = 0.0f;
		}
		bool operator != (const ColorTrack& CT) const noexcept
		{
			return (bTiling != CT.bTiling) || (bUVMoving != CT.bUVMoving) ||
				(nUTile != CT.nUTile) || (nVTile != CT.nVTile) || 
				(nStartFrame != CT.nStartFrame) ||
				(nLoopMode != CT.nLoopMode) ||
				(fPlaybackRate != CT.fPlaybackRate) ||
				(fUSpeed != CT.fUSpeed) || (fVSpeed != CT.fVSpeed) ||
				(ColorKeyFrame != CT.ColorKeyFrame);
		}
	};

	struct Track
	{
		sgp::Array<bool> vectorVisible;
		sgp::Array<Point3> vectorTrans;
		sgp::Array<Quat> vectorRot;
		sgp::Array<Point3> vectorScale;
		bool bMirror;
	};


	struct Influence
	{
		int nBoneId;
		char szBoneName[32];
		float fWeight;
		Influence()
		{
			nBoneId = -1;
			szBoneName[0] = 0;
			fWeight = 0.0f;
		}
	};

	struct InfluenceNameSorter
	{
		static int compareElements (const Influence m1, const Influence m2)
		{
			return strcmp(m1.szBoneName, m2.szBoneName);
		}
	};
	struct InfluenceWeightSorter
	{
		static int compareElements (const Influence m1, const Influence m2)
		{
			if( m1.fWeight > m2.fWeight )
				return -1;
			if( m1.fWeight < m2.fWeight )
				return 1;
			return 0;
		}
	};


	struct BoneGroup
	{
		sgp::Array<Influence> vectorInfl;

		BoneGroup& operator= (const BoneGroup& other) noexcept
		{
			vectorInfl.resize(other.vectorInfl.size());
			for( int i=0; i<vectorInfl.size(); i++ )
			{
				vectorInfl.getReference(i).nBoneId = other.vectorInfl.getReference(i).nBoneId;
				vectorInfl.getReference(i).fWeight = other.vectorInfl.getReference(i).fWeight;
				strcpy(vectorInfl.getReference(i).szBoneName, other.vectorInfl.getReference(i).szBoneName);
			}
			return *this;
		}
		void AddInfluence( Influence& infl )
		{
			for( int i = 0; i < vectorInfl.size(); i++ )
			{
				if( strcmp( vectorInfl[i].szBoneName, infl.szBoneName ) == 0 )
				{
					vectorInfl.getReference(i).fWeight += infl.fWeight;
					return;
				}
			}
			InfluenceNameSorter sorter;
			vectorInfl.addSorted( sorter, infl );
		}
	};


protected:
	static DWORD WINAPI ProgressFunction(LPVOID arg);
	

protected:
	ExpInterface *m_pExpInterface;
	Interface *m_pInterface;
	

public:
	SGP_MaxInterface();
	~SGP_MaxInterface();

	// member functions
public:
	bool Create(ExpInterface *pExpInterface, Interface *pInterface);

	int GetCurrentFrame();
	float GetCurrentTime();
	int GetEndFrame();
	int GetFps();
	int GetSelectedNodeCount();
	int GetStartFrame();
	int GetStartTick();
	int GetEndTick();
	int GetTickPerFrame();
	int GetFrameCount();


	bool IsMesh(INode *pNode);
	Mesh *GetMesh(INode *pNode);

	INode* GetSelectedNode();
	INode* GetSelectedNode( int nNodeId );
	INode *GetRootNode();
	INode* GetNode( int i ){ return m_vectorNode[i]; }
	Modifier* FindModifier( INode* pINode, Class_ID id );


	bool IsBone(INode *pNode);
	bool IsBipedBone(INode *pNode);
	bool IsRenderable(INode *pNode);	
	bool IsDummy(INode *pNode);
	bool IsTarget(INode *pNode);
	bool IsSelected(INode *pNode);


	void SetProgressInfo(int percentage);
	void StartProgressInfo(MCHAR* strText);
	void StopProgressInfo();


	

	Point3 GetVertexNormal( Mesh* pMesh, int faceId, int vertexId, int FaceVertexIdx );


	void ClearNodeTree(){ m_vectorNode.clear(); }
	void GetNodeTree( INode* pNode );
	int GetNodeCount(){ return m_vectorNode.size(); }




	void GetTracks( int nNodeCount, INode** pNodes, Track** tracks );



	ViewExp* GetActiveViewPort(){ return m_pInterface->GetActiveViewport(); }


	void GetBoneGroup(  Modifier *pModifier,
						int nModifierType, 
						INode* pNode, 
						Mesh* pMesh,
						int nVertexId, 
						BoneGroup& boneGroup );

	MSTR& GetCurFilePath() { return m_pInterface->GetCurFilePath(); }

	int GetTime(){ return m_pInterface->GetTime(); }


	bool GetMtlAnim( StdMat* pStdMtl, ColorTrack& track, int nChannel  );



	bool GetStdMtlChannelBitmapFileName( StdMat* pStdMat, int nChannel, TCHAR szFileName[] );


protected:
	Matrix3 GetNodeTM(INode *pNode, float time);


	sgp::Array<INode*> m_vectorNode;
};






#endif		// __SGP_MAX9Interface__H