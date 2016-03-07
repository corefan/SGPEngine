#include "SGP_MAX9Plugins.h"
#include "SGP_MAX9ControlPanel.h"
#include "SGP_MF1Candidate.h"

#include "MessageProcess.h"

void SGP_MF1Candidate::VNormal::AddNormal(Point3 &n,sgp::uint32 s) 
{
	if (!(s&smooth) && init) 
	{
		if (next) next->AddNormal(n,s);
		else 
		{
			next = new VNormal(n,s);
		}
	} 
	else 
	{
		norm += n;
		smooth |= s;
		init = true;
	}
}
// Retrieves a normal if the smoothing groups overlap or there is 
// only one in the list

Point3 &SGP_MF1Candidate::VNormal::GetNormal(sgp::uint32 s) 
{
	if (smooth&s || !next) 
		return norm;
	else 
		return next->GetNormal(s); 
}
// Normalize each normal in the list
void SGP_MF1Candidate::VNormal::Normalize() 
{
	VNormal *ptr = next, *prev = this;
	while (ptr) 
	{
		if (ptr->smooth&smooth) 
		{
			norm += ptr->norm;
			prev->next = ptr->next;
			delete ptr;
			ptr = prev->next;
		} 
		else 
		{
			prev = ptr;
			ptr = ptr->next;
		}
	}
	norm = ::Normalize(norm);
	if (next) 
		next->Normalize();
}


int SGP_MF1Candidate::FindBoneGroup( SGP_MaxInterface::BoneGroup* infls )
{	
	for( int i = 0; i < m_boneGroups.vectorBoneGroup.size(); i++ )
	{
		if( m_boneGroups.vectorBoneGroup[i].vectorInfl.size() != infls->vectorInfl.size() )
			continue;

		bool bFound = true;
		for( int id = 0; id < m_boneGroups.vectorBoneGroup[i].vectorInfl.size(); id++ )
		{
			if( strcmp( m_boneGroups.vectorBoneGroup[i].vectorInfl[id].szBoneName, infls->vectorInfl[id].szBoneName ) != 0
				|| sgp::FloatCmp( m_boneGroups.vectorBoneGroup[i].vectorInfl[id].fWeight, infls->vectorInfl[id].fWeight ) != 0 )
			{
				bFound = false;
				break;
			}
		}

		if( bFound )
			return i;
	}
	return -1;
}

int SGP_MF1Candidate::FindTexture( Texture* pTexture )
{
	int length = wcslen(pTexture->szTextureFileFullName);
	for( int i = 0; i < m_textures.vectorTexture.size(); i++ )
	{
		if( _wcsnicmp( m_textures.vectorTexture[i].szTextureFileFullName, pTexture->szTextureFileFullName, length ) == 0 )
			return i;
	}
	return -1;
}

int SGP_MF1Candidate::FindMtl( Material* pMtl )
{
	for( int i = 0; i < m_Materials.vectorMtl.size(); i++ )
	{
		Material* pMtlSrc = &m_Materials.vectorMtl.getReference(i);

		if( pMtlSrc->vectorColorTrack.size() != pMtl->vectorColorTrack.size() )
			continue;
		if( pMtlSrc->renderFlag != pMtl->renderFlag )
			continue;
		if( pMtlSrc->iTextureID != pMtl->iTextureID )
			continue;
		
		bool bSame = true;
		for( int j = 0; j < pMtlSrc->vectorColorTrack.size(); j++ )
		{
			SGP_MaxInterface::ColorTrack& TrackSrc = pMtlSrc->vectorColorTrack.getReference(j);
			SGP_MaxInterface::ColorTrack& TrackDst = pMtl->vectorColorTrack.getReference(j);
			if( TrackSrc != TrackDst )
			{
				bSame = false;
				break;
			}
		}

		if( !bSame )
			continue;

		return i;
	}
	return -1;
}

int SGP_MF1Candidate::FindBone( const char* pszBoneName )
{
	for( int i = 0; i < m_skeleton.vectorBone.size(); i++ )
	{
		if( strcmp( m_skeleton.vectorBone[i].szName, pszBoneName ) == 0 )
			return i;
	}
	return -1;
}

void SGP_MF1Candidate::ProjBoneGroups()
{
	for( int i = 0; i < m_boneGroups.vectorBoneGroup.size(); i++ )
	{
		SGP_MaxInterface::BoneGroup& bg = m_boneGroups.vectorBoneGroup.getReference(i);
		for( int j = 0; j < bg.vectorInfl.size(); j++ )
		{
			bg.vectorInfl.getReference(j).nBoneId = FindBone( bg.vectorInfl.getReference(j).szBoneName );
		}
	}
}












void SGP_MF1Candidate::ComputeVertexNormals( Mesh *mesh, 
	Matrix3& tm, Tab<VNormal>& vnorms, Tab<Point3>& fnorms )
{
	SGP_MaxInterface* ip = g_SGPMax9CtrlPanel.GetMaxIP();
	ip->StartProgressInfo( _M("Compute Vertex Normals") );
	
	
	Face *face; 
	Point3 *verts;
	Point3 v0, v1, v2;
	face = mesh->faces; 
	verts = mesh->verts;
	Matrix3 rot = tm;

	rot.NoTrans();
	rot.NoScale();
	
	vnorms.SetCount(mesh->getNumVerts());
	fnorms.SetCount(mesh->getNumFaces());

	int i = 0;
	// Compute face and vertex surface normals
	for (i = 0; i < mesh->getNumVerts(); i++) 
	{
		vnorms[i] = VNormal();
	}
	for(i = 0; i < mesh->getNumFaces(); i++, face++) 
	{
		// Calculate the surface normal
		if( i % 10 == 0)
			ip->SetProgressInfo( 100.0f*i/mesh->getNumFaces() );
		
		v0 = verts[face->v[0]];
		v1 = verts[face->v[1]];
		v2 = verts[face->v[2]];

		fnorms[i] = (v1-v0)^(v2-v1);
		fnorms[i] = rot*fnorms[i];
		for (int j=0; j<3; j++) 
		{  
			vnorms[face->v[j]].AddNormal(fnorms[i],face->smGroup);
		}
		fnorms[i] = Normalize(fnorms[i]);
	}
	int n = vnorms.Count();
	for (i=0; i < mesh->getNumVerts(); i++) 
	{
		vnorms[i].Normalize();
	}


	ip->StopProgressInfo();
}

