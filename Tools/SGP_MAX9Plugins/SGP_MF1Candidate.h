#ifndef __SGP_MF1Candidate__H
#define __SGP_MF1Candidate__H

#include "SGP_MAX9Interface.h"


class SGP_MF1Candidate
{
public:
	struct Texture
	{
		wchar_t szTextureFileFullName[256];
		Texture()
		{
			szTextureFileFullName[0] = 0;
		}
	};
	struct Textures
	{
		sgp::Array<Texture> vectorTexture;
	};

	struct Material
	{	
		int	iTextureID;
		sgp::uint32 renderFlag;
		sgp::Array<SGP_MaxInterface::ColorTrack> vectorColorTrack;
	};
	struct Materials
	{
		sgp::Array<Material> vectorMtl;
	};

	struct SplitVertex
	{
		int nVertexId;
		Point3 pos;
		Point3 normal;
		IPoint3 colorRGB;			// RGB
		float u, v;
		float u1, v1;


		SGP_MaxInterface::BoneGroup bg;

		SplitVertex()
		{
			nVertexId = -1;

			pos.x = 0.0f;
			pos.y = 0.0f;
			pos.z = 0.0f;

			normal.x = 0.0f;
			normal.y = 0.0f;
			normal.z = 0.0f;

			colorRGB.x = 255;
			colorRGB.y = 255;
			colorRGB.z = 255;

			u = 0.0f;
			v = 0.0f;
			u1 = 0.0f;
			v1 = 0.0f;			
		}
		SplitVertex& operator= (const SplitVertex& other) noexcept
		{
			nVertexId = other.nVertexId;
			pos = other.pos;
			normal = other.normal;
			colorRGB = other.colorRGB;
			u = other.u;
			v = other.v;
			u1 = other.u1;
			v1 = other.v1;
			bg = other.bg;
			return *this;
		}
	};

	struct MtlFace
	{
		sgp::uint32 nMaxVertexId[3];
	};
	struct GeomChunk
	{
		sgp::Array<Point3> vectorVertex;
		sgp::Array<Point3> vectorNormal;
		sgp::Array<MtlFace> vectorFace;
		sgp::Array<Point2> vectorUV;
		sgp::Array<Point2> vectorUV1;
		sgp::Array<sgp::uint16> vectorBGId;			// bone group ids
		sgp::Array<SplitVertex> vectorSplitVertex;
		sgp::Array<IPoint3> vectorColor;


		int nMtlId;					// Material ID in m_Materials array
		int nModifierType;			// MODIFIER Type


		sgp::AABBox  aabbox;		// Bounding box of this chunk
		sgp::uint32  nType;			// Some Logic Flag (for example floor, sand, grass etc)


		char szNodename[256];		// Chunk Name(Node Name)

		GeomChunk()
		{

			nMtlId = -1;
			nModifierType = -1;

			nType = sgp::SGPMESHCF_NORMAL;


			szNodename[0] = 0;

			aabbox.vcMin = sgp::Vector3D(0,0,0);
			aabbox.vcMax = sgp::Vector3D(0,0,0);
			aabbox.vcCenter = sgp::Vector3D(0,0,0);

		};
	};
	struct Geometry
	{
		sgp::Array<GeomChunk> vectorChunk;
	};

	struct BoneGroups
	{
		sgp::Array<SGP_MaxInterface::BoneGroup> vectorBoneGroup;
	};

	struct Bone
	{
		Bone()
		{
			memset( szName, 0x00, sizeof( szName ) );
			memset( szParentName, 0x00, sizeof( szParentName ) );
		}
		INode* pNode;
		char szName[256];
		char szParentName[256];
		int nParentId;
		sgp::Array<int> vectorChildId;
		
		SGP_MaxInterface::Track track;
	};

	struct Skeleton
	{
		sgp::Array<Bone> vectorBone;
		sgp::Array<int> vectorRootBoneId;
		int FindBone( const char* pszBoneName )
		{
			for( int i = 0; i < vectorBone.size(); i++ )
			{
				if( _stricmp( pszBoneName, vectorBone[i].szName ) == 0 )
					return i;
			}
			return -1;
		}
		bool IsParentBone( int nTryBoneID, int nBoneID )
		{
			if( nTryBoneID == nBoneID )
				return false;
			Bone& ChildBone = vectorBone.getReference(nBoneID);
			int nParentID = ChildBone.nParentId;
			if( nParentID == -1 )
				return false;
			if( nParentID == nTryBoneID )
				return true;
			return IsParentBone(nTryBoneID, nParentID);
		}
	};

