#include "SGP_MAX9Interface.h"

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//
SGP_MaxInterface::SGP_MaxInterface()
{
	m_pExpInterface = NULL;
	m_pInterface = NULL;
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//
SGP_MaxInterface::~SGP_MaxInterface()
{
}

//----------------------------------------------------------------------------//
// Create a max interface instance                                            //
//----------------------------------------------------------------------------//
bool SGP_MaxInterface::Create(ExpInterface *pExpInterface, Interface *pInterface)
{
	m_pExpInterface = pExpInterface;
	m_pInterface = pInterface;

	return true;
}

//----------------------------------------------------------------------------//
// Get the current frame of the animation                                     //
//----------------------------------------------------------------------------//
int SGP_MaxInterface::GetCurrentFrame()
{
	return m_pInterface->GetTime() / GetTicksPerFrame();
}

//----------------------------------------------------------------------------//
// Get the current time in seconds of the animation                           //
//----------------------------------------------------------------------------//
float SGP_MaxInterface::GetCurrentTime()
{
	return TicksToSec(m_pInterface->GetTime());
}

//----------------------------------------------------------------------------//
// Get the end frame of the animation                                         //
//----------------------------------------------------------------------------//
int SGP_MaxInterface::GetEndFrame()
{
	return m_pInterface->GetAnimRange().End() / GetTicksPerFrame();
}

//----------------------------------------------------------------------------//
// Get the frames per second (Fps) of the animation                           //
//----------------------------------------------------------------------------//
int SGP_MaxInterface::GetFps()
{
	return GetFrameRate();
}

//----------------------------------------------------------------------------//
// Get the number of selected nodes in the scene                              //
//----------------------------------------------------------------------------//
int SGP_MaxInterface::GetSelectedNodeCount()
{
	// get the number of selected nodes
	int nodeCount;
	nodeCount = m_pInterface->GetSelNodeCount();

	// if nothing is selected, there is always the scene root node
	if(nodeCount == 0) return 1;

	return nodeCount;
}


//----------------------------------------------------------------------------//
// Get the start frame of the animation                                       //
//----------------------------------------------------------------------------//
int SGP_MaxInterface::GetStartFrame()
{
	return m_pInterface->GetAnimRange().Start() / GetTicksPerFrame();
}




int SGP_MaxInterface::GetStartTick()
{
	return m_pInterface->GetAnimRange().Start();
}
int SGP_MaxInterface::GetEndTick()
{
	return m_pInterface->GetAnimRange().End();
}
int SGP_MaxInterface::GetFrameCount()
{
	return (GetEndTick()-GetStartTick())/GetFps() + 1;
}
int SGP_MaxInterface::GetTickPerFrame()
{
	return GetTicksPerFrame();
}


bool SGP_MaxInterface::IsMesh(INode *pNode)
{
	// check for root node
	if( !pNode ) 
		return false;
	if( pNode->IsRootNode() ) 
		return false;

	// check for mesh node
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) 
		return true;

	return false;
}

Mesh* SGP_MaxInterface::GetMesh( INode* pNode )
{
	if( !IsMesh( pNode ) )
		return NULL;

	TimeValue time = 0;

	// get max mesh instance
	ObjectState os;
	os = pNode->EvalWorldState(time);

	Object* obj = os.obj;
	if( !os.obj )
	{
		assert( false );
		return NULL;
	}
	TriObject* triObj = (TriObject *)obj->ConvertToType( time, triObjectClassID );
	if( !triObj )
	{
		assert( false );
		return NULL;
	}

	Mesh* pMesh = &triObj->GetMesh();

	return pMesh;
}


INode* SGP_MaxInterface::GetRootNode()
{
	return m_pInterface->GetRootNode();
}

INode* SGP_MaxInterface::GetSelectedNode()
{
	if( m_pInterface->GetSelNodeCount() <= 0 )
		return NULL;
	return m_pInterface->GetSelNode(0);
}

INode* SGP_MaxInterface::GetSelectedNode( int nNodeId )
{
	if( nNodeId < 0 || nNodeId >= m_pInterface->GetSelNodeCount() )
		return NULL;
	return m_pInterface->GetSelNode(nNodeId);
}

Modifier* SGP_MaxInterface::FindModifier(INode *pINode, Class_ID id)
{
	// get the object reference of the node
	Object *pObject;
	pObject = pINode->GetObjectRef();
	if(pObject == NULL) 
		return NULL;

	// loop through all derived objects
	while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject *pDerivedObject;
		pDerivedObject = static_cast<IDerivedObject *>(pObject);

		// loop through all modifiers
		int stackId;
		for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
		{
			// get the modifier
			Modifier *pModifier;
			pModifier = pDerivedObject->GetModifier(stackId);

			// check if we found the modifier
			if(pModifier->ClassID() == id) 
				return pModifier;
		}

		// continue with next derived object
		pObject = pDerivedObject->GetObjRef();
	}

	return NULL;
}

