#pragma once

namespace sgp
{
	struct ParticleSinInterpolatorData;
	struct ParticlePolyInterpolatorData;
	struct ParticleSelfDefInterpolatorData;
	struct ParticleInterpolatorParam;
	struct ParticleRegularParam;
	struct ParticleModelParam;
	struct ParticleRenderParam;
	struct ParticleZoneParam;
	struct ParticleEmitterParam;
	struct ParticleModifierParam;
	struct ParticleGroupParam;
	struct ParticleSystemParam;
	struct SGPMF1ParticleTag;
	struct SGPMF1ConfigSetting;
}

// Particle Module
bool operator==(const sgp::ParticleSinInterpolatorData& data1,const sgp::ParticleSinInterpolatorData& data2);
bool operator==(const sgp::ParticlePolyInterpolatorData& data1,const sgp::ParticlePolyInterpolatorData& data2);
bool operator==(const sgp::ParticleSelfDefInterpolatorData& data1,const sgp::ParticleSelfDefInterpolatorData& data2);
bool operator==(const sgp::ParticleInterpolatorParam& data1,const sgp::ParticleInterpolatorParam& data2);
bool operator==(const sgp::ParticleRegularParam& data1,const sgp::ParticleRegularParam& data2);
bool operator==(const sgp::ParticleModelParam& data1,const sgp::ParticleModelParam& data2);
bool operator==(const sgp::ParticleRenderParam& data1,const sgp::ParticleRenderParam& data2);
bool operator==(const sgp::ParticleZoneParam& data1,const sgp::ParticleZoneParam &data2);
bool operator==(const sgp::ParticleEmitterParam& data1,const sgp::ParticleEmitterParam& data2);
bool operator==(const sgp::ParticleModifierParam& data1,const sgp::ParticleModifierParam& data2);
bool operator==(const sgp::ParticleGroupParam& data1,const sgp::ParticleGroupParam& data2);
bool operator==(const sgp::ParticleSystemParam& data1,const sgp::ParticleSystemParam& data2);
void ParticleSystemCopy(const sgp::ParticleSystemParam& source,sgp::ParticleSystemParam& dest);
void RegisterTexture(sgp::SGPMF1ParticleTag& particleTag);
void UnRegisterTexture(sgp::SGPMF1ParticleTag& particleTag);

// Config Setting Module
bool operator==(const sgp::SGPMF1ConfigSetting& data1,const sgp::SGPMF1ConfigSetting& data2);
void ConfigSettingCopy(const sgp::SGPMF1ConfigSetting& source,sgp::SGPMF1ConfigSetting& dest);