bool SGP_MF1Candidate::CreateGeometry( INode* pNode )
{
	SGP_MaxInterface* ip = g_SGPMax9CtrlPanel.GetMaxIP();
	
	if( !pNode )
		return false;
	if( !ip->IsMesh( pNode ) )
		return false;
	if( !ip->IsRenderable( pNode ) )
		return false;
	if( ip->IsBone( pNode ) )
		return false;
	if( ip->IsDummy( pNode ) )
		return false;
	Mesh* mesh = ip->GetMesh( pNode );
	if( !mesh )
		return false;



	ip->StartProgressInfo( _M("Getting Material") );
	Modifier* pModifier = ip->FindModifier( pNode, Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B) );
	int nModifierType;
	if(	pModifier )
		nModifierType = MODIFIER_PHYSIQUE;
	else
	{
		pModifier = ip->FindModifier( pNode, SKIN_CLASSID	);
		if(	pModifier )
			nModifierType = MODIFIER_SKIN;
		else
			nModifierType = MODIFIER_NONE;
	}


	Mtl* pMtl = pNode->GetMtl();

	sgp::Array<StdMat*> vectorStdMtl;
	if( pMtl )
	{
#if 0
		if( pMtl->IsMultiMtl() )
		{
			for( int i = 0; i < pMtl->NumSubMtls(); i++ )
			{
				Mtl* pSubMtl = pMtl->GetSubMtl( i );
				StdMat* pStdMtl = (StdMat*)pSubMtl;
				if( pStdMtl == NULL )
					return;
				vectorStdMtl.add( pStdMtl );
			}
		}
		else
#endif
		{
			StdMat* pStdMtl = (StdMat*)pMtl;
			vectorStdMtl.add( pStdMtl );
		}
	}
	int nStdMtlCount = vectorStdMtl.size();
	sgp::Array<GeomChunk> chunks;
	if( nStdMtlCount > 0 )
	{
		chunks.resize( nStdMtlCount );
		for( int i = 0; i < chunks.size(); i++ )
		{
			strcpy( chunks.getReference(i).szNodename, pNode->GetName() );
		}
	}
	else
	{
		chunks.resize( 1 );
		strcpy( chunks.getReference(0).szNodename, pNode->GetName() );
#if 0
		if( _strnicmp( "floor", pNode->GetName(), 5 ) == 0 )
        {
			chunks[0].nType = HR3D_Mdx2GeoChunkProperty::eFloor;
            _tstringA strNodeName(chunks[0].szNodename);
            strNodeName.MakeLower();
            if (strNodeName.Find("stone",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Stone;
            }
            else if (strNodeName.Find("grass",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Grass;
            }
            else if (strNodeName.Find("snow",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Snow;
            }
            else if (strNodeName.Find("soil",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Soil;
            }
            else if (strNodeName.Find("water",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Water;
            }
            else if (strNodeName.Find("wood",5) != _tstringA::npos)
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Wood;
            }
            else
            {
                chunks[0].dwPhysicType = eFloorPhysicType_Max;
            }
        }
		if( _strnicmp( "coli", pNode->GetName(), 4 ) == 0 )
			chunks[0].nType = HR3D_Mdx2GeoChunkProperty::eCollision;
#endif
	}
	

	for( int nStdMtlId = 0; nStdMtlId < vectorStdMtl.size(); nStdMtlId++ )
	{
		StdMat* pStdMtl = vectorStdMtl[nStdMtlId];

		if( !pStdMtl )
		{
			chunks.getReference(nStdMtlId).nMtlId = -1;
		}
		else
		{
			SGP_MaxInterface::ColorTrack colorTrack;			

			bool bAlphaBlend = false;
			bool bAlphaTest = false;

			TCHAR szDiffuseMap[256] = "";
			TCHAR szTransMap[256] = "";

			Texture texture;
			if( ip->GetStdMtlChannelBitmapFileName( pStdMtl, ID_OP, szTransMap ) )
			{
				wchar_t wsTransMap[256];	
				int nc = strlen(szTransMap);
				int n = MultiByteToWideChar(CP_ACP, 0, szTransMap, nc, NULL, 0);
				MultiByteToWideChar(CP_ACP, 0, szTransMap, nc, wsTransMap, n);
				wsTransMap[n] = 0;

				bAlphaBlend = true/*sgp::File(sgp::String(wsTransMap)).exists()*/;

				if( ip->GetStdMtlChannelBitmapFileName( pStdMtl, ID_DI, szDiffuseMap ) )
				{
					wchar_t wsDiffuseMap[256];
					int nc = strlen(szDiffuseMap);
					int n = MultiByteToWideChar(CP_ACP, 0, szDiffuseMap, nc, NULL, 0);
					MultiByteToWideChar(CP_ACP, 0, szDiffuseMap, nc, wsDiffuseMap, n);
					wsDiffuseMap[n] = 0;

					bAlphaTest = true/*sgp::File(sgp::String(wsDiffuseMap)).exists()*/;
					ip->GetMtlAnim( pStdMtl, colorTrack, ID_DI );
					wcscpy( texture.szTextureFileFullName, wsDiffuseMap );
				}
				else
				{
					ip->GetMtlAnim( pStdMtl, colorTrack, ID_OP );
					wcscpy( texture.szTextureFileFullName, wsTransMap );
				}
			}
			else
			{
				if( ip->GetStdMtlChannelBitmapFileName( pStdMtl, ID_DI, szDiffuseMap ) )
				{
					wchar_t wsDiffuseMap[256];
					int nc = strlen(szDiffuseMap);
					int n = MultiByteToWideChar(CP_ACP, 0, szDiffuseMap, nc, NULL, 0);
					MultiByteToWideChar(CP_ACP, 0, szDiffuseMap, nc, wsDiffuseMap, n);
					wsDiffuseMap[n] = 0;


					ip->GetMtlAnim( pStdMtl, colorTrack, ID_DI );
					wcscpy( texture.szTextureFileFullName, wsDiffuseMap );
				}
			}

			int nTextureId = FindTexture( &texture );
			if( nTextureId == -1 )
			{
				nTextureId = m_textures.vectorTexture.size();
				m_textures.vectorTexture.add(texture);				
			}



			Material mtl;
			mtl.renderFlag = sgp::SGPMESHRF_NONE;
			mtl.iTextureID = nTextureId;

			if( pStdMtl->GetTwoSided() )
				mtl.renderFlag |= sgp::SGPMESHRF_TWOSIDED;

			if( bAlphaTest )
				mtl.renderFlag |= sgp::SGPMESHRF_ALPHATEST;
			else if( bAlphaBlend )
				mtl.renderFlag |= sgp::SGPMESHRF_ALPHABLEND;

			mtl.vectorColorTrack.add( colorTrack );

			int nNextMtlId = FindMtl( &mtl );
			if( nNextMtlId == -1 )
			{
				nNextMtlId = m_Materials.vectorMtl.size();
				m_Materials.vectorMtl.add( mtl );
			}
			chunks.getReference(nStdMtlId).nMtlId = nNextMtlId;
		}
	
			
		


		const char* pszNodeName = pNode->GetName();
		const char* LastNameStr = strrchr(pszNodeName, '_');
		if( LastNameStr && _strnicmp( "_bump", LastNameStr, 5 ) == 0 )
		{
			chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_BUMP;
		}

		if( _strnicmp( "bbrd", pszNodeName, 4 ) == 0 )
		{
			// Normal BBRD, Look towards the camera plane (fast)
			chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_BBRD;
			if( _strnicmp( "bbrdc", pszNodeName, 5 ) == 0 )
			{
				// Look towards the camera point (better effect but more expensive)
				chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_BBRD_CAMERAPOINT;
			}
			if( _strnicmp( "bbrdv", pszNodeName, 5 ) == 0 )
			{
				// Y Axial locked Billboard (always stand vertical)
				chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_BBRD_VERTICALGROUND;
			}
			//if( _strnicmp("bbrda",pszNodeName,5) == 0)
			//{
			//	chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_BBRD_APERTURE;
			//}

		}
		else if( _strnicmp( "avmesh", pszNodeName, 6 ) == 0 )
			chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_AVMESH;
		else if( _strnicmp( "decal", pszNodeName, 5 ) == 0 )
			chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_DECAL;
		else
			chunks.getReference(nStdMtlId).nType = sgp::SGPMESHCF_NORMAL;
	}

	ip->StopProgressInfo();











	mesh->buildNormals();
	mesh->buildRenderNormals();

	Tab<VNormal> vnorms;
	Tab<Point3> fnorms;
	Matrix3 matrix = pNode->GetObjTMAfterWSM ( 0 );
	Matrix3	matTrans = matrix;
	ComputeVertexNormals( mesh, matrix, vnorms, fnorms );

	// 判断是否为镜像物体
	bool bMirror = DotProd ( CrossProd ( matrix.GetRow ( 0 ), matrix.GetRow ( 1 ) ), matrix.GetRow ( 2 ) ) < 0.0 ? true : false;


	ip->StartProgressInfo( _M("Getting Mesh") );
	for( int nFaceId = 0; nFaceId < mesh->numFaces; nFaceId++ )
	{
		if( nFaceId % 10 == 0 )
			ip->SetProgressInfo( 100.0f*nFaceId/mesh->numFaces );
		
		MtlFace face;
		GeomChunk* pChunk = NULL;
		if( nStdMtlCount > 0 )
		{
			int nStdMtlId = mesh->getFaceMtlIndex( nFaceId ) % nStdMtlCount;
			pChunk = &chunks.getReference(nStdMtlId);
		}
		else
		{
			pChunk = &chunks.getReference(0);
		}

		for( int nVertexId = 0; nVertexId < 3; nVertexId++ )
		{
			float u = 0.0f;
			float v = 0.0f;

			float u1 = 0.0f;
			float v1 = 0.0f;

			if( mesh->tvFace )
			{
				int nOffset = mesh->tvFace[nFaceId].t[nVertexId];
				u = mesh->tVerts[nOffset].x;
				v = 1 - mesh->tVerts[nOffset].y;

				if( mesh->numMaps >= 3 )
				{
					nOffset = mesh->mapFaces( 2 )[nFaceId].t[nVertexId];
					u1 = mesh->mapVerts( 2 )[nOffset].x;
					v1 = 1 - mesh->mapVerts( 2 )[nOffset].y;
				}
			}

			int nOldVertexId = mesh->faces[nFaceId].getVert(nVertexId);
			
			// 如果导出时选择了修改法线选项，则输出支持Smooth Group的顶点法线
			// 否则输出自己计算的顶点法线（调用ComputeVertexNormals函数遍历所有的面法线计算）
			Point3 normal;
			if( 1 )
			{
				normal = ip->GetVertexNormal( mesh, nFaceId, nOldVertexId, nVertexId );
				// 若需要导出Max的Render Normal，Normal向量需要同时变换
				Matrix3 NormalTransform = matTrans;
				NormalTransform.SetRow( 3, Point3(0.0f, 0.0f, 0.0f) );
				normal = normal * NormalTransform;
				normal = normal.Normalize();
			}
			else
				normal = vnorms[nOldVertexId].GetNormal( mesh->faces[nFaceId].smGroup );

			IPoint3 dwColorRGB(255,255,255);
			if( mesh->vertCol )
			{
				int nVCVertexID = mesh->vcFace[nFaceId].t[nVertexId];
				Point3 color = mesh->vertCol[nVCVertexID];

				dwColorRGB.x = int(color.x*255);
				dwColorRGB.y = int(color.y*255);
				dwColorRGB.z = int(color.z*255);
			}
			else if( mesh->vertColArray )
			{
				int nVCVertexID = mesh->vcFaceData[nFaceId].t[nVertexId];
				Point3 color = mesh->vertColArray[nVCVertexID];
				dwColorRGB.x = int(color.x*255);
				dwColorRGB.y = int(color.y*255);
				dwColorRGB.z = int(color.z*255);
			}
	
			bool found = false;
			for( int i = 0; i < pChunk->vectorSplitVertex.size(); i++ )
			{
				if( nOldVertexId == pChunk->vectorSplitVertex[i].nVertexId &&
					dwColorRGB == pChunk->vectorSplitVertex[i].colorRGB &&
					sgp::FloatCmp( u, pChunk->vectorSplitVertex[i].u ) == 0 &&
					sgp::FloatCmp( v, pChunk->vectorSplitVertex[i].v ) == 0 &&
					sgp::FloatCmp( u1, pChunk->vectorSplitVertex[i].u1 ) == 0 &&
					sgp::FloatCmp( v1, pChunk->vectorSplitVertex[i].v1 ) == 0 &&
					sgp::FloatCmp( normal.x, pChunk->vectorSplitVertex[i].normal.x ) == 0 && 
					sgp::FloatCmp( normal.y, pChunk->vectorSplitVertex[i].normal.y ) == 0 && 
					sgp::FloatCmp( normal.z, pChunk->vectorSplitVertex[i].normal.z ) == 0 )
				{
					found = true;
					face.nMaxVertexId[nVertexId] = i;
					break;
				}
			}

			if( !found )
			{
				// 新的索引
				face.nMaxVertexId[nVertexId] = pChunk->vectorSplitVertex.size();
				if( face.nMaxVertexId[nVertexId] > 65535 )
				{
					MessageBox(NULL, TEXT("结点Chunk顶点数超过65535，无法保存！！"), TEXT("error"), MB_OK );
					return false;
				}
				// 加入新的顶点
				SplitVertex s;
				s.nVertexId = nOldVertexId;
				s.pos = mesh->getVert( nOldVertexId )*matTrans;
				s.normal = normal;
				s.colorRGB = dwColorRGB;
				s.u = u;
				s.v = v;

				s.u1 = u1;
				s.v1 = v1;
				pChunk->vectorSplitVertex.add( s );
			}
		}

		if( bMirror )
		{
			sgp::swapVariables(face.nMaxVertexId[0], face.nMaxVertexId[1]);
		}

		pChunk->vectorFace.add( face );
	}

	//Delete Normal
	for( int i=0; i<vnorms.Count(); i++ )
	{
		vnorms[i].Release();
	}

	// Dealing with Double-sided Material
	for( int nChunkId = 0; nChunkId < chunks.size(); nChunkId++ )
	{
		GeomChunk* pChunk = &chunks.getReference(nChunkId);
		int nSplitVertexCount = pChunk->vectorSplitVertex.size();

		if( pChunk->nMtlId != -1 )
		{
			assert( pChunk->nMtlId < m_Materials.vectorMtl.size() );
			sgp::uint32 dwFlag = m_Materials.vectorMtl[pChunk->nMtlId].renderFlag;
			if( dwFlag & sgp::SGPMESHRF_TWOSIDED )
			{
				sgp::Array<SplitVertex> DupSplitVertex = pChunk->vectorSplitVertex;
				sgp::Array<MtlFace> DupFace = pChunk->vectorFace;
				for( int i = 0; i < DupSplitVertex.size(); i++ )
				{
					DupSplitVertex.getReference(i).normal = -pChunk->vectorSplitVertex.getReference(i).normal;
				}
				for( int j = 0; j < DupFace.size(); j++ )
				{
					DupFace.getReference(j).nMaxVertexId[0] = 
						nSplitVertexCount + pChunk->vectorFace.getReference(j).nMaxVertexId[1];
					DupFace.getReference(j).nMaxVertexId[1] = 
						nSplitVertexCount + pChunk->vectorFace.getReference(j).nMaxVertexId[0];
					DupFace.getReference(j).nMaxVertexId[2] = 
						nSplitVertexCount + pChunk->vectorFace.getReference(j).nMaxVertexId[2];
				}
				pChunk->vectorSplitVertex.addArray(DupSplitVertex);
				pChunk->vectorFace.addArray(DupFace);
			}
		}
	}

	ip->StopProgressInfo();




	ip->StartProgressInfo( _M("Building Chunk") );
	for( int nChunkId = 0; nChunkId < chunks.size(); nChunkId++ )
	{
		GeomChunk* pChunk = &chunks.getReference(nChunkId);

		for( int i = 0; i < pChunk->vectorFace.size(); i++ )
		{
			MtlFace* face = &pChunk->vectorFace.getReference(i);
			if( face->nMaxVertexId[0] > pChunk->vectorSplitVertex.size() ||
				face->nMaxVertexId[1] > pChunk->vectorSplitVertex.size() ||
				face->nMaxVertexId[2] > pChunk->vectorSplitVertex.size() )
			{
				assert( false && "chunk error, face index > vertex count" );
				return false;
			}
		}



		int nSplitVertexCount = pChunk->vectorSplitVertex.size();

		pChunk->vectorVertex.resize( nSplitVertexCount );
		pChunk->vectorNormal.resize( nSplitVertexCount );
		pChunk->vectorColor.resize( nSplitVertexCount );
		pChunk->vectorUV.resize( nSplitVertexCount );
		pChunk->vectorUV1.resize( nSplitVertexCount );
		pChunk->vectorBGId.resize( nSplitVertexCount );



		for( int i = 0; i < pChunk->vectorSplitVertex.size(); i++ )
		{
			if( i % 10 == 0 )
			{
				ip->SetProgressInfo( 100.0f*i/pChunk->vectorSplitVertex.size() );
			}

			// MF1Candidate Bounding Box AABB Update
			m_AABBox += sgp::Vector3D( pChunk->vectorSplitVertex[i].pos.x, pChunk->vectorSplitVertex[i].pos.y, pChunk->vectorSplitVertex[i].pos.z );

			// GeomChunk Bounding Box AABB Update
			pChunk->aabbox += sgp::Vector3D( pChunk->vectorSplitVertex[i].pos.x, pChunk->vectorSplitVertex[i].pos.y,  pChunk->vectorSplitVertex[i].pos.z );
			
			pChunk->vectorVertex.getReference(i) = pChunk->vectorSplitVertex[i].pos;
			pChunk->vectorNormal.getReference(i) = pChunk->vectorSplitVertex[i].normal;
			pChunk->vectorColor.getReference(i) = pChunk->vectorSplitVertex[i].colorRGB;


			pChunk->vectorUV.getReference(i).x = pChunk->vectorSplitVertex[i].u;
			pChunk->vectorUV.getReference(i).y = pChunk->vectorSplitVertex[i].v;

			pChunk->vectorUV1.getReference(i).x = pChunk->vectorSplitVertex[i].u1;
			pChunk->vectorUV1.getReference(i).y = pChunk->vectorSplitVertex[i].v1;

			SGP_MaxInterface::BoneGroup bg;
			ip->GetBoneGroup( 
				pModifier, 
				nModifierType, 
				pNode, 
				mesh,
				pChunk->vectorSplitVertex[i].nVertexId, bg );

			if( bg.vectorInfl.size() > 4 )
			{
				MessageBox( 0, TEXT("There are vertex in Chunk be influenced by more than 4 bones. skip more than 4 bones"), TEXT("warning"), MB_OK );
				SGP_MaxInterface::InfluenceWeightSorter sorter;
				bg.vectorInfl.sort( sorter );
				bg.vectorInfl.resize( 4 );

				// Normalize weight
				float sum = bg.vectorInfl.getReference(0).fWeight + 
					bg.vectorInfl.getReference(1).fWeight +
					bg.vectorInfl.getReference(2).fWeight +
					bg.vectorInfl.getReference(3).fWeight;
				bg.vectorInfl.getReference(0).fWeight = bg.vectorInfl.getReference(0).fWeight / sum;
				bg.vectorInfl.getReference(1).fWeight = bg.vectorInfl.getReference(1).fWeight / sum;
				bg.vectorInfl.getReference(2).fWeight = bg.vectorInfl.getReference(2).fWeight / sum;
				bg.vectorInfl.getReference(3).fWeight = bg.vectorInfl.getReference(3).fWeight / sum;

			}
			if( bg.vectorInfl.size() != 0 )
			{
				int nBGId = FindBoneGroup( &bg );
				// 如果没有找到相同的bone group
				if( nBGId == -1 )
				{
					nBGId = m_boneGroups.vectorBoneGroup.size();
					m_boneGroups.vectorBoneGroup.add( bg );
				}
				if( nBGId >= 65534 )
				{
					MessageBox( 0, TEXT("Bone Group Count > 65534, this mesh skip"), TEXT("error"), MB_OK );
					return false;
				}
	
				pChunk->vectorBGId.getReference(i) = (sgp::uint16)nBGId;
			}
			// static mesh
			else
			{
				TCHAR WarnString[512];
				_stprintf(WarnString, "There are vertices not in any Bone Group, skip this mesh: %s", pChunk->szNodename);
				
				MessageBox( 0, WarnString, TEXT("error"), MB_OK );
				return false;

				//pChunk->vectorBGId.getReference(i) = (sgp::uint16)0xFFFF;
			}
			// 每个split vertex保留一份骨骼影响
			pChunk->vectorSplitVertex.getReference(i).bg = bg;
		}

		switch( pChunk->nType )
		{
		case sgp::SGPMESHCF_BBRD:
		case sgp::SGPMESHCF_BBRD_CAMERAPOINT:
		case sgp::SGPMESHCF_BBRD_VERTICALGROUND:
		//case sgp::SGPMESHCF_BBRD_APERTURE:
			if( pChunk->vectorSplitVertex.size() != 4 ||
				pChunk->vectorFace.size() != 2 )
			{
				MessageBox( NULL, TEXT("billboard's vertex count != 4 or face count != 2"), TEXT("error"), MB_OK );
				return false;
			}
			{
				// making The billboard vertex be effective
				Point3 dx = pChunk->vectorVertex.getReference(2) - pChunk->vectorVertex.getReference(0);
				Point3 dy = pChunk->vectorVertex.getReference(1) - pChunk->vectorVertex.getReference(0);
				pChunk->vectorVertex.getReference(0).Set( 
					pChunk->aabbox.vcCenter.x - dx.Length() * 0.5f,
					pChunk->aabbox.vcCenter.y,
					pChunk->aabbox.vcCenter.z + dy.Length() * 0.5f );
				pChunk->vectorVertex.getReference(1).Set( 
					pChunk->aabbox.vcCenter.x - dx.Length() * 0.5f,
					pChunk->aabbox.vcCenter.y,
					pChunk->aabbox.vcCenter.z - dy.Length() * 0.5f );
				pChunk->vectorVertex.getReference(2).Set( 
					pChunk->aabbox.vcCenter.x + dx.Length() * 0.5f,
					pChunk->aabbox.vcCenter.y,
					pChunk->aabbox.vcCenter.z + dy.Length() * 0.5f );
				pChunk->vectorVertex.getReference(3).Set( 
					pChunk->aabbox.vcCenter.x + dx.Length() * 0.5f,
					pChunk->aabbox.vcCenter.y,
					pChunk->aabbox.vcCenter.z - dy.Length() * 0.5f );
				pChunk->aabbox.vcMin.Set(
					pChunk->vectorVertex.getReference(1).x,
					pChunk->vectorVertex.getReference(1).y,
					pChunk->vectorVertex.getReference(1).z );
				pChunk->aabbox.vcMax.Set(
					pChunk->vectorVertex.getReference(2).x,
					pChunk->vectorVertex.getReference(2).y,
					pChunk->vectorVertex.getReference(2).z );
				for(int i=0; i<4; i++)
				{
					pChunk->vectorNormal.getReference(i).x = 0.0f;
					pChunk->vectorNormal.getReference(i).y = -1.0f;
					pChunk->vectorNormal.getReference(i).z = 0.0f;
				}
			}

			//m_billboards.vectorChunk.add( *pChunk );
			m_geometry.vectorChunk.add( *pChunk );
			break;
		case sgp::SGPMESHCF_DECAL:
			if( pChunk->vectorSplitVertex.size() != 4 ||
				pChunk->vectorFace.size() != 2 )
			{
				MessageBox( NULL, TEXT("decal's vertex count != 4 or face count != 2"), TEXT("warning"), MB_OK );
			}
			//m_decals.vectorChunk.add( *pChunk );
			m_geometry.vectorChunk.add( *pChunk );
			break;
		default:
			m_geometry.vectorChunk.add( *pChunk );
			break;
		}
	}
	ip->StopProgressInfo();

	return true;
}

bool SGP_MF1Candidate::CreateMF1()
{
	SGP_MaxInterface* ip = g_SGPMax9CtrlPanel.GetMaxIP();
	ip->ClearNodeTree();
	INode* pRootNode = NULL;
	ip->GetNodeTree( pRootNode );

	ip->StartProgressInfo(_M("Get All Node..."));

	int nNodeCount = ip->GetNodeCount();
	sgp::Array<INode*> vectorBone;
	for( int i = 0; i < nNodeCount; i++ )
	{
		ip->SetProgressInfo( 100.0f*i/nNodeCount );

		INode* pNode = ip->GetNode( i );

		char szName[256];
		strcpy( szName, pNode->GetName() );

		// 如果是骨骼，或者是特定的辅助物体
		if( ip->IsBone( pNode ) || ip->IsBipedBone( pNode ) )
		{
			// attachment物体，不计算为骨骼，辅助物体
			if( _strnicmp( szName, "attc", 4 ) == 0 )
			{
				Attachment attachment;
				attachment.pNode = pNode;
				strcpy( attachment.szName, pNode->GetName() );
				Matrix3 tm = pNode->GetObjTMAfterWSM ( 0 );

				// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
				// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
				// Node Transform Matrix should be swaped.
				/*
				If your matrix looks like this:
				{ rx, ry, rz, 0 }  
				{ ux, uy, uz, 0 }  
				{ lx, ly, lz, 0 }  
				{ px, py, pz, 1 }
				To change it from left to right or right to left, flip it like this:
				{ rx, rz, ry, 0 }  
				{ lx, lz, ly, 0 }  
				{ ux, uz, uy, 0 }  
				{ px, pz, py, 1 }
				*/

				attachment.matInit.Identity();
				Point3 row0 = tm.GetRow(0);
				Point3 row1 = tm.GetRow(1);
				Point3 row2 = tm.GetRow(2);
				Point3 row3 = tm.GetRow(3);
				sgp::swapVariables( row0.y,  row0.z );
				sgp::swapVariables( row1.x,  row2.x );
				sgp::swapVariables( row1.y,  row2.z );
				sgp::swapVariables( row1.z,  row2.y );
				sgp::swapVariables( row3.y,  row3.z );

				attachment.matInit._11 = row0.x;
				attachment.matInit._12 = row0.y;
				attachment.matInit._13 = row0.z;
				attachment.matInit._21 = row1.x;
				attachment.matInit._22 = row1.y;
				attachment.matInit._23 = row1.z;
				attachment.matInit._31 = row2.x;
				attachment.matInit._32 = row2.y;
				attachment.matInit._33 = row2.z;
				attachment.matInit._41 = row3.x * MessageProcess::GetInstance()->m_fZoomRatio;
				attachment.matInit._42 = row3.y * MessageProcess::GetInstance()->m_fZoomRatio;
				attachment.matInit._43 = row3.z * MessageProcess::GetInstance()->m_fZoomRatio;

				m_attachments.vectorAttachment.add( attachment );
			}
			else if( _strnicmp( szName, "ettc", 4 ) == 0 )
			{
				Attachment attachment;
				attachment.pNode = pNode;
				strcpy( attachment.szName, pNode->GetName() );
				Matrix3 tm = pNode->GetObjTMAfterWSM ( 0 );

				// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
				// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
				// Node Transform Matrix should be swaped.
				/*
				If your matrix looks like this:
				{ rx, ry, rz, 0 }  
				{ ux, uy, uz, 0 }  
				{ lx, ly, lz, 0 }  
				{ px, py, pz, 1 }
				To change it from left to right or right to left, flip it like this:
				{ rx, rz, ry, 0 }  
				{ lx, lz, ly, 0 }  
				{ ux, uz, uy, 0 }  
				{ px, pz, py, 1 }
				*/

				attachment.matInit.Identity();
				Point3 row0 = tm.GetRow(0);
				Point3 row1 = tm.GetRow(1);
				Point3 row2 = tm.GetRow(2);
				Point3 row3 = tm.GetRow(3);
				sgp::swapVariables( row0.y,  row0.z );
				sgp::swapVariables( row1.x,  row2.x );
				sgp::swapVariables( row1.y,  row2.z );
				sgp::swapVariables( row1.z,  row2.y );
				sgp::swapVariables( row3.y,  row3.z );

				attachment.matInit._11 = row0.x;
				attachment.matInit._12 = row0.y;
				attachment.matInit._13 = row0.z;
				attachment.matInit._21 = row1.x;
				attachment.matInit._22 = row1.y;
				attachment.matInit._23 = row1.z;
				attachment.matInit._31 = row2.x;
				attachment.matInit._32 = row2.y;
				attachment.matInit._33 = row2.z;
				attachment.matInit._41 = row3.x * MessageProcess::GetInstance()->m_fZoomRatio;
				attachment.matInit._42 = row3.y * MessageProcess::GetInstance()->m_fZoomRatio;
				attachment.matInit._43 = row3.z * MessageProcess::GetInstance()->m_fZoomRatio;

				m_effattchments.vectorAttachment.add( attachment );
			}
			else
			{
				vectorBone.add( pNode );
			}

		}
		// 其他类型的节点
		else
		{
#if 0
			// 条带发射器
			if( strnicmp( "ribb.", szName, 5 ) == 0 )
			{
				const char* LastNameStr = strrchr(szName, '_');
				if( LastNameStr && strnicmp( "_bump", LastNameStr, 5 ) == 0 )
					CreateRibbonEmitter( pNode, TRUE );
				else
					CreateRibbonEmitter( pNode, FALSE );
				continue;
			}
#endif
			// 粒子发射器
			if( _strnicmp( "part.", szName, 5 ) == 0 )
			{				
				Particle part0;
				part0.pNode = pNode;
				strcpy( part0.szName, pNode->GetName() );
				Matrix3 tm = pNode->GetObjTMAfterWSM ( 0 );

				// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
				// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
				// Node Transform Matrix should be swaped.
				/*
				If your matrix looks like this:
				{ rx, ry, rz, 0 }  
				{ ux, uy, uz, 0 }  
				{ lx, ly, lz, 0 }  
				{ px, py, pz, 1 }
				To change it from left to right or right to left, flip it like this:
				{ rx, rz, ry, 0 }  
				{ lx, lz, ly, 0 }  
				{ ux, uz, uy, 0 }  
				{ px, pz, py, 1 }
				*/

				part0.matInit.Identity();
				Point3 row0 = tm.GetRow(0);
				Point3 row1 = tm.GetRow(1);
				Point3 row2 = tm.GetRow(2);
				Point3 row3 = tm.GetRow(3);
				sgp::swapVariables( row0.y,  row0.z );
				sgp::swapVariables( row1.x,  row2.x );
				sgp::swapVariables( row1.y,  row2.z );
				sgp::swapVariables( row1.z,  row2.y );
				sgp::swapVariables( row3.y,  row3.z );

				part0.matInit._11 = row0.x;
				part0.matInit._12 = row0.y;
				part0.matInit._13 = row0.z;
				part0.matInit._21 = row1.x;
				part0.matInit._22 = row1.y;
				part0.matInit._23 = row1.z;
				part0.matInit._31 = row2.x;
				part0.matInit._32 = row2.y;
				part0.matInit._33 = row2.z;
				part0.matInit._41 = row3.x * MessageProcess::GetInstance()->m_fZoomRatio;
				part0.matInit._42 = row3.y * MessageProcess::GetInstance()->m_fZoomRatio;
				part0.matInit._43 = row3.z * MessageProcess::GetInstance()->m_fZoomRatio;


				m_Particles.vectorParticle.add(part0);

				continue;
			}

			// 基本几何体
			if( !CreateGeometry( pNode ) )
				continue;
		}
	}

	ip->StopProgressInfo();

	if( vectorBone.size() == 0 )
	{
		MessageBox( NULL, TEXT("创建模型骨骼失败，当前骨骼数量为0！"), TEXT("error"), MB_OK );
		return false;
	}
	if( vectorBone.size() > 254 )
	{
		MessageBox( NULL, TEXT("创建模型骨骼失败，当前骨骼数量超过254！"), TEXT("error"), MB_OK );
		return false;
	}

	INodeSorter sorter;
	vectorBone.sort( sorter );

	// 创建骨骼
	for( int i = 0; i < vectorBone.size(); i++ )
	{
		INode* pNode = vectorBone.getReference(i);
		
		Bone bone;
		bone.pNode = pNode;
		strcpy( bone.szName, pNode->GetName() );
		INode* pParent = pNode->GetParentNode();
		
		if( pParent == pRootNode )
			strcpy( bone.szParentName, "" );
		else 
			strcpy( bone.szParentName, pNode->GetParentNode()->GetName() );

		bone.nParentId = -1;
		m_skeleton.vectorBone.add( bone );
	}

	int MAX_NODES = m_skeleton.vectorBone.size();
	INode** nodes = new INode* [MAX_NODES];
	SGP_MaxInterface::Track** tracks = new SGP_MaxInterface::Track* [MAX_NODES];

	for(int i = 0; i < m_skeleton.vectorBone.size(); i++ )
	{
		nodes[i] = m_skeleton.vectorBone.getReference(i).pNode;
		tracks[i] = &m_skeleton.vectorBone.getReference(i).track;
	}

	ip->GetTracks( vectorBone.size(), nodes, tracks );

	delete [] nodes;
	nodes = NULL;
	delete [] tracks;
	tracks = NULL;

	// 建立骨架父子关系
	for(int i = 0; i < m_skeleton.vectorBone.size(); i++ )
	{
		Bone* pBone = &m_skeleton.vectorBone.getReference(i);
		if( pBone->szParentName[0] == 0 ||
			pBone->pNode->GetParentNode() == ip->GetRootNode() ||
			pBone->pNode->GetParentNode() == NULL )
		{
			pBone->nParentId = -1;
			m_skeleton.vectorRootBoneId.add( i );
		}
		else
		{
			for( int nBoneId = 0; nBoneId < m_skeleton.vectorBone.size(); nBoneId++ )
			{
				if( nBoneId == i )
					continue;
				if( strcmp( pBone->szParentName, m_skeleton.vectorBone[nBoneId].szName ) == 0 )
				{
					pBone->nParentId = nBoneId;
					m_skeleton.vectorBone.getReference(nBoneId).vectorChildId.add( i );
				}
			}
		}

	}
	// 映射骨骼组
	ProjBoneGroups();

	// 检查骨骼组是否都已经被影射
	for( int i = 0; i < m_boneGroups.vectorBoneGroup.size(); i++ )
	{
		SGP_MaxInterface::BoneGroup* bg = &m_boneGroups.vectorBoneGroup.getReference(i);
		for( int j = 0; j < bg->vectorInfl.size(); j++ )
		{
			if( -1 == bg->vectorInfl[j].nBoneId )
			{
				MessageBox(NULL, TEXT("骨骼组存在异常的绑定点, 会导致无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}

	for( int nAtt = 0; nAtt < m_attachments.vectorAttachment.size(); nAtt++ )
	{
		Attachment* pAttachment = &m_attachments.vectorAttachment.getReference(nAtt);
		if( pAttachment->pNode->GetParentNode() )
			pAttachment->nAttachBoneID = m_skeleton.FindBone( pAttachment->pNode->GetParentNode()->GetName() );
	}
	for( int nAtt = 0; nAtt < m_effattchments.vectorAttachment.size(); nAtt++)
	{
		Attachment* pAttachment = &m_effattchments.vectorAttachment.getReference(nAtt);
		if( pAttachment->pNode->GetParentNode() )
			pAttachment->nAttachBoneID = m_skeleton.FindBone( pAttachment->pNode->GetParentNode()->GetName() );
	}
	for( int nPart = 0; nPart < m_Particles.vectorParticle.size(); nPart++)
	{
		Particle* pParticle = &m_Particles.vectorParticle.getReference(nPart);
		if( pParticle->pNode )
			pParticle->nAttachBoneID = m_skeleton.FindBone( pParticle->pNode->GetParentNode()->GetName() );
	}
	return true;
}


bool SGP_MF1Candidate::SaveMF1File( const char* pszFilename, bool bConvTexturePath )
{
	sgp::File MAXFullFile = sgp::File( sgp::String(sgp::CharPointer_UTF8(pszFilename)) );

	if( MAXFullFile.isDirectory() || MAXFullFile == sgp::File::nonexistent )
		return false;

	sgp::String MF1RelativePathName = sgp::File( MessageProcess::GetInstance()->m_MeshDir ).getRelativePathFrom(sgp::File( MessageProcess::GetInstance()->m_WorkDir ));
	sgp::String BF1RelativePathName = sgp::File( MessageProcess::GetInstance()->m_BoneDir ).getRelativePathFrom(sgp::File( MessageProcess::GetInstance()->m_WorkDir ));
	sgp::String TexRelativePathName = sgp::File( MessageProcess::GetInstance()->m_TextureDir ).getRelativePathFrom(sgp::File( MessageProcess::GetInstance()->m_WorkDir ));


	sgp::String FileName = MAXFullFile.getFileNameWithoutExtension();


	sgp::String MF1FullPathName = MF1RelativePathName + sgp::File::separatorString + FileName + sgp::String(".mf1");
	sgp::String BF1FullPathName = BF1RelativePathName + sgp::File::separatorString + FileName + sgp::String(".bf1");

	// Check Something
	if( MF1FullPathName.length() >= 64 )
	{
		MessageBox(NULL, TEXT("文件名过长，无法保存！！"), TEXT("error"), MB_OK );
		return false;
	}


	// Optimization: Remove not used by any vertex and no name bones 
	RemoveUnUsedBones();

	sgp::CSGPModelMF1 ModelMF1Inst;

	ModelMF1Inst.m_Header.m_iHeaderSize = sizeof(sgp::SGPMF1Header);

	strcpy(ModelMF1Inst.m_Header.m_cFilename, MF1FullPathName.toUTF8().getAddress());
	ModelMF1Inst.m_Header.m_iNumActionList = MessageProcess::GetInstance()->m_ActionList.size();

	// AABB
	{		
		ModelMF1Inst.m_MeshAABBox = m_AABBox;
		// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
		// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
		// Bounding box Y and Z axis should be swaped.
		sgp::swapVariables(ModelMF1Inst.m_MeshAABBox.vcMin.y, ModelMF1Inst.m_MeshAABBox.vcMin.z);
		sgp::swapVariables(ModelMF1Inst.m_MeshAABBox.vcMax.y, ModelMF1Inst.m_MeshAABBox.vcMax.z);
		sgp::swapVariables(ModelMF1Inst.m_MeshAABBox.vcCenter.y, ModelMF1Inst.m_MeshAABBox.vcCenter.z);
	
		ModelMF1Inst.m_MeshAABBox.vcMin *= MessageProcess::GetInstance()->m_fZoomRatio;
		ModelMF1Inst.m_MeshAABBox.vcMax *= MessageProcess::GetInstance()->m_fZoomRatio;
		ModelMF1Inst.m_MeshAABBox.vcCenter *= MessageProcess::GetInstance()->m_fZoomRatio;
	}

	// Skin
	{
		ModelMF1Inst.m_Header.m_iUVAnim = 0;
		ModelMF1Inst.m_Header.m_iNumSkins = m_Materials.vectorMtl.size();

		ModelMF1Inst.m_pSkins = new sgp::SGPMF1Skin [ModelMF1Inst.m_Header.m_iNumSkins];
				
		for( int i=0; i<ModelMF1Inst.m_Header.m_iNumSkins; i++ )
		{
			sgp::String TextureFileName = sgp::File(m_textures.vectorTexture[m_Materials.vectorMtl[i].iTextureID].szTextureFileFullName).getFileName();
			TextureFileName = TexRelativePathName + sgp::File::separatorString + TextureFileName;
			
			strcpy(ModelMF1Inst.m_pSkins[i].m_cName, TextureFileName.toUTF8().getAddress());
			ModelMF1Inst.m_pSkins[i].m_iMtlFlag = m_Materials.vectorMtl[i].renderFlag;
			ModelMF1Inst.m_pSkins[i].m_UTile = m_Materials.vectorMtl[i].vectorColorTrack[0].nUTile;
			ModelMF1Inst.m_pSkins[i].m_VTile = m_Materials.vectorMtl[i].vectorColorTrack[0].nVTile;
			ModelMF1Inst.m_pSkins[i].m_iStartFrame = m_Materials.vectorMtl[i].vectorColorTrack[0].nStartFrame;
			ModelMF1Inst.m_pSkins[i].m_iLoopMode = m_Materials.vectorMtl[i].vectorColorTrack[0].nLoopMode;
			ModelMF1Inst.m_pSkins[i].m_fPlayBackRate = m_Materials.vectorMtl[i].vectorColorTrack[0].fPlaybackRate;
			ModelMF1Inst.m_pSkins[i].m_fUSpeed = m_Materials.vectorMtl[i].vectorColorTrack[0].fUSpeed;
			ModelMF1Inst.m_pSkins[i].m_fVSpeed = m_Materials.vectorMtl[i].vectorColorTrack[0].fVSpeed;
			
			ModelMF1Inst.m_Header.m_iUVAnim += 
				(m_Materials.vectorMtl[i].vectorColorTrack[0].bTiling ||
				 m_Materials.vectorMtl[i].vectorColorTrack[0].bUVMoving) ? 1 : 0;

			// Compress material Color Key frame
			sgp::Array<sgp::SGPMF1MatKeyFrame> CompressedColorKey;
			CompressMaterialColorKeyFrame(CompressedColorKey, m_Materials.vectorMtl.getReference(i).vectorColorTrack.getReference(0).ColorKeyFrame);
			ModelMF1Inst.m_pSkins[i].m_iNumMatKeyFrame = CompressedColorKey.size();
			if( ModelMF1Inst.m_pSkins[i].m_iNumMatKeyFrame > 0 )
				ModelMF1Inst.m_pSkins[i].m_pMatKeyFrame = new sgp::SGPMF1MatKeyFrame [ModelMF1Inst.m_pSkins[i].m_iNumMatKeyFrame];
					
			for( int j=0; j<ModelMF1Inst.m_pSkins[i].m_iNumMatKeyFrame; j++ )
				ModelMF1Inst.m_pSkins[i].m_pMatKeyFrame[j] = CompressedColorKey[j];

			ModelMF1Inst.m_Header.m_iUVAnim += (CompressedColorKey.size() > 1) ? 1 : 0;				
		}
	}

	// LOD Mesh
	{
		if( ModelMF1Inst.m_Header.m_iNumLods >= 1 )
		{
			ModelMF1Inst.m_Header.m_iNumMeshes = m_geometry.vectorChunk.size();

			ModelMF1Inst.m_pLOD0Meshes = new sgp::SGPMF1Mesh [ModelMF1Inst.m_Header.m_iNumMeshes];
			for( int i = 0; i < m_geometry.vectorChunk.size(); i++ )
			{
				GeomChunk& Chunk = m_geometry.vectorChunk.getReference(i);
				if( Chunk.vectorVertex.size() == 0 || Chunk.vectorFace.size() == 0 )
					continue;

				strcpy(ModelMF1Inst.m_pLOD0Meshes[i].m_cMeshId, "nm");
				strcpy(ModelMF1Inst.m_pLOD0Meshes[i].m_cName, Chunk.szNodename);

				if( Chunk.vectorColor.size() > 0 )
				{
					int k = 1;
					for(k=1; k<Chunk.vectorColor.size(); k++)
					{
						if( Chunk.vectorColor[0] != Chunk.vectorColor[k] )
							break;
					}
					if( k == Chunk.vectorColor.size() )
						Chunk.vectorColor.resize(1);
				}

				ModelMF1Inst.m_pLOD0Meshes[i].m_iNumVerts = Chunk.vectorVertex.size();
				ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex = new sgp::SGPMF1Vertex [Chunk.vectorVertex.size()];
				for(int j=0; j<Chunk.vectorVertex.size(); j++)
				{
					// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
					// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vPos[0] = Chunk.vectorVertex[j].x * MessageProcess::GetInstance()->m_fZoomRatio;
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vPos[1] = Chunk.vectorVertex[j].z * MessageProcess::GetInstance()->m_fZoomRatio;
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vPos[2] = Chunk.vectorVertex[j].y * MessageProcess::GetInstance()->m_fZoomRatio;
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vNormal[0] = Chunk.vectorNormal[j].x;
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vNormal[1] = Chunk.vectorNormal[j].z;
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertex[j].vNormal[2] = Chunk.vectorNormal[j].y;
				}

				ModelMF1Inst.m_pLOD0Meshes[i].m_iNumIndices = Chunk.vectorFace.size()*3;
				ModelMF1Inst.m_pLOD0Meshes[i].m_pIndices = new sgp::uint16 [Chunk.vectorFace.size()*3];
				for(int j=0; j<Chunk.vectorFace.size(); j++)
				{
					ModelMF1Inst.m_pLOD0Meshes[i].m_pIndices[j*3] = Chunk.vectorFace[j].nMaxVertexId[0];
					ModelMF1Inst.m_pLOD0Meshes[i].m_pIndices[j*3+1] = Chunk.vectorFace[j].nMaxVertexId[1];
					ModelMF1Inst.m_pLOD0Meshes[i].m_pIndices[j*3+2] = Chunk.vectorFace[j].nMaxVertexId[2];
				}


				ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexBoneGroupID = new sgp::uint16 [Chunk.vectorVertex.size()];
				for(int j=0; j<Chunk.vectorVertex.size(); j++)
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexBoneGroupID[j] = Chunk.vectorBGId[j];

				if( Chunk.vectorUV.size() > 0 )
				{
					ModelMF1Inst.m_pLOD0Meshes[i].m_iNumUV0 = Chunk.vectorUV.size();
					ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords0 = new sgp::SGPMF1TexCoord [Chunk.vectorUV.size()];
					for(int j=0; j<Chunk.vectorUV.size(); j++)
					{
						ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords0[j].m_fTexCoord[0] = Chunk.vectorUV[j].x;
						ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords0[j].m_fTexCoord[1] = Chunk.vectorUV[j].y;
					}
				}

				if( !MessageProcess::GetInstance()->m_bExportUV2 )
					Chunk.vectorUV1.clear();
				if( Chunk.vectorUV1.size() > 0 )
				{
					ModelMF1Inst.m_pLOD0Meshes[i].m_iNumUV1 = Chunk.vectorUV1.size();
					ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords1 = new sgp::SGPMF1TexCoord [Chunk.vectorUV1.size()];
					for(int j=0; j<Chunk.vectorUV1.size(); j++)
					{
						ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords1[j].m_fTexCoord[0] = Chunk.vectorUV1[j].x;
						ModelMF1Inst.m_pLOD0Meshes[i].m_pTexCoords1[j].m_fTexCoord[1] = Chunk.vectorUV1[j].y;
					}
				}

				if( !MessageProcess::GetInstance()->m_bExportVertexColor )
					Chunk.vectorColor.clear();
				if( Chunk.vectorColor.size() > 0 )
				{
					ModelMF1Inst.m_pLOD0Meshes[i].m_iNumVertexColor = Chunk.vectorColor.size();
					ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexColor = new sgp::SGPMF1VertexColor [Chunk.vectorColor.size()];
					for(int j=0; j<Chunk.vectorColor.size(); j++)
					{
						// Vertex color in Chunk.vectorColor Array range 0-255
						ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexColor[j].m_fVertColor[0] = Chunk.vectorColor[j].x / 255.0f;
						ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexColor[j].m_fVertColor[1] = Chunk.vectorColor[j].y / 255.0f;
						ModelMF1Inst.m_pLOD0Meshes[i].m_pVertexColor[j].m_fVertColor[2] = Chunk.vectorColor[j].z / 255.0f;	
					}
				}
				ModelMF1Inst.m_pLOD0Meshes[i].m_SkinIndex = Chunk.nMtlId;
				ModelMF1Inst.m_pLOD0Meshes[i].m_nType = Chunk.nType;
				ModelMF1Inst.m_pLOD0Meshes[i].m_bbox = Chunk.aabbox;
				// The coordinate system of 3DMax9 is Right-X Up-Z Screenin-Y
				// But coordinate system of SGP Engine is like D3D Right-X Up-Y Screenin-Z
				// Bounding box Y and Z axis should be swaped.
				sgp::swapVariables(ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMin.y, ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMin.z);
				sgp::swapVariables(ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMax.y, ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMax.z);
				sgp::swapVariables(ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcCenter.y, ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcCenter.z);
			
				ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMin *= MessageProcess::GetInstance()->m_fZoomRatio;
				ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcMax *= MessageProcess::GetInstance()->m_fZoomRatio;
				ModelMF1Inst.m_pLOD0Meshes[i].m_bbox.vcCenter *= MessageProcess::GetInstance()->m_fZoomRatio;
			}
		}
		if( ModelMF1Inst.m_Header.m_iNumLods >= 2 )
		{
			ModelMF1Inst.m_Header.m_iLod1MeshOffset = 0;
			// TODO
		}
		if( ModelMF1Inst.m_Header.m_iNumLods >= 3 )
		{
			ModelMF1Inst.m_Header.m_iLod2MeshOffset = 0;
			// TODO
		}
		if( ModelMF1Inst.m_Header.m_iNumLods >= 4 )
			assert(0);
	}


	bool bSkeletonAnim = false;
	{
		// 上下半身的分离标志骨骼（划分到下半身）
		ModelMF1Inst.m_Header.m_iBipBoneID = m_skeleton.FindBone( "Bip01" );

		// 上半身骨骼的分界线（包括它自己以及所有它的子骨骼都属于上半身骨骼）
		int nUpperBodyRootBoneID = m_skeleton.FindBone( "Bip01 Spine1" );
		
		// Have Bone Data
		if( m_skeleton.vectorBone.size() > 0 )
			ModelMF1Inst.m_pBones = new sgp::SGPMF1Bone [m_skeleton.vectorBone.size()];
		for( int i = 0; i < m_skeleton.vectorBone.size(); i++ )
		{
			Bone& BoneSrc = m_skeleton.vectorBone.getReference(i);
			strcpy(ModelMF1Inst.m_pBones[i].m_cName, BoneSrc.szName);
			ModelMF1Inst.m_pBones[i].m_sParentID = BoneSrc.nParentId;
			ModelMF1Inst.m_pBones[i].m_iNumChildId = BoneSrc.vectorChildId.size();

			if( ModelMF1Inst.m_pBones[i].m_iNumChildId > 0 )
			{
				ModelMF1Inst.m_pBones[i].m_ChildIds = new sgp::uint16 [BoneSrc.vectorChildId.size()];
				for( int j=0; j<BoneSrc.vectorChildId.size(); j++ )
					ModelMF1Inst.m_pBones[i].m_ChildIds[j] = (sgp::uint16)BoneSrc.vectorChildId[j];
			}

			// according to nUpperBodyRootBoneID, Determine the current bones belonging to the upper and lower body
			if( (i == nUpperBodyRootBoneID) || m_skeleton.IsParentBone(nUpperBodyRootBoneID, i) )
				ModelMF1Inst.m_pBones[i].m_bUpperBone = 1;
			else
				ModelMF1Inst.m_pBones[i].m_bUpperBone = 0;

			ModelMF1Inst.m_pBones[i].m_matFrame0Inv.Identity();
			ModelMF1Inst.m_pBones[i].m_matFrame0Inv._11 = ModelMF1Inst.m_pBones[i].m_matFrame0Inv._22 = ModelMF1Inst.m_pBones[i].m_matFrame0Inv._33 = 
				BoneSrc.track.vectorScale[0].x;

			sgp::Matrix4x4 matTemp;
			sgp::Quaternion(
				BoneSrc.track.vectorRot[0].x,
				BoneSrc.track.vectorRot[0].y,
				BoneSrc.track.vectorRot[0].z,
				BoneSrc.track.vectorRot[0].w ).GetMatrix( &matTemp );		

			ModelMF1Inst.m_pBones[i].m_matFrame0Inv = ModelMF1Inst.m_pBones[i].m_matFrame0Inv * matTemp;

			ModelMF1Inst.m_pBones[i].m_matFrame0Inv.Translate(
				BoneSrc.track.vectorTrans[0].x * MessageProcess::GetInstance()->m_fZoomRatio,
				BoneSrc.track.vectorTrans[0].y * MessageProcess::GetInstance()->m_fZoomRatio,
				BoneSrc.track.vectorTrans[0].z * MessageProcess::GetInstance()->m_fZoomRatio );
					
			matTemp = ModelMF1Inst.m_pBones[i].m_matFrame0Inv;
			ModelMF1Inst.m_pBones[i].m_matFrame0Inv.InverseOf(matTemp);

	
			// Compress Bone Track Key frame
			sgp::Array<sgp::SGPMF1KeyFrame> CompressedTrackTrans;
			sgp::Array<sgp::SGPMF1KeyFrame> CompressedTrackRots;
			sgp::Array<sgp::SGPMF1ScaleKeyFrame> CompressedTrackScales;
			sgp::Array<sgp::SGPMF1VisibleKeyFrame> CompressedTrackVisiible;

			
			CompressTrackTransKeyFrame(CompressedTrackTrans, BoneSrc.track.vectorTrans);
			CompressTrackRotsKeyFrame(CompressedTrackRots, BoneSrc.track.vectorRot);
			CompressTrackScalesKeyFrame(CompressedTrackScales, BoneSrc.track.vectorScale);
			CompressTrackVisibleKeyFrame(CompressedTrackVisiible, BoneSrc.track.vectorVisible);

			// If this bone have no anim data and no action List, It is static chunk
			if(	!HaveBoneAnimData(BoneSrc) && (ModelMF1Inst.m_Header.m_iNumActionList == 0) )
			{
				CompressedTrackTrans.resize(1);
				CompressedTrackRots.resize(1);
				CompressedTrackScales.resize(1);
				CompressedTrackVisiible.resize(1);
			}
			else
				bSkeletonAnim = true;

			if( MessageProcess::GetInstance()->m_bStaticMesh )
				bSkeletonAnim = false;

			ModelMF1Inst.m_pBones[i].m_TransKeyFrames = new sgp::KeyFrameBlock();
			ModelMF1Inst.m_pBones[i].m_TransKeyFrames->m_iNumber = CompressedTrackTrans.size();
			ModelMF1Inst.m_pBones[i].m_RotsKeyFrames = new sgp::KeyFrameBlock();
			ModelMF1Inst.m_pBones[i].m_RotsKeyFrames->m_iNumber = CompressedTrackRots.size();
			ModelMF1Inst.m_pBones[i].m_ScaleKeyFrames = new sgp::ScaleKeyFrameBlock();
			ModelMF1Inst.m_pBones[i].m_ScaleKeyFrames->m_iNumber = CompressedTrackScales.size();
			ModelMF1Inst.m_pBones[i].m_VisibleKeyFrames = new sgp::VisibleKeyFrameBlock();
			ModelMF1Inst.m_pBones[i].m_VisibleKeyFrames->m_iNumber = CompressedTrackVisiible.size();


			ModelMF1Inst.m_pBones[i].m_TransKeyFrames->m_KeyFrames = new sgp::SGPMF1KeyFrame [CompressedTrackTrans.size()];
			for( int j=0; j<CompressedTrackTrans.size(); j++ )
				ModelMF1Inst.m_pBones[i].m_TransKeyFrames->m_KeyFrames[j] = CompressedTrackTrans[j];
			ModelMF1Inst.m_pBones[i].m_RotsKeyFrames->m_KeyFrames = new sgp::SGPMF1KeyFrame [CompressedTrackRots.size()];
			for( int j=0; j<CompressedTrackRots.size(); j++ )
				ModelMF1Inst.m_pBones[i].m_RotsKeyFrames->m_KeyFrames[j] = CompressedTrackRots[j];
			ModelMF1Inst.m_pBones[i].m_ScaleKeyFrames->m_KeyFrames = new sgp::SGPMF1ScaleKeyFrame [CompressedTrackScales.size()];
			for( int j=0; j<CompressedTrackScales.size(); j++ )
				ModelMF1Inst.m_pBones[i].m_ScaleKeyFrames->m_KeyFrames[j] = CompressedTrackScales[j];
			ModelMF1Inst.m_pBones[i].m_VisibleKeyFrames->m_KeyFrames = new sgp::SGPMF1VisibleKeyFrame [CompressedTrackVisiible.size()];
			for( int j=0; j<CompressedTrackVisiible.size(); j++ )
				ModelMF1Inst.m_pBones[i].m_VisibleKeyFrames->m_KeyFrames[j] = CompressedTrackVisiible[j];

		}

		// Bone Anim file name
		if( bSkeletonAnim )
		{
			// First get the number of BoneAnimFile	
			sgp::uint32 iNumBoneAnimFile = 0;
			for( int k=0; k<ModelMF1Inst.m_Header.m_iNumActionList; k++ )
				if( MessageProcess::GetInstance()->m_ActionList[k].fileNo > iNumBoneAnimFile )
					iNumBoneAnimFile = MessageProcess::GetInstance()->m_ActionList[k].fileNo;

			ModelMF1Inst.m_Header.m_iNumBoneAnimFile = iNumBoneAnimFile + 1;
			ModelMF1Inst.m_pBoneFileNames = new sgp::SGPMF1BoneFileName [ModelMF1Inst.m_Header.m_iNumBoneAnimFile];
			for( int k=0; k<ModelMF1Inst.m_Header.m_iNumBoneAnimFile; k++ )
			{
				if( k==0 )
					strcpy(ModelMF1Inst.m_pBoneFileNames[k].m_cBoneFileName, BF1FullPathName.toUTF8().getAddress());
				else
					strcpy(ModelMF1Inst.m_pBoneFileNames[k].m_cBoneFileName, sgp::String(BF1FullPathName + sgp::String(k)).toUTF8().getAddress());
			}
		}
		else
			ModelMF1Inst.m_Header.m_iBoneAnimFileOffset = 0;
	}


	// ActionList
	{
		if( bSkeletonAnim )
		{
			// If Node has Anim data but not setting ActionLists, Create default ActionList
			if( ModelMF1Inst.m_Header.m_iNumActionList == 0 )
			{
				ModelMF1Inst.m_Header.m_iNumActionList = 1;
				ModelMF1Inst.m_pActionLists = new sgp::SGPMF1ActionList [ModelMF1Inst.m_Header.m_iNumActionList];
				strcpy(ModelMF1Inst.m_pActionLists[0].m_cName, "idle");
				ModelMF1Inst.m_pActionLists[0].m_iBoneFileIndex = 0;
				ModelMF1Inst.m_pActionLists[0].m_iActionStart = g_SGPMax9CtrlPanel.GetMaxIP()->GetStartFrame();
				ModelMF1Inst.m_pActionLists[0].m_iActionEnd = g_SGPMax9CtrlPanel.GetMaxIP()->GetEndFrame();
			}
			else
			{
				ModelMF1Inst.m_pActionLists = new sgp::SGPMF1ActionList [ModelMF1Inst.m_Header.m_iNumActionList];
				for( int k=0; k<ModelMF1Inst.m_Header.m_iNumActionList; k++ )
				{
					strcpy(ModelMF1Inst.m_pActionLists[k].m_cName, MessageProcess::GetInstance()->m_ActionList[k].actionName);
					ModelMF1Inst.m_pActionLists[k].m_iBoneFileIndex = MessageProcess::GetInstance()->m_ActionList[k].fileNo;
					ModelMF1Inst.m_pActionLists[k].m_iActionStart = MessageProcess::GetInstance()->m_ActionList[k].startFrameID;
					ModelMF1Inst.m_pActionLists[k].m_iActionEnd = MessageProcess::GetInstance()->m_ActionList[k].endFrameID;
				}
			}
		}
		else
		{
			ModelMF1Inst.m_Header.m_iNumActionList = 0;
			ModelMF1Inst.m_Header.m_iActionListOffset = 0;
		}
	}



	//ATTACHMENT
	{
		ModelMF1Inst.m_Header.m_iNumAttc = m_attachments.vectorAttachment.size();
		if( ModelMF1Inst.m_Header.m_iNumAttc > 0 )
		{
			ModelMF1Inst.m_pAttachTags = new sgp::SGPMF1AttachmentTag [ModelMF1Inst.m_Header.m_iNumAttc];
			for( int nAtt = 0; nAtt < m_attachments.vectorAttachment.size(); nAtt++ )
			{
				ModelMF1Inst.m_pAttachTags[nAtt].m_iAttachBoneID = (sgp::uint16)m_attachments.vectorAttachment[nAtt].nAttachBoneID;
				for( sgp::uint16 nPart = 0; nPart < sgp::SGPATTDEF_MAXATTACHMENT; nPart++ )
					if( _strnicmp( sgp::g_AttachmentName[nPart], m_attachments.vectorAttachment[nAtt].szName, strlen(sgp::g_AttachmentName[nPart]) ) == 0 )
						ModelMF1Inst.m_pAttachTags[nAtt].m_iAttachParts = nPart;
				ModelMF1Inst.m_pAttachTags[nAtt].m_InitMatTag = m_attachments.vectorAttachment[nAtt].matInit;
			}
		}
		else
			ModelMF1Inst.m_Header.m_iAttachOffset = 0;

		ModelMF1Inst.m_Header.m_iNumEttc = m_effattchments.vectorAttachment.size();
		if( ModelMF1Inst.m_Header.m_iNumEttc > 0 )
		{
			ModelMF1Inst.m_pEffectTags = new sgp::SGPMF1AttachmentTag [ModelMF1Inst.m_Header.m_iNumEttc];
			for( int nAtt = 0; nAtt < m_effattchments.vectorAttachment.size(); nAtt++ )
			{
				ModelMF1Inst.m_pEffectTags[nAtt].m_iAttachBoneID = (sgp::uint16)m_effattchments.vectorAttachment[nAtt].nAttachBoneID;
				for( sgp::uint16 nPart = 0; nPart < sgp::SGPATTDEF_MAXATTACHMENT; nPart++ )
					if( _strnicmp( sgp::g_AttachmentName[nPart], m_effattchments.vectorAttachment[nAtt].szName, strlen(sgp::g_AttachmentName[nPart]) ) == 0 )
						ModelMF1Inst.m_pEffectTags[nAtt].m_iAttachParts = nPart;
				ModelMF1Inst.m_pEffectTags[nAtt].m_InitMatTag = m_effattchments.vectorAttachment[nAtt].matInit;
			}
		}
		else
			ModelMF1Inst.m_Header.m_iEttachOffset = 0;
	}

	// Particle
	{
		ModelMF1Inst.m_Header.m_iNumParticles = m_Particles.vectorParticle.size();
		if( ModelMF1Inst.m_Header.m_iNumParticles > 0 )
		{
			ModelMF1Inst.m_pParticleEmitter = new sgp::SGPMF1ParticleTag [ModelMF1Inst.m_Header.m_iNumParticles];
			for( int nPart = 0; nPart < m_Particles.vectorParticle.size(); nPart++ )
			{
				ModelMF1Inst.m_pParticleEmitter[nPart].m_iAttachBoneID = (sgp::uint16)m_Particles.vectorParticle[nPart].nAttachBoneID;
				strcpy(ModelMF1Inst.m_pParticleEmitter[nPart].m_szname, m_Particles.vectorParticle[nPart].szName);
				ModelMF1Inst.m_pParticleEmitter[nPart].m_AbsoluteMatrix = m_Particles.vectorParticle[nPart].matInit;
			}
		}
		else
			ModelMF1Inst.m_Header.m_iParticleOffset = 0;
	}

	// RIBBON
	{
		ModelMF1Inst.m_Header.m_iRibbonOffset = 0;
	}

	// CONFIG SETTING
	{
		ModelMF1Inst.m_Header.m_iConfigsOffset = 0;
	}



	// Save MF1 Files
	ModelMF1Inst.SaveMF1( sgp::String(MessageProcess::GetInstance()->m_WorkDir), MF1FullPathName );






	if( bSkeletonAnim )
	{
		ModelMF1Inst.m_iNumBones = m_skeleton.vectorBone.size();
		ModelMF1Inst.m_iNumBoneGroup = m_boneGroups.vectorBoneGroup.size();

		ModelMF1Inst.m_pBoneGroup = new sgp::SGPMF1BoneGroup [m_boneGroups.vectorBoneGroup.size()];
		for(int i=0; i<m_boneGroups.vectorBoneGroup.size(); i++)
		{
			switch( m_boneGroups.vectorBoneGroup[i].vectorInfl.size() )
			{
			case 1:
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.y = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.z = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.w = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.y = ModelMF1Inst.m_pBoneGroup[i].BoneWeight.z = ModelMF1Inst.m_pBoneGroup[i].BoneWeight.w = 0;
				break;
			case 2:
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.z = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.w = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.z = ModelMF1Inst.m_pBoneGroup[i].BoneWeight.w = 0;
				break;
			case 3:
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.z = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[2].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.w = 0;
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.z = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[2].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.w = 0;
				break;
			case 4:
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.z = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[2].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneIndex.w = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[3].nBoneId);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.x = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[0].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.y = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[1].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.z = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[2].fWeight * 255);
				ModelMF1Inst.m_pBoneGroup[i].BoneWeight.w = (sgp::uint8)(m_boneGroups.vectorBoneGroup[i].vectorInfl[3].fWeight * 255);
				break;
			default:
				assert(0);
				break;
			}
		}

		// Save BF1 File
		for( sgp::uint16 BoneFileIndex = 0; BoneFileIndex < (sgp::uint16)ModelMF1Inst.m_Header.m_iNumBoneAnimFile; BoneFileIndex++ )
			ModelMF1Inst.SaveBone( sgp::String(MessageProcess::GetInstance()->m_WorkDir), BF1FullPathName, BoneFileIndex );
	}

	// Copy working Skin Texture Files
	for( int i=0; i<m_Materials.vectorMtl.size(); i++ )
	{
		sgp::String TexturePath = sgp::String( MessageProcess::GetInstance()->m_TextureDir );

		sgp::File TextureFile( m_textures.vectorTexture[m_Materials.vectorMtl[i].iTextureID].szTextureFileFullName );
		sgp::String TextureFileName = TextureFile.getFileName();

		if( !TextureFile.existsAsFile() )
		{
			SGP_MaxInterface* ip = g_SGPMax9CtrlPanel.GetMaxIP();
			sgp::File TextureFileTry2( ip->GetCurFilePath().data() );
			sgp::String TextureFileNameTry2 = TextureFileTry2.getParentDirectory().getFullPathName() + sgp::File::separatorString + TextureFileName;
			sgp::File(TextureFileNameTry2).copyFileTo(sgp::File(TexturePath + sgp::File::separatorString + TextureFileName));
			
		}
		else
			TextureFile.copyFileTo(sgp::File(TexturePath + sgp::File::separatorString + TextureFileName));
	}

	return true;
}

bool SGP_MF1Candidate::CheckMF1FileFormat()
{
	sgp::String TexRelativePathName = sgp::File( MessageProcess::GetInstance()->m_TextureDir ).getRelativePathFrom(sgp::File( MessageProcess::GetInstance()->m_WorkDir ));

	// Check Texture Name
	for( int i=0; i<m_textures.vectorTexture.size(); i++ )
	{
		sgp::String TextureFileName = sgp::File(m_textures.vectorTexture[i].szTextureFileFullName).getFileName();
		TextureFileName = TexRelativePathName + sgp::File::separatorString + TextureFileName;

		if( TextureFileName.length() >= 64 )
		{
			MessageBox(NULL, TEXT("Texture文件名过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
	}
	// Check material Info 
	for( int i=0; i<m_Materials.vectorMtl.size(); i++ )
	{
		if( m_Materials.vectorMtl[i].vectorColorTrack.size() != 1 )
		{
			MessageBox(NULL, TEXT("一个mesh中的Material过多，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
	}

	// Check geometry chunk name
	for( int i = 0; i < m_geometry.vectorChunk.size(); i++ )
	{
		if( strlen( m_geometry.vectorChunk[i].szNodename ) >= 64 )
		{
			MessageBox(NULL, TEXT("结点Chunk名子过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
		for( int j=i+1; j<m_geometry.vectorChunk.size(); j++ )
		{
			if( strcmp( m_geometry.vectorChunk[i].szNodename,
						m_geometry.vectorChunk[j].szNodename ) == 0 )
			{
				MessageBox(NULL, TEXT("发现重名结点Chunk，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}

	// Check bone Info
	const float fValueDelta = 0.001f;
	for( int i = 0; i < m_skeleton.vectorBone.size(); i++ )
	{
		// Check bone name
		if( strlen( m_skeleton.vectorBone[i].szName ) >= 32 )
		{
			MessageBox(NULL, TEXT("骨骼Bone名子过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
		for( int j=i+1; j<m_skeleton.vectorBone.size(); j++ )
		{
			if( strcmp( m_skeleton.vectorBone[i].szName,
						m_skeleton.vectorBone[j].szName ) == 0 )
			{
				MessageBox(NULL, TEXT("发现重名骨骼Bone，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}

		// Check bone uniform scale
		Bone& BoneSrc = m_skeleton.vectorBone.getReference(i);
		for( int j=0; j<BoneSrc.track.vectorScale.size(); j++ )
		{
			if( sgp::FloatCmp( BoneSrc.track.vectorScale[j].x, BoneSrc.track.vectorScale[j].y, fValueDelta ) != 0 ||
				sgp::FloatCmp( BoneSrc.track.vectorScale[j].y, BoneSrc.track.vectorScale[j].z, fValueDelta ) != 0 )
			{
				MessageBox(NULL, TEXT("bone must uniform scale，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}
	// Check attachments name
	for( int i = 0; i < m_attachments.vectorAttachment.size(); i++ )
	{
		if( strlen( m_attachments.vectorAttachment[i].szName ) >= 32 )
		{
			MessageBox(NULL, TEXT("Attachment名子过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}

		TCHAR WarnString[512];
		Attachment* pAttachment = &m_attachments.vectorAttachment.getReference(i);
		bool bAttachmentNameValid = false;
		for( sgp::uint16 nPart = 0; nPart < sgp::SGPATTDEF_MAXATTACHMENT; nPart++ )
		{
			if( _strnicmp( sgp::g_AttachmentName[nPart], pAttachment->szName, strlen(sgp::g_AttachmentName[nPart]) ) == 0 )
			{
				bAttachmentNameValid = true;
				break;
			}
		}
		if( !bAttachmentNameValid )
		{
			_stprintf(WarnString, "检测到无效的Attachment绑定点:%s, 无法保存！！", pAttachment->szName );
			MessageBox(NULL, WarnString, TEXT("error"), MB_OK );
			return false;
		}

		for( int j=i+1; j<m_attachments.vectorAttachment.size(); j++ )
		{
			if( strcmp( m_attachments.vectorAttachment[i].szName,
						m_attachments.vectorAttachment[j].szName ) == 0 )
			{
				MessageBox(NULL, TEXT("发现重名Attachment，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}
	// Check effect attachments name
	for( int i = 0; i < m_effattchments.vectorAttachment.size(); i++ )
	{
		if( strlen( m_effattchments.vectorAttachment[i].szName ) >= 32 )
		{
			MessageBox(NULL, TEXT("Effect Attachment名子过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
		for( int j=i+1; j<m_effattchments.vectorAttachment.size(); j++ )
		{
			if( strcmp( m_effattchments.vectorAttachment[i].szName,
						m_effattchments.vectorAttachment[j].szName ) == 0 )
			{
				MessageBox(NULL, TEXT("发现重名Effect Attachment，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}

	// Check Particle name
	for( int i = 0; i < m_Particles.vectorParticle.size(); i++ )
	{
		if( strlen( m_Particles.vectorParticle[i].szName ) >= 32 )
		{
			MessageBox(NULL, TEXT("Particle名子过长，无法保存！！"), TEXT("error"), MB_OK );
			return false;
		}
		for( int j=i+1; j<m_Particles.vectorParticle.size(); j++ )
		{
			if( strcmp( m_Particles.vectorParticle[i].szName,
						m_Particles.vectorParticle[j].szName ) == 0 )
			{
				MessageBox(NULL, TEXT("发现重名Particle，无法保存！！"), TEXT("error"), MB_OK );
				return false;
			}
		}
	}

	return true;
}

// Remove no data bone, not used by any vertex and no name bones
void SGP_MF1Candidate::RemoveUnUsedBones()
{
	TCHAR WarnString[512];
	for( int i = 0; i < m_skeleton.vectorBone.size(); i++ )
	{
		Bone& BoneSrc = m_skeleton.vectorBone.getReference(i);
		if( strlen(BoneSrc.szName) == 0 )
		{
			// bone no name
			MessageBox(NULL, TEXT("发现没有命名的骨骼，请检查后重新导出！！"), TEXT("error"), MB_OK );

			//m_skeleton.vectorBone.remove(i);
			//i--;
			continue;
		}

		// Is this bone used by any Vertex?
		bool bused = false;
		for(int j=0; j<m_boneGroups.vectorBoneGroup.size(); j++)
		{				
			for(int k=0; k<m_boneGroups.vectorBoneGroup[j].vectorInfl.size(); k++)
			{
				if(strcmp(BoneSrc.szName, m_boneGroups.vectorBoneGroup[j].vectorInfl[k].szBoneName) == 0)
					bused = true;
			}
		}

		// Is this bone used by any Particle?
		for(int m=0; m<m_Particles.vectorParticle.size(); m++)
			if( m_Particles.vectorParticle[m].nAttachBoneID == i )
				bused = true;

		if( !bused )
		{
			_stprintf(WarnString, "发现没有被任何顶点引用的骨骼:%s，请检查后重新导出！！", BoneSrc.szName);
			MessageBox(NULL, WarnString, TEXT("error"), MB_OK );

			//m_skeleton.vectorBone.remove(i);
			//i--;
			continue;
		}
	}
}

bool SGP_MF1Candidate::HaveBoneAnimData(const Bone& BoneSrc)
{
	for(int j=0; j<BoneSrc.track.vectorTrans.size(); j++)
	{
		for(int k=j+1; k<BoneSrc.track.vectorTrans.size(); k++)
		{
			if( !BoneSrc.track.vectorTrans[j].Equals( BoneSrc.track.vectorTrans[k] ) )
				return true;
		}
	}
	for(int j=0; j<BoneSrc.track.vectorRot.size(); j++)
	{
		for(int k=j+1; k<BoneSrc.track.vectorRot.size(); k++)
		{
			if( !BoneSrc.track.vectorRot[j].Equals( BoneSrc.track.vectorRot[k] ) )
				return true;
		}
	}
	for(int j=0; j<BoneSrc.track.vectorScale.size(); j++)
	{
		for(int k=j+1; k<BoneSrc.track.vectorScale.size(); k++)
		{
			if( !BoneSrc.track.vectorScale[j].Equals( BoneSrc.track.vectorScale[k] ) )
				return true;
		}
	}
	for(int j=0; j<BoneSrc.track.vectorVisible.size(); j++)
	{
		for(int k=j+1; k<BoneSrc.track.vectorVisible.size(); k++)
		{
			if( BoneSrc.track.vectorVisible[j] != BoneSrc.track.vectorVisible[k] )
				return true;
		}
	}

	return false;
}

void SGP_MF1Candidate::CompressMaterialColorKeyFrame(sgp::Array<sgp::SGPMF1MatKeyFrame>& CKDst, const sgp::Array<SGP_ColorKey>& CKSrc)
{
	const double KeyFrameValueDelta = 0.005;

	/*
	遍历时间轴上的连续的3个关键帧，如果按每个关键帧对于时间的比例，
	计算第2号关键帧是否正好是1，3关键帧的差值数据，
	如果是的话就删除此关键帧，然后从头再遍历，再执行上述过程，直到最后找不到可删除的关键帧，
	或者关键帧数小于3，就停止。
	*/

	for(int i=0; i<CKSrc.size(); i++)
	{
		sgp::SGPMF1MatKeyFrame keyframe;
		keyframe.m_iFrameID = i;
		keyframe.m_MaterialColor[0] = CKSrc[i].dr;
		keyframe.m_MaterialColor[1] = CKSrc[i].dg;
		keyframe.m_MaterialColor[2] = CKSrc[i].db;
		keyframe.m_MaterialColor[3] = CKSrc[i].da;
		keyframe.m_fUOffset = CKSrc[i].uoffset;
		keyframe.m_fVOffset = CKSrc[i].voffset;

		CKDst.add(keyframe);
	}

	// 只有一个关键帧，直接返回；
	if( CKDst.size() < 2 )
		return;


	int index = 0;
	while( index < CKDst.size()-2 && CKDst.size() > 2 )
	{
		sgp::SGPMF1MatKeyFrame& first = CKDst.getReference(index);
		sgp::SGPMF1MatKeyFrame& second = CKDst.getReference(index+1);
		sgp::SGPMF1MatKeyFrame& third = CKDst.getReference(index+2);

		if( !(first == second) && (second == third) )
		{
			index++;
			continue;
		}

		bool bSame = sgp::DoubleCmp(
			(double)second.m_MaterialColor[0],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_MaterialColor[0] + double(second.m_iFrameID - first.m_iFrameID) * third.m_MaterialColor[0]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0;
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_MaterialColor[1],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_MaterialColor[1] + double(second.m_iFrameID - first.m_iFrameID) * third.m_MaterialColor[1]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_MaterialColor[2],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_MaterialColor[2] + double(second.m_iFrameID - first.m_iFrameID) * third.m_MaterialColor[2]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_MaterialColor[3],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_MaterialColor[3] + double(second.m_iFrameID - first.m_iFrameID) * third.m_MaterialColor[3]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );

		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_fUOffset,
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_fUOffset + double(second.m_iFrameID - first.m_iFrameID) * third.m_fUOffset) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_fVOffset,
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_fVOffset + double(second.m_iFrameID - first.m_iFrameID) * third.m_fVOffset) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );

		if( bSame )
		{
			// 删除中间的关键帧,从头再遍历
			CKDst.remove(index+1);
			index = 0;
			continue;
		}

		index++;
	}

	// 关键帧数小于3时，若两个关键帧相同，则保留一个关键帧
	if( CKDst.size() < 3 )
	{
		if( CKDst[0] == CKDst[1] )
			CKDst.resize(1);
	}

	return;
}

void SGP_MF1Candidate::CompressTrackTransKeyFrame(sgp::Array<sgp::SGPMF1KeyFrame>& CompressedTrack, const sgp::Array<Point3>& vectorTrans)
{
	const double KeyFrameValueDelta = 0.005;

	int iStartFrame = vectorTrans.size() -1;
	int iEndFrame = 0;

	//首先根据ActionList获得有效的帧数范围
	for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
	{
		if( iStartFrame > MessageProcess::GetInstance()->m_ActionList[i].startFrameID )
			iStartFrame = MessageProcess::GetInstance()->m_ActionList[i].startFrameID;
		if( iEndFrame < MessageProcess::GetInstance()->m_ActionList[i].endFrameID )
			iEndFrame = MessageProcess::GetInstance()->m_ActionList[i].endFrameID;
	}

	/*
	遍历时间轴上的连续的3个关键帧，如果按每个关键帧对于时间的比例，
	计算第2号关键帧是否正好是1，3关键帧的差值数据，
	如果是的话就删除此关键帧，然后从头再遍历，再执行上述过程，直到最后找不到可删除的关键帧，
	或者关键帧数小于3，就停止。
	*/
	for(int i=0; i<vectorTrans.size(); i++)
	{
		sgp::SGPMF1KeyFrame keyframe;
		keyframe.m_iFrameID = i;
		keyframe.m_fParam[0] = vectorTrans[i].x * MessageProcess::GetInstance()->m_fZoomRatio;
		keyframe.m_fParam[1] = vectorTrans[i].y * MessageProcess::GetInstance()->m_fZoomRatio;
		keyframe.m_fParam[2] = vectorTrans[i].z * MessageProcess::GetInstance()->m_fZoomRatio;
		keyframe.m_fParam[3] = 0;
		CompressedTrack.add(keyframe);
	}

	int index = 0;
	if( !MessageProcess::GetInstance()->m_bUseAnimationCompress )
		index = CompressedTrack.size();

	while( index < CompressedTrack.size()-2 && CompressedTrack.size() > 2 )
	{
		sgp::SGPMF1KeyFrame& first = CompressedTrack.getReference(index);
		sgp::SGPMF1KeyFrame& second = CompressedTrack.getReference(index+1);
		sgp::SGPMF1KeyFrame& third = CompressedTrack.getReference(index+2);

		bool bSame = sgp::DoubleCmp(
			(double)second.m_fParam[0],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_fParam[0] + double(second.m_iFrameID - first.m_iFrameID) * third.m_fParam[0]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0;
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_fParam[1],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_fParam[1] + double(second.m_iFrameID - first.m_iFrameID) * third.m_fParam[1]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );
		bSame = bSame && (sgp::DoubleCmp(
			(double)second.m_fParam[2],
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_fParam[2] + double(second.m_iFrameID - first.m_iFrameID) * third.m_fParam[2]) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0 );

		// Action List start frame and end frame must be keyframe
		for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
		{
			if( (second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].startFrameID) ||
				(second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].endFrameID) )
			{
				bSame = false;
				break;
			}
		}

		if( bSame )
		{
			// 删除中间的关键帧,从头再遍历
			CompressedTrack.remove(index+1);
			index = 0;
			continue;
		}

		index++;
	}

	//根据ActionList的有效的帧数范围，裁剪多余的关键帧
	if( MessageProcess::GetInstance()->m_ActionList.size() > 0 )
	{
		for( int i=0; i<CompressedTrack.size(); i++ )
		{
			if( (CompressedTrack[i].m_iFrameID < iStartFrame) ||
				(CompressedTrack[i].m_iFrameID > iEndFrame) )
			{
				CompressedTrack.remove(i);
				i = 0;
			}
		}
	}
	return;
}

void SGP_MF1Candidate::CompressTrackRotsKeyFrame(sgp::Array<sgp::SGPMF1KeyFrame>& CompressedTrack, const sgp::Array<Quat>& vectorRot)
{
	const float KeyFrameValueDelta = 0.005f;

	int iStartFrame = vectorRot.size() -1;
	int iEndFrame = 0;

	//首先根据ActionList获得有效的帧数范围
	for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
	{
		if( iStartFrame > MessageProcess::GetInstance()->m_ActionList[i].startFrameID )
			iStartFrame = MessageProcess::GetInstance()->m_ActionList[i].startFrameID;
		if( iEndFrame < MessageProcess::GetInstance()->m_ActionList[i].endFrameID )
			iEndFrame = MessageProcess::GetInstance()->m_ActionList[i].endFrameID;
	}
	/*
	遍历时间轴上的连续的3个关键帧，如果按每个关键帧对于时间的比例，
	计算第2号关键帧是否正好是1，3关键帧的差值数据，
	如果是的话就删除此关键帧，然后从头再遍历，再执行上述过程，直到最后找不到可删除的关键帧，
	或者关键帧数小于3，就停止。
	*/
	for(int i=0; i<vectorRot.size(); i++)
	{
		sgp::SGPMF1KeyFrame keyframe;
		keyframe.m_iFrameID = i;
		keyframe.m_fParam[0] = vectorRot[i].x;
		keyframe.m_fParam[1] = vectorRot[i].y;
		keyframe.m_fParam[2] = vectorRot[i].z;
		keyframe.m_fParam[3] = vectorRot[i].w;
		CompressedTrack.add(keyframe);
	}



	int index = 0;
	if( !MessageProcess::GetInstance()->m_bUseAnimationCompress )
		index = CompressedTrack.size();
	while( index < CompressedTrack.size()-2 && CompressedTrack.size() > 2 )
	{
		sgp::SGPMF1KeyFrame& first = CompressedTrack.getReference(index);
		sgp::SGPMF1KeyFrame& second = CompressedTrack.getReference(index+1);
		sgp::SGPMF1KeyFrame& third = CompressedTrack.getReference(index+2);


		sgp::Quaternion firstQuat(first.m_fParam[0], first.m_fParam[1], first.m_fParam[2], first.m_fParam[3]);
		sgp::Quaternion secondQuat(second.m_fParam[0], second.m_fParam[1], second.m_fParam[2], second.m_fParam[3]);
		sgp::Quaternion thirdQuat(third.m_fParam[0], third.m_fParam[1], third.m_fParam[2], third.m_fParam[3]);

		float t = float(second.m_iFrameID - first.m_iFrameID) / float(third.m_iFrameID - first.m_iFrameID);
		firstQuat.Slerp(thirdQuat, t);
		bool bSame = sgp::FloatCmp(	firstQuat.x, secondQuat.x, KeyFrameValueDelta ) == 0;
		bSame = bSame && (sgp::FloatCmp( firstQuat.y, secondQuat.y, KeyFrameValueDelta ) == 0 );
		bSame = bSame && (sgp::FloatCmp( firstQuat.z, secondQuat.z, KeyFrameValueDelta) == 0 );
		bSame = bSame && (sgp::FloatCmp( firstQuat.w, secondQuat.w, KeyFrameValueDelta) == 0 );

		// Action List start frame and end frame must be keyframe
		for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
		{
			if( (second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].startFrameID) ||
				(second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].endFrameID) )
			{
				bSame = false;
				break;
			}
		}

		if( bSame )
		{
			// 删除中间的关键帧,从头再遍历
			CompressedTrack.remove(index+1);
			index = 0;
			continue;
		}

		index++;
	}

	//根据ActionList的有效的帧数范围，裁剪多余的关键帧
	if( MessageProcess::GetInstance()->m_ActionList.size() > 0 )
	{
		for( int i=0; i<CompressedTrack.size(); i++ )
		{
			if( (CompressedTrack[i].m_iFrameID < iStartFrame) ||
				(CompressedTrack[i].m_iFrameID > iEndFrame) )
			{
				CompressedTrack.remove(i);
				i = 0;
			}
		}
	}
	return;
}

void SGP_MF1Candidate::CompressTrackScalesKeyFrame(sgp::Array<sgp::SGPMF1ScaleKeyFrame>& CompressedTrack, const sgp::Array<Point3>& vectorScale)
{
	const double KeyFrameValueDelta = 0.005;

	int iStartFrame = vectorScale.size() -1;
	int iEndFrame = 0;

	//首先根据ActionList获得有效的帧数范围
	for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
	{
		if( iStartFrame > MessageProcess::GetInstance()->m_ActionList[i].startFrameID )
			iStartFrame = MessageProcess::GetInstance()->m_ActionList[i].startFrameID;
		if( iEndFrame < MessageProcess::GetInstance()->m_ActionList[i].endFrameID )
			iEndFrame = MessageProcess::GetInstance()->m_ActionList[i].endFrameID;
	}
	/*
	遍历时间轴上的连续的3个关键帧，如果按每个关键帧对于时间的比例，
	计算第2号关键帧是否正好是1，3关键帧的差值数据，
	如果是的话就删除此关键帧，然后从头再遍历，再执行上述过程，直到最后找不到可删除的关键帧，
	或者关键帧数小于3，就停止。
	*/
	for(int i=0; i<vectorScale.size(); i++)
	{
		sgp::SGPMF1ScaleKeyFrame keyframe;
		keyframe.m_iFrameID = i;
		keyframe.m_scale = vectorScale[i].x;

		CompressedTrack.add(keyframe);
	}


	int index = 0;
	if( !MessageProcess::GetInstance()->m_bUseAnimationCompress )
		index = CompressedTrack.size();
	while( index < CompressedTrack.size()-2 && CompressedTrack.size() > 2 )
	{
		sgp::SGPMF1ScaleKeyFrame& first = CompressedTrack.getReference(index);
		sgp::SGPMF1ScaleKeyFrame& second = CompressedTrack.getReference(index+1);
		sgp::SGPMF1ScaleKeyFrame& third = CompressedTrack.getReference(index+2);


		bool bSame = sgp::DoubleCmp(
			(double)second.m_scale,
			(double(third.m_iFrameID - second.m_iFrameID) * first.m_scale + double(second.m_iFrameID - first.m_iFrameID) * third.m_scale) / double(third.m_iFrameID - first.m_iFrameID), KeyFrameValueDelta
			) == 0;

		// Action List start frame and end frame must be keyframe
		for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
		{
			if( (second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].startFrameID) ||
				(second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].endFrameID) )
			{
				bSame = false;
				break;
			}
		}

		if( bSame )
		{
			// 删除中间的关键帧,从头再遍历
			CompressedTrack.remove(index+1);
			index = 0;
			continue;
		}

		index++;
	}

	//根据ActionList的有效的帧数范围，裁剪多余的关键帧
	if( MessageProcess::GetInstance()->m_ActionList.size() > 0 )
	{
		for( int i=0; i<CompressedTrack.size(); i++ )
		{
			if( (CompressedTrack[i].m_iFrameID < iStartFrame) ||
				(CompressedTrack[i].m_iFrameID > iEndFrame) )
			{
				CompressedTrack.remove(i);
				i = 0;
			}
		}
	}
	return;
}
void SGP_MF1Candidate::CompressTrackVisibleKeyFrame(sgp::Array<sgp::SGPMF1VisibleKeyFrame>& CompressedTrack, const sgp::Array<bool>& vectorVisible)
{

	int iStartFrame = vectorVisible.size() -1;
	int iEndFrame = 0;

	//首先根据ActionList获得有效的帧数范围
	for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
	{
		if( iStartFrame > MessageProcess::GetInstance()->m_ActionList[i].startFrameID )
			iStartFrame = MessageProcess::GetInstance()->m_ActionList[i].startFrameID;
		if( iEndFrame < MessageProcess::GetInstance()->m_ActionList[i].endFrameID )
			iEndFrame = MessageProcess::GetInstance()->m_ActionList[i].endFrameID;
	}

	/*
	遍历时间轴上的连续的3个关键帧，如果按每个关键帧对于时间的比例，
	计算第2号关键帧是否正好是1，3关键帧的差值数据，
	如果是的话就删除此关键帧，然后从头再遍历，再执行上述过程，直到最后找不到可删除的关键帧，
	或者关键帧数小于3，就停止。
	*/
	for(int i=0; i<vectorVisible.size(); i++)
	{
		sgp::SGPMF1VisibleKeyFrame keyframe;
		keyframe.m_iFrameID = i;
		keyframe.m_Visible = vectorVisible[i] ? true : false;

		CompressedTrack.add(keyframe);
	}



	int index = 0;
	if( !MessageProcess::GetInstance()->m_bUseAnimationCompress )
		index = CompressedTrack.size();
	while( index < CompressedTrack.size()-2 && CompressedTrack.size() > 2 )
	{
		sgp::SGPMF1VisibleKeyFrame& first = CompressedTrack.getReference(index);
		sgp::SGPMF1VisibleKeyFrame& second = CompressedTrack.getReference(index+1);
		sgp::SGPMF1VisibleKeyFrame& third = CompressedTrack.getReference(index+2);


		bool bSame = (first.m_Visible == second.m_Visible) && (second.m_Visible == third.m_Visible);

		// Action List start frame and end frame must be keyframe
		for( int i=0; i<MessageProcess::GetInstance()->m_ActionList.size(); i++ )
		{
			if( (second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].startFrameID) ||
				(second.m_iFrameID == MessageProcess::GetInstance()->m_ActionList[i].endFrameID) )
			{
				bSame = false;
				break;
			}
		}

		if( bSame )
		{
			// 删除中间的关键帧,从头再遍历
			CompressedTrack.remove(index+1);
			index = 0;
			continue;
		}

		index++;
	}

	//根据ActionList的有效的帧数范围，裁剪多余的关键帧
	if( MessageProcess::GetInstance()->m_ActionList.size() > 0 )
	{
		for( int i=0; i<CompressedTrack.size(); i++ )
		{
			if( (CompressedTrack[i].m_iFrameID < iStartFrame) ||
				(CompressedTrack[i].m_iFrameID > iEndFrame) )
			{
				CompressedTrack.remove(i);
				i = 0;
			}
		}
	}
	return;
}