bool SGP_MaxInterface::IsBone(INode *pNode)
{
	// check for root node
	if(pNode->IsRootNode()) 
		return false;

	// check for bone node
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(BONE_CLASS_ID, 0)) 
		return true;

#if MAX_RELEASE >= 4000
	if(os.obj->ClassID() == BONE_OBJ_CLASSID) 
		return true;
#endif

	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) 
		return false;

	// check for biped node
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID)) 
		return true;

	return false;
}

bool SGP_MaxInterface::IsBipedBone( INode* pNode )
{
	// check for invalid nodes
	if(pNode == NULL) 
		return false;

	// check for root node
	if(pNode->IsRootNode())
		return false;

	// check for bone node
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0)) 
		return false;

	// check for biped node
	Control *pControl;
	pControl = pNode->GetTMController();
	if((pControl->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) || (pControl->ClassID() == BIPBODY_CONTROL_CLASS_ID))
		return true;

	return false;
}

bool SGP_MaxInterface::IsDummy(INode *pNode)
{
	// check for invalid nodes
	if(pNode == NULL)
		return false;
	// check for root node
	if(pNode->IsRootNode()) 
		return false;

	// check for dummy node
	ObjectState os;
	os = pNode->EvalWorldState(0);
	if(os.obj->ClassID() == Class_ID(DUMMY_CLASS_ID, 0))
		return true;

	return false;
}

bool SGP_MaxInterface::IsRenderable( INode* node )
{
	Object *obj = node->EvalWorldState ( 0 ).obj;	
	return obj->IsRenderable() != 0;
}

bool SGP_MaxInterface::IsTarget( INode* pNode )
{
	return pNode->IsTarget() != 0;
}

bool SGP_MaxInterface::IsSelected( INode* pNode )
{
	return pNode->Selected() != 0;
}


DWORD WINAPI SGP_MaxInterface::ProgressFunction(LPVOID arg)
{
	return 0;
}
void SGP_MaxInterface::SetProgressInfo(int percentage)
{
	m_pInterface->ProgressUpdate(percentage);
}
void SGP_MaxInterface::StartProgressInfo(MCHAR* strText)
{
	m_pInterface->ProgressStart(strText, true, ProgressFunction, 0);
}
void SGP_MaxInterface::StopProgressInfo()
{
	m_pInterface->ProgressEnd();
}

/*
*	Get vertex normal using smooth group with normal method.(using RVertex information)
*	FaceVertexIdx is between 0 and 2 local to a triangle
*/
Point3 SGP_MaxInterface::GetVertexNormal( Mesh* pMesh, int faceId, int vertexId, int FaceVertexIdx )
{
	//////////////////////////////////////////////////////////////////////////
	// Below is the way if someone has used "edit normals modifier" to change vertex normal
	MeshNormalSpec *pNormalSpec = pMesh->GetSpecifiedNormals();
	if( pNormalSpec )
	{
		const int NumFaces = pNormalSpec->GetNumFaces();
		const int NumNormals = pNormalSpec->GetNumNormals();

		if( NumFaces != 0 && NumNormals != 0 )
		{
			const int NormalID = pNormalSpec->Face(faceId).GetNormalID(FaceVertexIdx);
			return pNormalSpec->Normal(NormalID).Normalize();
		}
	}
	//////////////////////////////////////////////////////////////////////////

	RVertex *pRVertex;
	pRVertex = pMesh->getRVertPtr(vertexId);


	// get the face
	Face *pFace;
	pFace = &pMesh->faces[faceId];

	// get the smoothing group of the face
	DWORD smGroup;
	smGroup = pFace->smGroup;

	// get the number of normals
	int normalCount;
	normalCount = pRVertex->rFlags & NORCT_MASK;

	// check if the normal is specified ...
	if(pRVertex->rFlags & SPECIFIED_NORMAL)
	{
		return pRVertex->rn.getNormal();
	}
	// ... otherwise, check for a smoothing group
	else if((normalCount > 0) && (smGroup != 0))
	{
		// If there is only one vertex is found in the rn member.
		if(normalCount == 1)
		{
			return pRVertex->rn.getNormal();
		}
		else
		{
			int normalId;
			Point3 n(0,0,0);
			for(normalId = 0; normalId < normalCount; normalId++)
			{
				if(pRVertex->ern[normalId].getSmGroup() & smGroup)
				{
					n = n+pRVertex->ern[normalId].getNormal();
				}
			}
			n = n.Normalize();
			return n;
		}
	}

	// if all fails, return the face normal
	return pMesh->getFaceNormal(faceId);
}

