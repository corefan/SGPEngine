#include "stdafx.h"
#include "HelpFunction.h"
//engine header
#include "../SGPLibraryCode/SGPHeader.h"
#include "ModelViewerRenderInterface.h"

float myAbs(float data)
{
	if(data<0) data=-data;
	return data;
}

const float PRECISION=0.0000002f;

bool operator==(const ParticleSinInterpolatorData& data1,const ParticleSinInterpolatorData& data2)
{
	if(myAbs(data1.m_period-data2.m_period)>PRECISION) return false;
	if(myAbs(data1.m_amplitudeMin-data2.m_amplitudeMin)>PRECISION) return false;
	if(myAbs(data1.m_amplitudeMax-data2.m_amplitudeMax)>PRECISION) return false;
	if(myAbs(data1.m_offsetX-data2.m_offsetX)>PRECISION) return false;
	if(myAbs(data1.m_offsetY-data2.m_offsetY)>PRECISION) return false;
	if(myAbs(data1.m_startX-data2.m_startX)>PRECISION) return false;
	if(data1.m_length!=data2.m_length) return false;
	if(data1.m_nbSamples!=data2.m_nbSamples) return false;
	return true;
}

bool operator==(const ParticlePolyInterpolatorData& data1,const ParticlePolyInterpolatorData& data2)
{
	if(myAbs(data1.m_constant-data2.m_constant)>PRECISION) return false;
	if(myAbs(data1.m_linear-data2.m_linear)>PRECISION) return false;
	if(myAbs(data1.m_quadratic-data2.m_quadratic)>PRECISION) return false;
	if(myAbs(data1.m_cubic-data2.m_cubic)>PRECISION) return false;
	if(myAbs(data1.m_startX-data2.m_startX)>PRECISION) return false;
	if(myAbs(data1.m_endX-data2.m_endX)>PRECISION) return false;
	if(data1.m_nbSamples!=data2.m_nbSamples) return false;
	return true;
}

bool operator==(const ParticleSelfDefInterpolatorData& data1,const ParticleSelfDefInterpolatorData& data2)
{
	if(data1.m_count!=data2.m_count) return false;
	for(uint32 i=0;i<data1.m_count;++i)
	{
		ParticleEntryParam& entry1=data1.m_pEntry[i];
		ParticleEntryParam& entry2=data2.m_pEntry[i];
		if(memcmp(&entry1,&entry2,sizeof(ParticleEntryParam))!=0) return false;
	}
	return true;
}

bool operator==(const ParticleInterpolatorParam& data1,const ParticleInterpolatorParam& data2)
{
	if(data1.m_ModelParam!=data2.m_ModelParam) return false;
	if(data1.m_InterpolatorType!=data2.m_InterpolatorType) return false;
	switch(data1.m_InterpolatorType)
	{
	case Interpolator_SelfDefine:
	//	if(memcmp(&(data1.m_SelfDefData),&(data2.m_SelfDefData),sizeof(ParticleSelfDefInterpolatorData))!=0)
	//		return false;
		{
			const ParticleSelfDefInterpolatorData& selfDefData1=data1.m_SelfDefData;
			const ParticleSelfDefInterpolatorData& selfDefData2=data2.m_SelfDefData;
			if(selfDefData1.m_count!=selfDefData2.m_count) return false;
			if(selfDefData1.m_count!=0)
			{
				if(memcmp(selfDefData1.m_pEntry,selfDefData2.m_pEntry,sizeof(ParticleEntryParam)*selfDefData1.m_count)!=0) return false;
			}
		}
		break;
	case Interpolator_Sinusoidal:
	//	if(memcmp(&(data1.m_SinData),&(data2.m_SinData),sizeof(ParticleSinInterpolatorData))!=0)
		if(!(data1.m_SinData==data2.m_SinData)) return false;
		break;
	case Interpolator_Polynomial:
	//	if(memcmp(&(data1.m_PolyData),&(data2.m_PolyData),sizeof(ParticlePolyInterpolatorData))!=0)
		if(!(data1.m_PolyData==data2.m_PolyData)) return false;
		break;
	}
	return true;
}

bool operator==(const ParticleRegularParam& data1,const ParticleRegularParam& data2)
{
	if(memcmp(&data1,&data2,sizeof(ParticleRegularParam))==0) return true;
	else return false;
}