	struct Particle
	{
		INode* pNode;
		int nAttachBoneID;
		char szName[256];
		sgp::Matrix4x4 matInit;
		Particle()
		{
			pNode = NULL;
			nAttachBoneID = -1;
			szName[0] = 0;
		}
	};
	struct Particles
	{
		sgp::Array<Particle> vectorParticle;
	};

	struct Attachment
	{
		INode* pNode;
		int nAttachBoneID;
		char szName[256];
		sgp::Matrix4x4 matInit;
		Attachment()
		{
			pNode = NULL;
			nAttachBoneID = -1;
			szName[0] = 0;
		}
	};
	struct Attachments
	{
		sgp::Array<Attachment> vectorAttachment;
	};

	struct INodeSorter
	{
		static int compareElements ( INode* m1, INode* m2)
		{
			if( _stricmp( m1->GetName(), m2->GetName() ) > 0 )
				return 1;
			if( _stricmp( m1->GetName(), m2->GetName() ) < 0 )
				return -1;
			return 0;
		}
	};


public:
	SGP_MF1Candidate() 
	{
		m_AABBox.vcMin = sgp::Vector3D(0,0,0);
		m_AABBox.vcMax = sgp::Vector3D(0,0,0);
		m_AABBox.vcCenter = sgp::Vector3D(0,0,0);
	}
	~SGP_MF1Candidate() { }



	int FindBoneGroup( SGP_MaxInterface::BoneGroup* infls );
	int FindTexture( Texture* pTexture );
	int FindMtl( Material* pMtl );
	int FindBone( const char* pszBoneName );
	void ProjBoneGroups();


	bool CreateGeometry( INode* pNode );
	bool CreateMF1();
	// Check MF1 File data are valid
	bool CheckMF1FileFormat();
	bool SaveMF1File( const char* pszFilename, bool bConvTexturePath = true );


private:
	class VNormal 
	{
	public:
		Point3 norm;
		sgp::uint32 smooth;
		VNormal *next;
		bool init;
		VNormal() {smooth=0;next=NULL;init=false;norm=Point3(0,0,0);}
		VNormal(Point3 &n,sgp::uint32 s) {next=NULL;init=true;norm=n;smooth=s;}
		~VNormal() 
		{
			if(next)
			{
				delete next;
				next = NULL;
			}
		}
		void AddNormal(Point3 &n, sgp::uint32 s);
		Point3 &GetNormal(sgp::uint32 s);
		void Normalize();
		void Release()
		{
			if(next)
			{
				delete next;
				next = NULL;
			}
		}
	};

	void ComputeVertexNormals(	Mesh *mesh, 
								Matrix3& tm,
								Tab<VNormal>& vnorms, 
								Tab<Point3>& fnorms );

	void CompressMaterialColorKeyFrame(sgp::Array<sgp::SGPMF1MatKeyFrame>& CKDst, const sgp::Array<SGP_ColorKey>& CKSrc);
	void CompressTrackTransKeyFrame(sgp::Array<sgp::SGPMF1KeyFrame>& CompressedTrack, const sgp::Array<Point3>& vectorTrans);
	void CompressTrackRotsKeyFrame(sgp::Array<sgp::SGPMF1KeyFrame>& CompressedTrack, const sgp::Array<Quat>& vectorRot);
	void CompressTrackScalesKeyFrame(sgp::Array<sgp::SGPMF1ScaleKeyFrame>& CompressedTrack, const sgp::Array<Point3>& vectorScale);
	void CompressTrackVisibleKeyFrame(sgp::Array<sgp::SGPMF1VisibleKeyFrame>& CompressedTrack, const sgp::Array<bool>& vectorVisible);


	// Remove no data bone, not used by any vertex and no name bones 
	void RemoveUnUsedBones();
	bool HaveBoneAnimData(const Bone& BoneSrc);

private:
	Textures	m_textures;
	Materials	m_Materials;
	Geometry	m_geometry;
	//Geometry	m_billboards;
	//Geometry	m_decals;
	BoneGroups	m_boneGroups;
	Skeleton	m_skeleton;
	Attachments	m_attachments;
	Attachments m_effattchments;

	Particles	m_Particles;

	sgp::AABBox m_AABBox;

};




#endif		// __SGP_MF1Candidate__H