void SGP_MaxInterface::GetNodeTree( INode* pNode )
{
	if( pNode == NULL )
	{
		pNode = GetRootNode();
	}
	if( !pNode )
		return;
	if( pNode != GetRootNode() )
		m_vectorNode.add( pNode );

	for (int i = 0; i < pNode->NumberOfChildren(); i++) 
	{
		GetNodeTree( pNode->GetChildNode( i ) );
	}
}

void SGP_MaxInterface::GetTracks( int nNodeCount, INode** nodes, Track** tracks )
{
	StartProgressInfo(_M("Get node track..."));

	TimeValue nStartTick = GetStartTick();
	TimeValue nEndTick = GetEndTick();
	int nTickPerFrame = GetTickPerFrame();
	int nFrameCount = 0;

	for( TimeValue t = nStartTick; t <= nEndTick; t += nTickPerFrame )
		nFrameCount++;

	for( int i = 0; i < nNodeCount; i++ )
	{
		tracks[i]->vectorVisible.resize( nFrameCount );
		tracks[i]->vectorTrans.resize( nFrameCount );
		tracks[i]->vectorRot.resize( nFrameCount );
		tracks[i]->vectorScale.resize( nFrameCount );

		Matrix3 matrix = nodes[i]->GetObjTMAfterWSM ( 0 );
		bool bMirror = DotProd ( CrossProd ( matrix.GetRow ( 0 ), matrix.GetRow ( 1 ) ), matrix.GetRow ( 2 ) ) < 0.0 ? true : false;
		tracks[i]->bMirror = bMirror;
	}
	TimeValue t = nStartTick;
	for( int nFrameId = 0; nFrameId < nFrameCount; nFrameId++, t += nTickPerFrame )
	{
		SetProgressInfo( 100.0f*nFrameId/nFrameCount );

		for( int nNodeId = 0; nNodeId < nNodeCount; nNodeId++ )
		{
			INode* pNode = nodes[nNodeId];
			Track* pTrack = tracks[nNodeId];

			Matrix3 tm = pNode->GetNodeTM(t);

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
			Point3 Row0 = tm.GetRow(0);
			Point3 Row1 = tm.GetRow(1);
			Point3 Row2 = tm.GetRow(2);
			Point3 Row3 = tm.GetRow(3);
			sgp::swapVariables( Row0.y,  Row0.z );
			sgp::swapVariables( Row1.x,  Row2.x );
			sgp::swapVariables( Row1.y,  Row2.z );
			sgp::swapVariables( Row1.z,  Row2.y );
			sgp::swapVariables( Row3.y,  Row3.z );
			tm.SetRow(0, Row0);
			tm.SetRow(1, Row1);
			tm.SetRow(2, Row2);
			tm.SetRow(3, Row3);


			Point3 trans;
			Quat quat;
			Point3 scale;

			{
				// calculate the translation component
				Point3 p;
				p = tm.GetTrans();

				trans.x = p.x;
				trans.y = p.y;
				trans.z = p.z;

				scale.x = tm.GetRow(0).Length();
				scale.y = tm.GetRow(1).Length();
				scale.z = tm.GetRow(2).Length();

				tm.NoScale();

				// calculate the rotation component
				Quat q(tm);
				if( tracks[nNodeId]->bMirror )
				{
					float m[4][3];
					memcpy( m, &tm, sizeof(float)*4*3 );

					m[0][0] *= -1;
					m[1][0] *= -1;
					m[2][0] *= -1;
	
					Matrix3 mm(m);

					Quat q0(mm);
					q = q0;
				}

				quat.x = q.x;
				quat.y = q.y;
				quat.z = q.z;
				quat.w = q.w;
			}


			pTrack->vectorTrans.getReference(nFrameId) = trans;
			pTrack->vectorRot.getReference(nFrameId) = quat;
			pTrack->vectorScale.getReference(nFrameId) = scale;

			float fv = pNode->GetVisibility( t );
			if( fv == 0 )
				pTrack->vectorVisible.getReference(nFrameId) = false;
			else
				pTrack->vectorVisible.getReference(nFrameId) = true;
		}
	}
	StopProgressInfo();
}