bool operator==(const ParticleModelParam& data1,const ParticleModelParam& data2)
{
	if(data1.m_LifeTimeMin!=data2.m_LifeTimeMin) return false;
	if(data1.m_LifeTimeMax!=data2.m_LifeTimeMax) return false;
	if(data1.m_bImmortal!=data2.m_bImmortal) return false;
	if(data1.m_EnableFlag!=data2.m_EnableFlag) return false;
	if(data1.m_MutableFlag!=data2.m_MutableFlag) return false;
	if(data1.m_RandomFlag!=data2.m_RandomFlag) return false;
	if(data1.m_InterpolatedFlag!=data2.m_InterpolatedFlag) return false;
	if(data1.m_ParamCount!=data2.m_ParamCount) return false;
	if(data1.m_InterpolatorCount!=data2.m_InterpolatorCount) return false;

	uint32 i;
	for(i=0;i<data1.m_ParamCount;++i)
	{
		if(data1.m_pRegularParam[i]==data2.m_pRegularParam[i]) ;
		else return false;
	}
	for(i=0;i<data1.m_InterpolatorCount;++i)
	{
		if(data1.m_pInterpolatorParam[i]==data2.m_pInterpolatorParam[i]) ;
		else return false;
	}
	return true;
}

bool operator==(const ParticleRenderParam& data1,const ParticleRenderParam& data2)
{
	if(data1.m_type!=data2.m_type) return false;
	if(data1.m_blendMode!=data2.m_blendMode) return false;
	switch(data1.m_type)
	{
	case Render_Line:
		if(memcmp(&(data1.m_lineData),&(data2.m_lineData),sizeof(ParticleLineRenderData))!=0) return false;
		break;
	case Render_Quad:
		if(memcmp(&(data1.m_quadData),&(data2.m_quadData),sizeof(ParticleQuadRenderData))!=0) return false;
		break;
	case Render_Point:
		if(memcmp(&(data1.m_pointData),&(data2.m_pointData),sizeof(ParticlePointRenderData))!=0) return false;
		break;
	}
	return true;
}

bool operator==(const ParticleZoneParam& data1,const ParticleZoneParam &data2)
{
	if(data1.m_bFullZone!=data2.m_bFullZone) return false;
	if(data1.m_ZoneType!=data2.m_ZoneType) return false;
	switch(data1.m_ZoneType)
	{
	case Zone_Sphere:
		if(memcmp(&(data1.m_sphereZoneData),&(data2.m_sphereZoneData),sizeof(ParticleSphereZoneData))!=0) return false;
		break;
	case Zone_AABox:
		if(memcmp(&(data1.m_aaboxZoneData),&(data2.m_aaboxZoneData),sizeof(ParticleAABoxZoneData))!=0) return false;
		break;
	case Zone_Line:
		if(memcmp(&(data1.m_lineZoneData),&(data2.m_lineZoneData),sizeof(ParticleLineZoneData))!=0) return false;
		break;
	case Zone_Plane:
		if(memcmp(&(data1.m_planeZoneData),&(data2.m_planeZoneData),sizeof(ParticlePlaneZoneData))!=0) return false;
		break;
	case Zone_Ring:
		if(memcmp(&(data1.m_ringZoneData),&(data2.m_ringZoneData),sizeof(ParticleRingZoneData))!=0) return false;
		break;
	case Zone_Point:
		if(memcmp(&(data1.m_pointZoneData),&(data2.m_pointZoneData),sizeof(ParticlePointZoneData))!=0) return false;
		break;
	}
	return true;
}

bool operator==(const ParticleEmitterParam& data1,const ParticleEmitterParam& data2)
{
	if(data1.m_zoneParam==data2.m_zoneParam);
	else return false;
	if(data1.m_Flow!=data2.m_Flow) return false;
	if(data1.m_Tank!=data2.m_Tank) return false;
	if(data1.m_ForceMin!=data2.m_ForceMin) return false;
	if(data1.m_ForceMax!=data2.m_ForceMax) return false;
	if(data1.m_EmitterType!=data2.m_EmitterType) return false;
	switch(data1.m_EmitterType)
	{
	case Emitter_Straight:
		if(memcmp(&(data1.m_straightEmitterData),&(data2.m_straightEmitterData),sizeof(ParticleStraightEmitterData))!=0)
			return false;
		break;
	case Emitter_Static:
		break;
	case Emitter_Spheric:
		if(memcmp(&(data1.m_sphericEmitterData),&(data2.m_sphericEmitterData),sizeof(ParticleSphericEmitterData))!=0)
			return false;
		break;
	case Emitter_Random:
		break;
	case Emitter_Normal:
		if(memcmp(&(data1.m_normalEmitterData),&(data2.m_normalEmitterData),sizeof(ParticleNormalEmitterData))!=0)
			return false;
		break;
	}
	return true;
}

