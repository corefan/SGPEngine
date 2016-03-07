
#include "../../SGPLibraryCode/SGPHeader.h"
#include "OpenALSound.h"
#include "SceneSound.h"
#include "SoundManager.h"

int32 SoundManager::StaticSoundSourceID = 0;

SoundManager::SoundManager(SGPDevice *device) : m_device(device)
{
	initAL();
}


void SoundManager::initAL() 
{
    captureDevice = NULL;
    
	device = alcOpenDevice(0);
	if(device == 0) 
	{
		Logger::getCurrentLogger()->writeToLog(String("InitializeAL: Cannot open preferred device! "), ELL_ERROR);
		return;
	}

	ALCenum error = alcGetError(device);
	if (error != ALC_NO_ERROR)
	{
		alcCloseDevice(device);
		Logger::getCurrentLogger()->writeToLog(String("InitializeAL: Could not open device. ") + String(error), ELL_ERROR);
		return;
	}
	
	context = alcCreateContext(device, 0);
	if (context == 0)
	{
		alcCloseDevice(device);
		Logger::getCurrentLogger()->writeToLog(String("InitializeAL: Could not create context"), ELL_ERROR);
		return;
	}

	error = alcGetError(device);
	if (error != ALC_NO_ERROR)
	{
		alcDestroyContext(context);
		alcCloseDevice(device);
		Logger::getCurrentLogger()->writeToLog(String("InitializeAL: Could not open device. ") + String(error), ELL_ERROR);
		return;
	}
	
	if (alcMakeContextCurrent(context) != ALC_TRUE) 
	{
		alcDestroyContext(context);
		alcCloseDevice(device);
		Logger::getCurrentLogger()->writeToLog(String("InitializeAL: Could not make context current"), ELL_ERROR);
		return;
	}
	
	ALfloat listenerPos[] = { 0.0, 0.0, 0.0 };
	ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };	
	ALfloat listenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
	
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	
	Logger::getCurrentLogger()->writeToLog(String("OpenAL initialized..."), ELL_INFORMATION);
}

SoundManager::~SoundManager() 
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( m_AllSoundSource.getReference(i).pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;
		}
	}
	m_AllSoundSource.clear();

	if( m_SoundListener.pSoundListener )
	{
		delete m_SoundListener.pSoundListener;
		m_SoundListener.pSoundListener = NULL;
	}

	if (context != 0 )
	{
		alcSuspendContext(context);
		alcMakeContextCurrent(0);
		alcDestroyContext(context);
	}
	if (device != 0) 
	{
		alcCloseDevice(device);
	}
}

void SoundManager::setGlobalVolume(float globalVolume)
{
	alListenerf(AL_GAIN, globalVolume);
}

void SoundManager::setListenerPosition(Vector4D position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void SoundManager::setListenerOrientation(Vector4D orientation, Vector4D upVector)
{
	ALfloat ori[6];
	ori[0] = orientation.x;
	ori[1] = orientation.y;
	ori[2] = orientation.z;
	
	ori[3] = upVector.x;
	ori[4] = upVector.y;
	ori[5] = upVector.z;	
	alListenerfv(AL_ORIENTATION,ori);
}

bool SoundManager::recordSound(unsigned int rate, unsigned int sampleSize)
{    
    if(captureDevice)
	{
        Logger::getCurrentLogger()->writeToLog(String("Error: Audio capture already in progress!"), ELL_ERROR);
        return false;
    }
    
    captureDevice = alcCaptureOpenDevice(NULL, rate, AL_FORMAT_STEREO16, sampleSize);
    if (alGetError() != AL_NO_ERROR) 
	{
        captureDevice = NULL;
        return false;
    }
    recordingBufferRate = rate;
    
    recordingBuffer = (ALbyte*) malloc(1);
    recordingBufferSize = 0;
    
    alcCaptureStart(captureDevice);
    return true;
}

Sound *SoundManager::stopRecording(bool generateFloatBuffer)
{
    if(!captureDevice) 
	{
        Logger::getCurrentLogger()->writeToLog(String("No recording in process!"), ELL_ERROR);
        return NULL;
    }
    alcCaptureStop(captureDevice);
    alcCaptureCloseDevice(captureDevice);
    captureDevice = NULL;
    
    Sound *newSound = new Sound(recordingBufferSize, (const char*)recordingBuffer, 2, recordingBufferRate, 16, generateFloatBuffer);
    
    free(recordingBuffer);
    recordingBuffer = NULL;

    return newSound;
}

void SoundManager::Update(float fElapsedTime)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( m_AllSoundSource.getReference(i).pSoundSource )
			m_AllSoundSource.getReference(i).pSoundSource->Update(fElapsedTime);		
	}

	if( m_SoundListener.pSoundListener )
		m_SoundListener.pSoundListener->Update(fElapsedTime);

	UpdateInner();
}