bool SGP_MaxInterface::GetStdMtlChannelBitmapFileName( StdMat* pStdMat, int nChannel, TCHAR szFileName[] )
{
	if( !pStdMat )
	{
		assert( false );
		return false;
	}
	Texmap *tx = pStdMat->GetSubTexmap(nChannel);
	if( !tx  )
		return false;
	if(tx->ClassID() != Class_ID(BMTEX_CLASS_ID,0))
		return false;
	BitmapTex *bmt = (BitmapTex*)tx;
	_tcscpy( szFileName, bmt->GetMapName() );
	return true;
}

void SGP_MaxInterface::GetBoneGroup( Modifier *pModifier,
									 int nModifierType, 
									 INode* pNode, 
									 Mesh* pMesh,
									 int nVertexId,
									 BoneGroup& boneGroup )
{
	if( !pMesh )
	{
		assert( false );
		return;
	}

	if( nVertexId >= pMesh->numVerts )
	{
		assert( false );
		return;
	}

	// static mesh
	if( nModifierType == MODIFIER_NONE )
	{
		INode* pParent = pNode->GetParentNode();
		if( pParent && ( IsBone( pParent ) || IsBipedBone( pParent ) ) )
		{
			Influence infl;
			infl.fWeight = 1.0f;
			strcpy( infl.szBoneName, pParent->GetName() );
			boneGroup.AddInfluence( infl );
		}
	}
	// check for physique modifier
	else if( nModifierType == MODIFIER_PHYSIQUE )
	{
		assert( pModifier && "get bone group error, modifier is null" );
		// create a physique export interface
		IPhysiqueExport *pPhysiqueExport = (IPhysiqueExport *)pModifier->GetInterface(I_PHYINTERFACE);
		if(pPhysiqueExport == NULL)
		{
			return;
		}

		// create a context export interface
		IPhyContextExport *pContextExport = (IPhyContextExport *)pPhysiqueExport->GetContextInterface(pNode);
		if(pContextExport == NULL)
		{
			pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
			return;
		}

		// set the flags in the context export interface
		pContextExport->ConvertToRigid(TRUE);
		pContextExport->AllowBlending(TRUE);

		// get the vertex export interface
		IPhyVertexExport *pVertexExport = (IPhyVertexExport *)pContextExport->GetVertexInterface(nVertexId);
		if(pVertexExport == NULL)
		{
			pPhysiqueExport->ReleaseContextInterface(pContextExport);
			pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);
			return;
		}

		// get the vertex type
		int vertexType = pVertexExport->GetVertexType();

		// handle the specific vertex type
		if(vertexType == RIGID_TYPE)
		{
			// typecast to rigid vertex
			IPhyRigidVertex *pTypeVertex = (IPhyRigidVertex *)pVertexExport;


			Influence infl;
			if( pTypeVertex->GetNode() )
			{
				strcpy( infl.szBoneName, pTypeVertex->GetNode()->GetName() );
				infl.fWeight = 1.0f;
				boneGroup.AddInfluence( infl );
			}
			else 
				return;
		}
		else if(vertexType == RIGID_BLENDED_TYPE)
		{
			// typecast to blended vertex
			IPhyBlendedRigidVertex *pTypeVertex = (IPhyBlendedRigidVertex *)pVertexExport;

			// loop through all influencing bones
			Influence infl;
			for(int nodeId = 0; nodeId < pTypeVertex->GetNumberNodes(); nodeId++)
			{
				strcpy( infl.szBoneName, pTypeVertex->GetNode( nodeId )->GetName() );
				infl.fWeight = pTypeVertex->GetWeight( nodeId );

				boneGroup.AddInfluence( infl );
			}
		}

		// release all interfaces
		pPhysiqueExport->ReleaseContextInterface(pContextExport);
		pModifier->ReleaseInterface(I_PHYINTERFACE, pPhysiqueExport);

	}
	else if( nModifierType == MODIFIER_SKIN)
	{
		assert( pModifier && "get bone group error, modifier is null" );
		// create a skin interface
		ISkin *pSkin = (ISkin*)pModifier->GetInterface(I_SKIN);
		if(pSkin == 0)
		{
			return;
		}

		// create a skin context data interface
		ISkinContextData *pSkinContextData;
		pSkinContextData = (ISkinContextData *)pSkin->GetContextInterface(pNode);
		if(pSkinContextData == NULL)
		{
			pModifier->ReleaseInterface(I_SKIN, pSkin);
			return;
		}

		// loop through all influencing bones
		for(int nodeId = 0; nodeId < pSkinContextData->GetNumAssignedBones(nVertexId); nodeId++)
		{
			// get the bone id
			int boneId = pSkinContextData->GetAssignedBone(nVertexId, nodeId);
			if(boneId < 0)
				continue;

			INode* pBone = pSkin->GetBone( boneId );
			Influence infl;
			strcpy( infl.szBoneName, pBone->GetName() );
			infl.fWeight = pSkinContextData->GetBoneWeight(nVertexId, nodeId);

			boneGroup.AddInfluence( infl );
		}

		// release all interfaces
		pModifier->ReleaseInterface(I_SKIN, pSkin);
	}
}