bool operator==(const ParticleModifierParam& data1,const ParticleModifierParam& data2)
{
	if(data1.m_ModifierType!=data2.m_ModifierType) return false;
	switch(data1.m_ModifierType)
	{
	case Modifier_Obstacle:
		if(memcmp(&(data1.m_ObstacleData),&(data2.m_ObstacleData),sizeof(ParticleObstacleData))!=0)
			return false;
		break;
	case Modifier_PointMass:
		if(memcmp(&(data1.m_PointMassData),&(data2.m_PointMassData),sizeof(ParticlePointMassData))!=0)
			return false;
		break;
	case Modifier_LinearForce:
		if(memcmp(&(data1.m_LinearForceData),&(data1.m_LinearForceData),sizeof(ParticleLinearForceData))!=0)
			return false;
		break;
	}
	if(data1.m_bCustomZone!=data2.m_bCustomZone) return false;
	if(data1.m_bCustomZone)
	{
		if(data1.m_ZoneParam==data2.m_ZoneParam) ;
		else return false;
	}
	return true;
}

bool operator==(const ParticleGroupParam& data1,const ParticleGroupParam& data2)
{
	if(memcmp(data1.m_GroupName,data2.m_GroupName,16)!=0) return false;
	if(data1.m_EnableAABBComputing!=data2.m_EnableAABBComputing) return false;
	if(data1.m_EnableComputeDistance!=data2.m_EnableComputeDistance) return false;
	if(data1.m_EnableSorting!=data2.m_EnableSorting) return false;
	if(data1.m_Gravity[0]!=data2.m_Gravity[0]) return false;
	if(data1.m_Gravity[1]!=data2.m_Gravity[1]) return false;
	if(data1.m_Gravity[2]!=data2.m_Gravity[2]) return false;
	if(data1.m_Capacity!=data2.m_Capacity) return false;
	if(data1.m_Friction!=data2.m_Friction) return false;
	if(data1.m_nEmitterCount!=data2.m_nEmitterCount) return false;
	if(data1.m_nModifierCount!=data2.m_nModifierCount) return false;

	if(data1.m_ModelParam==data2.m_ModelParam) ;
	else return false;

	if(data1.m_RenderParam==data2.m_RenderParam) ;
	else return false;

	uint32 i;
	for(i=0;i<data1.m_nEmitterCount;++i)
	{
		if(data1.m_pEmitterParam[i]==data2.m_pEmitterParam[i]) ;
		else return false;
	}
	for(i=0;i<data1.m_nModifierCount;++i)
	{
		if(data1.m_pModifierParam[i]==data2.m_pModifierParam[i]) ;
		else return false;
	}
	return true;
}

bool operator==(const ParticleSystemParam& data1,const ParticleSystemParam& data2)
{
	if(data1.m_bEnableAABBCompute!=data2.m_bEnableAABBCompute) return false;
	if(data1.m_groupCount!=data2.m_groupCount) return false;

	uint32 i;
	for(i=0;i<data1.m_groupCount;++i)
	{
		if(data1.m_pGroupParam[i]==data2.m_pGroupParam[i]) ;
		else return false;
	}
	return true;
}

