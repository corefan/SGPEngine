
#include "../../SGPLibraryCode/SGPHeader.h"
#include "OpenALSound.h"
#include "SceneSound.h"

using namespace sgp;


SceneSoundListener::SceneSoundListener(SGPDevice *pdevice) 
{
	CMovementController::Init();
	m_device = pdevice;
}

SceneSoundListener::~SceneSoundListener() 
{
}

void SceneSoundListener::Update(float )
{
	ISGPSoundManager *pSoundManager = m_device ? m_device->getSoundManager() : NULL;
	if(pSoundManager)
	{
		pSoundManager->setListenerPosition(GetPos());
		pSoundManager->setListenerOrientation(GetDir(), GetUp());
	}
}







SceneSound::SceneSound(const String& fileName, float referenceDistance, float maxDistance, bool directionalSound) 
{
	CMovementController::Init();
	this->directionalSound = directionalSound;
	
	sound = new Sound(fileName);
	sound->setIsPositional(true);
	sound->setPositionalProperties(referenceDistance, maxDistance);
}

SceneSound::SceneSound(const String& fileName, int size, const char *data, int channels, int freq, int bps, float referenceDistance, float maxDistance, bool directionalSound)
{
	CMovementController::Init();
	this->directionalSound = directionalSound;

	sound = new Sound(size, data, channels, freq, bps);
	sound->setFileName(fileName);
	sound->setIsPositional(true);
	sound->setPositionalProperties(referenceDistance, maxDistance);
}

SceneSound::~SceneSound() 
{
	delete sound;
	sound = NULL;
}

bool SceneSound::isDirectionalSound() const 
{
    return directionalSound;
}
void SceneSound::setDirectionalSound(bool val)
{
    directionalSound = val;
}

void SceneSound::Update(float )
{
	sound->setSoundPosition(GetPos());
	
	if(directionalSound)
	{
		sound->setSoundDirection(GetDir());
	}	
}

Sound *SceneSound::getSound()
{
	return sound;
}