bool SGP_MaxInterface::GetMtlAnim( StdMat* pStdMtl, ColorTrack& track, int nChannel )
{
	if( pStdMtl == NULL )
	{
		assert( false && "std mtl is NULL" );
		return false;
	}

	int nFrameCount = 0;
	TimeValue nStartTick = GetStartTick();
	TimeValue nEndTick = GetEndTick();
	int nTickPerFrame = GetTickPerFrame();



	track.bTiling = false;

	StdUVGen *uv = NULL;

	Texmap *tx = pStdMtl->GetSubTexmap(nChannel);
	if( tx )
	{
		if( tx->ClassID() == Class_ID( BMTEX_CLASS_ID, 0 ) )
		{
			BitmapTex *bmt = (BitmapTex*)tx;
			uv = bmt->GetUVGen();
			if( uv )
			{
				track.nUTile = (int)uv->GetUScl(0);
				track.nVTile = (int)uv->GetVScl(0);
				if( track.nUTile == 1 && track.nVTile == 1 )
					track.bTiling = false;
				else
					track.bTiling = true;
				track.nStartFrame = bmt->GetStartTime();
				track.fPlaybackRate = bmt->GetPlaybackRate();
				track.nLoopMode = bmt->GetEndCondition();

				if( uv->GetUAng( 0 ) != 0.0f ||
					uv->GetVAng( 0 ) != 0.0f )
				{
					track.fUSpeed = uv->GetUAng( 0 ) / piOver180;
					track.fVSpeed = uv->GetVAng( 0 ) / piOver180;
					track.bUVMoving = true;
				}
				else
					track.bUVMoving = false;
			}
		}
	}


	TimeValue t;
	for( t = nStartTick; t <= nEndTick; t += nTickPerFrame )
		nFrameCount++;


	track.ColorKeyFrame.resize( nFrameCount );

	t = nStartTick;
	for( int i = 0; i < nFrameCount; i++, t += nTickPerFrame )
	{
		SGP_ColorKey key;
		memset( &key, 0x00, sizeof( key ) );
		Color diffuse	= pStdMtl->GetDiffuse( t );
		Color ambient	= pStdMtl->GetAmbient( t );
		Color specular	= pStdMtl->GetSpecular( t );

		Color filter	= pStdMtl->GetFilter( t );
		float alpha		= pStdMtl->GetOpacity( t );
		float shinstr	= pStdMtl->GetShinStr(t);
		float selfillum = pStdMtl->GetSelfIllum( t );

		float uoffset	= 0;
		float voffset	= 0;
		if( uv )
		{
			uoffset	= uv->GetUOffs( t );
			voffset	= uv->GetVOffs( t );
		}
/*
		int	nTransparencyType = pStdMtl->GetTransparencyType();

		key.dwBlendMode = 0;
		switch( nTransparencyType )
		{
		case TRANSP_SUBTRACTIVE:
			key.dwBlendMode |= HR3D_MDX2_MODULATE;
			break;
		case TRANSP_ADDITIVE:
			key.dwBlendMode |= HR3D_MDX2_ADD;
			break;
		case TRANSP_FILTER:
			key.dwBlendMode |= HR3D_MDX2_MODULATE2X;
			break;
		default:
			break;
		};
*/
		key.dr = diffuse.r;
		key.dg = diffuse.g;
		key.db = diffuse.b;

		key.da = alpha;


		if( uv )
		{
			key.uoffset = uv->GetUOffs( t );
			key.voffset = uv->GetVOffs( t );
		}
		else
		{
			key.uoffset = 0;
			key.voffset = 0;
		}

		track.ColorKeyFrame.getReference(i) = key;
	}

	return true;
}





Matrix3 SGP_MaxInterface::GetNodeTM(INode *pNode, float time)
{
	// initialize matrix with the identity
	Matrix3 tm;
	tm.IdentityMatrix();

	// only do this for valid nodes
	if(pNode != NULL)
	{
		// get the node transformation
		tm = pNode->GetNodeTM(SecToTicks(time));

		// make the transformation uniform
		tm.NoScale();
	}

	return tm;
}