void ParticleSystemCopy(const ParticleSystemParam& source,ParticleSystemParam& dest)
{
	uint32 i;
	memcpy(&dest,&source,sizeof(ParticleSystemParam));
	if(source.m_groupCount!=0)
	{
		dest.m_pGroupParam=new ParticleGroupParam[source.m_groupCount];
		memcpy(dest.m_pGroupParam,source.m_pGroupParam,sizeof(ParticleGroupParam)*source.m_groupCount);
		for(i=0;i<source.m_groupCount;++i)
		{
			ParticleGroupParam& destGroup=dest.m_pGroupParam[i];
			ParticleGroupParam& sourceGroup=source.m_pGroupParam[i];

			// Model
			ParticleModelParam& destModel=destGroup.m_ModelParam;
			ParticleModelParam& sourceModel=sourceGroup.m_ModelParam;
			// Normal param
			if(sourceModel.m_ParamCount>0)
			{
				destModel.m_pRegularParam=new ParticleRegularParam[sourceModel.m_ParamCount];
				memcpy(destModel.m_pRegularParam,sourceModel.m_pRegularParam,sizeof(ParticleRegularParam)*sourceModel.m_ParamCount);
			}
			else
				destModel.m_pRegularParam=NULL;
			// Interpolator param
			if(sourceModel.m_InterpolatorCount>0)
			{
				destModel.m_pInterpolatorParam=new ParticleInterpolatorParam[sourceModel.m_InterpolatorCount];
				memcpy(destModel.m_pInterpolatorParam,sourceModel.m_pInterpolatorParam,sizeof(ParticleInterpolatorParam)*sourceModel.m_InterpolatorCount);
			}
			else
				destModel.m_pInterpolatorParam=NULL;
			for(uint32 j=0;j<sourceModel.m_InterpolatorCount;++j)
			{
				ParticleInterpolatorParam& destInterpolator=destModel.m_pInterpolatorParam[j];
				ParticleInterpolatorParam& sourceInterpolator=sourceModel.m_pInterpolatorParam[j];
				if(sourceInterpolator.m_InterpolatorType==Interpolator_SelfDefine)
				{
					ParticleSelfDefInterpolatorData& destSelfDef=destInterpolator.m_SelfDefData;
					ParticleSelfDefInterpolatorData& sourceSelfDef=sourceInterpolator.m_SelfDefData;
					if(sourceSelfDef.m_count!=0)
					{
						destSelfDef.m_pEntry=new ParticleEntryParam[sourceSelfDef.m_count];
						memcpy(destSelfDef.m_pEntry,sourceSelfDef.m_pEntry,sizeof(ParticleEntryParam)*sourceSelfDef.m_count);
					}
					else
						destSelfDef.m_pEntry=NULL;
				}
			}

			// Emitter
			if(sourceGroup.m_nEmitterCount>0)
			{
				destGroup.m_pEmitterParam=new ParticleEmitterParam[sourceGroup.m_nEmitterCount];
				memcpy(destGroup.m_pEmitterParam,sourceGroup.m_pEmitterParam,sizeof(ParticleEmitterParam)*sourceGroup.m_nEmitterCount);
			}
			else
				destGroup.m_pEmitterParam=NULL;

			// Modifier
			if(sourceGroup.m_nModifierCount>0)
			{
				destGroup.m_pModifierParam=new ParticleModifierParam[sourceGroup.m_nModifierCount];
				memcpy(destGroup.m_pModifierParam,sourceGroup.m_pModifierParam,sizeof(ParticleModifierParam)*sourceGroup.m_nModifierCount);
			}
			else
				destGroup.m_pModifierParam=NULL;
		}
	}
}

void RegisterTexture(SGPMF1ParticleTag& particleTag)
{
	ParticleSystemParam& systemParam=particleTag.m_SystemParam;
	for(uint32 j=0;j<systemParam.m_groupCount;++j)
	{
		ParticleRenderParam& renderParam=systemParam.m_pGroupParam[j].m_RenderParam;
		switch(renderParam.m_type)
		{
		case Render_Point:
			{
				char* texPath=renderParam.m_pointData.m_texPath;
				ModelViewerRenderInterface::GetInstance()->RegisterTexture(texPath);
			}
			break;
		case Render_Quad:
			{
				char* texPath=renderParam.m_quadData.m_texPath;
				ModelViewerRenderInterface::GetInstance()->RegisterTexture(texPath);
			}
			break;
		}
	}
}

void UnRegisterTexture(SGPMF1ParticleTag& particleTag)
{
	ParticleSystemParam& systemParam=particleTag.m_SystemParam;
	for(uint32 j=0;j<systemParam.m_groupCount;++j)
	{
		ParticleRenderParam& renderParam=systemParam.m_pGroupParam[j].m_RenderParam;
		switch(renderParam.m_type)
		{
		case Render_Point:
			{
				char* texPath=renderParam.m_pointData.m_texPath;
				ModelViewerRenderInterface::GetInstance()->UnRegisterTexture(texPath);
			}
			break;
		case Render_Quad:
			{
				char* texPath=renderParam.m_quadData.m_texPath;
				ModelViewerRenderInterface::GetInstance()->UnRegisterTexture(texPath);
			}
			break;
		}
	}
}