void SoundManager::UpdateInner()
{
    // if recording sound, save samples
    if(captureDevice) 
	{
        ALint samples;
        alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, (ALCsizei)sizeof(ALint), &samples);
        if(samples)
		{
            unsigned int newBufferSize = sizeof(ALbyte) * samples * 4;
            ALbyte* tmp = (ALbyte*) realloc(recordingBuffer, recordingBufferSize + newBufferSize);
			if (tmp == NULL)
			{
				Logger::getCurrentLogger()->writeToLog(String("No Memory in UpdateInner()!"), ELL_ERROR);
				return;
			}
			else
				recordingBuffer = tmp;
            alcCaptureSamples(captureDevice, (ALCvoid *)(recordingBuffer+recordingBufferSize), samples);
            recordingBufferSize += newBufferSize;
        }
    }
}


void SoundManager::addListener(const String& name)
{
	if(m_SoundListener.pSoundListener)
	{
		delete m_SoundListener.pSoundListener;
		m_SoundListener.pSoundListener = NULL;		
	}

	m_SoundListener.pSoundListener = new SceneSoundListener(m_device);
	m_SoundListener.SoundListenerName = name;
}

void SoundManager::addSceneSoundSource(const String& name, const String& soundFileName, float referenceDistance, float maxDistance, bool directionalSound, bool autoPlay, bool bLoop)
{
	StaticSoundSourceID++;

	OpenALSoundSource OpenALSoundSourceStr;
	OpenALSoundSourceStr.SoundSourceName = name;
	OpenALSoundSourceStr.SoundSourceFileName = soundFileName;
	OpenALSoundSourceStr.SoundSourceID = StaticSoundSourceID;
	OpenALSoundSourceStr.pSoundSource = new SceneSound(m_WorkingDir + soundFileName, referenceDistance, maxDistance, directionalSound);
	
	m_AllSoundSource.add(OpenALSoundSourceStr);

	if(autoPlay)
		OpenALSoundSourceStr.pSoundSource->getSound()->Play(bLoop);
}

CMovementController* SoundManager::getListener()
{
	if( m_SoundListener.pSoundListener )
		return m_SoundListener.pSoundListener;
	return NULL;
}

void* SoundManager::getSceneSoundSourceByID(int32 id)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceID == id) && m_AllSoundSource.getReference(i).pSoundSource )
			return &(m_AllSoundSource.getReference(i));		
	}
	return NULL;
}

void* SoundManager::getSceneSoundSourceByName(const String& name)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if(m_AllSoundSource.getReference(i).SoundSourceName == name)
			return &(m_AllSoundSource.getReference(i));
	}
	return NULL;
}

void SoundManager::deleteSceneSoundSourceByName(const String& name)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceName == name) && m_AllSoundSource.getReference(i).pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;

			m_AllSoundSource.remove(i);
			return;		
		}
	}
}

void SoundManager::deleteSceneSoundSourceByID(int32 id)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceID == id) && m_AllSoundSource.getReference(i).pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;
	
			m_AllSoundSource.remove(i);
			return;
		}
	}
}