bool operator==(const SGPMF1ConfigSetting& data1,const SGPMF1ConfigSetting& data2)
{
	if(data1.MeshConfigNum!=data2.MeshConfigNum) return false;
	if(data1.ReplaceTextureConfigNum!=data2.ReplaceTextureConfigNum) return false;
	if(data1.ParticleConfigNum!=data2.ParticleConfigNum) return false;
	if(data1.RibbonConfigNum!=data2.RibbonConfigNum) return false;

	if(memcmp(data1.pMeshConfigList,data2.pMeshConfigList,data1.MeshConfigNum*sizeof(SGPMF1ConfigSetting::MeshConfig))!=0)
		return false;
	if(memcmp(data1.pReplaceTextureConfigList,data2.pReplaceTextureConfigList,data1.ReplaceTextureConfigNum*sizeof(SGPMF1ConfigSetting::ReplaceTextureConfig))!=0)
		return false;
	if(memcmp(data1.pParticleConfigList,data2.pParticleConfigList,data1.ParticleConfigNum*sizeof(SGPMF1ConfigSetting::ParticleConfig))!=0)
		return false;
	if(memcmp(data1.pRibbonConfigList,data2.pRibbonConfigList,data1.RibbonConfigNum*sizeof(SGPMF1ConfigSetting::RibbonConfig))!=0)
		return false;

	return true;
}

void ConfigSettingCopy(const sgp::SGPMF1ConfigSetting& source,sgp::SGPMF1ConfigSetting& dest)
{
	// delete current
	if(dest.pMeshConfigList!=NULL)
	{
		delete [] dest.pMeshConfigList;
		dest.pMeshConfigList=NULL;
	}
	if(dest.pReplaceTextureConfigList!=NULL)
	{
		delete [] dest.pReplaceTextureConfigList;
		dest.pReplaceTextureConfigList=NULL;
	}
	if(dest.pParticleConfigList!=NULL)
	{
		delete [] dest.pParticleConfigList;
		dest.pParticleConfigList=NULL;
	}
	if(dest.pRibbonConfigList!=NULL)
	{
		delete [] dest.pRibbonConfigList;
		dest.pRibbonConfigList=NULL;
	}

	// copy data
	dest.MeshConfigNum=source.MeshConfigNum;
	if(dest.MeshConfigNum!=0)
	{
		dest.pMeshConfigList=new SGPMF1ConfigSetting::MeshConfig[dest.MeshConfigNum];
		memcpy(dest.pMeshConfigList,source.pMeshConfigList,sizeof(SGPMF1ConfigSetting::MeshConfig)*dest.MeshConfigNum);
	}

	dest.ReplaceTextureConfigNum=source.ReplaceTextureConfigNum;
	if(dest.ReplaceTextureConfigNum!=0)
	{
		dest.pReplaceTextureConfigList=new SGPMF1ConfigSetting::ReplaceTextureConfig[dest.ReplaceTextureConfigNum];
		memcpy(dest.pReplaceTextureConfigList,source.pReplaceTextureConfigList,sizeof(SGPMF1ConfigSetting::ReplaceTextureConfig)*dest.ReplaceTextureConfigNum);
	}

	dest.ParticleConfigNum=source.ParticleConfigNum;
	if(dest.ParticleConfigNum!=0)
	{
		dest.pParticleConfigList=new SGPMF1ConfigSetting::ParticleConfig[dest.ParticleConfigNum];
		memcpy(dest.pParticleConfigList,source.pParticleConfigList,sizeof(SGPMF1ConfigSetting::ParticleConfig)*dest.ParticleConfigNum);
	}

	dest.RibbonConfigNum=source.RibbonConfigNum;
	if(dest.RibbonConfigNum!=0)
	{
		dest.pRibbonConfigList=new SGPMF1ConfigSetting::RibbonConfig[dest.RibbonConfigNum];
		memcpy(dest.pRibbonConfigList,source.pRibbonConfigList,sizeof(SGPMF1ConfigSetting::RibbonConfig)*dest.RibbonConfigNum);
	}
}
