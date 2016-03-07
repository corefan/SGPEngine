
#include "../../SGPLibraryCode/SGPHeader.h"
#include "OpenALSound.h"
#include "SceneSound.h"
#include "SoundManagerMT.h"

int32 SoundManagerMT::StaticSoundSourceID = 0;

SoundManagerMT::SoundManagerMT(SGPDevice *device) : Thread ("Sound Manager Multi Thread"), m_device(device)
{
	Logger::getCurrentLogger()->writeToLog(String("Create Sound Manager Thread"), ELL_INFORMATION);

    // give the threads a background priority (lower)
    startThread(3);

	// sound thread update frequency is 10 ms
	interval = 10;

	initAL();
}


void SoundManagerMT::initAL() 
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

// called in main thread, Last chance for deleting raw sound file data
void SoundManagerMT::removeAll()
{
	const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

	for( int i=0; i<m_CachedSoundFileRawDataList.size(); i++ )
	{
		if(m_CachedSoundFileRawDataList[i].pRawData)
		{
			if(m_CachedSoundFileRawDataList[i].getReferenceCount() != 0)
				Logger::getCurrentLogger()->writeToLog(String("Found Undeleted cached Sound data ") + m_CachedSoundFileRawDataList.getReference(i).SoundFileName, ELL_WARNING);
			
			delete m_CachedSoundFileRawDataList.getReference(i).pRawData;
			m_CachedSoundFileRawDataList.getReference(i).pRawData = NULL;
		}
	}
	m_CachedSoundFileRawDataList.clear();

	m_LoadingSounds.clear();
	m_DeletingSounds.clear();
}

SoundManagerMT::~SoundManagerMT() 
{
	Logger::getCurrentLogger()->writeToLog(String("Shutdown Sound Manager Thread"), ELL_INFORMATION);

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

	removeAll();

	// allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread (2000);
}


void SoundManagerMT::setGlobalVolume(float globalVolume)
{
	alListenerf(AL_GAIN, globalVolume);
}

void SoundManagerMT::setListenerPosition(Vector4D position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void SoundManagerMT::setListenerOrientation(Vector4D orientation, Vector4D upVector)
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

bool SoundManagerMT::recordSound(unsigned int rate, unsigned int sampleSize)
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

Sound *SoundManagerMT::stopRecording(bool generateFloatBuffer)
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
    
    return newSound;
}

void SoundManagerMT::UpdateInner()
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

void SoundManagerMT::Update(float fElapsedTime)
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

// called in sound thread
void SoundManagerMT::run()
{
    // this is the code that runs this thread - we'll loop continuously,


    // threadShouldExit() returns true when the stopThread() method has been
    // called, so we should check it often, and exit as soon as it gets flagged.
    while (! threadShouldExit())
    {
        // sleep a bit so the threads don't all grind the CPU to a halt..
        wait (interval);

		const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

		for( int i=0; i<m_DeletingSounds.size(); i++ )
		{
			int idx = m_LoadingSounds.indexOf( m_DeletingSounds.getReference(i) );
			if( idx != -1 )
			{					
				m_LoadingSounds.remove(idx);
				m_DeletingSounds.remove(i);
				i--;					
			}			
		}
		for( int i=0; i<m_LoadingSounds.size(); i++ )
		{
			if( m_LoadingSounds.getReference(i).pRawDataStr )
				continue;

			bool bFound = false;
			for( int j=0; j<m_CachedSoundFileRawDataList.size(); j++ )
			{
				if(m_LoadingSounds.getReference(i).SoundFileName == m_CachedSoundFileRawDataList.getReference(j).SoundFileName)
				{					
					m_LoadingSounds.getReference(i).pRawDataStr = &(m_CachedSoundFileRawDataList.getReference(j));

					bFound = true;
					break;
				}
			}

			if(!bFound)
			{
				// Create one new RawDataStr
				CachedSoundFileRawDataStr RawDataStr;
				RawDataStr.pRawData = Sound::LoadSoundFileMT(m_WorkingDir + m_LoadingSounds.getReference(i).SoundFileName, RawDataStr);
				RawDataStr.bReady = true;
				RawDataStr.SoundFileName = m_LoadingSounds.getReference(i).SoundFileName;

				m_CachedSoundFileRawDataList.add(RawDataStr);

				m_LoadingSounds.getReference(i).pRawDataStr = &(m_CachedSoundFileRawDataList.getReference(m_CachedSoundFileRawDataList.size() - 1));
			}
		}

		for( int i=0; i<m_DeletingSounds.size(); i++ )
		{
			bool bFound = false;
			for( int j=0; j<m_CachedSoundFileRawDataList.size(); j++ )
			{
				if( m_DeletingSounds.getReference(i).SoundFileName == 
					m_CachedSoundFileRawDataList.getReference(j).SoundFileName )
				{
					m_CachedSoundFileRawDataList.getReference(j).decReferenceCount();
					if(m_CachedSoundFileRawDataList.getReference(j).getReferenceCount() == 0)
					{
						if(m_device && m_device->getTimer())
							m_CachedSoundFileRawDataList.getReference(j).deleteTimeStamp = m_device->getTimer()->getTime();
					}

					Logger::getCurrentLogger()->writeToLog(String("Delete sound ref in sound Thread") + m_DeletingSounds.getReference(i).SoundFileName, ELL_INFORMATION);
			
					m_DeletingSounds.remove(i);
					i--;
					bFound = true;
					break;
				}
			}
			if( !bFound )
			{
				Logger::getCurrentLogger()->writeToLog(String("Delete sound ref in sound Thread, BUT not found!!!") + m_DeletingSounds.getReference(i).SoundFileName, ELL_ERROR);
			}
		}

		// Remove overtime CachedSoundFileRawData
		for( int i=0; i<m_CachedSoundFileRawDataList.size(); i++ )
		{
			if( m_CachedSoundFileRawDataList.getReference(i).pRawData &&
				m_CachedSoundFileRawDataList.getReference(i).bReady &&
				m_CachedSoundFileRawDataList.getReference(i).getReferenceCount() == 0 &&
				m_CachedSoundFileRawDataList.getReference(i).deleteTimeStamp != 0 &&
				m_device && m_device->getTimer() &&
				m_device->getTimer()->getTime() - m_CachedSoundFileRawDataList.getReference(i).deleteTimeStamp > RESOURCE_SOUND_TO_FREE_KEEPTIME )
			{
				delete m_CachedSoundFileRawDataList.getReference(i).pRawData;
				m_CachedSoundFileRawDataList.getReference(i).pRawData = NULL;

				m_CachedSoundFileRawDataList.remove(i);
				i--;
			}
		}
	}
}

// called in main thread, change cached sound file data ref count and create sound resource
void SoundManagerMT::syncSoundResource()
{
	// Called In main thread
	const GenericScopedLock<CriticalSection> s1 (resourceArrayLock);

	for( int i=0; i<m_LoadingSounds.size(); i++ )
	{
		if( m_LoadingSounds.getReference(i).pRawDataStr &&
			m_LoadingSounds.getReference(i).pRawDataStr->bReady &&
			m_LoadingSounds.getReference(i).pRawDataStr->pRawData )
		{
			// In main thread, create sound resource
			StaticSoundSourceID++;

			OpenALSoundSource OpenALSoundSourceStr;
			OpenALSoundSourceStr.SoundSourceName = m_LoadingSounds.getReference(i).SoundName;
			OpenALSoundSourceStr.SoundSourceFileName = m_LoadingSounds.getReference(i).SoundFileName;
			OpenALSoundSourceStr.SoundSourceID = StaticSoundSourceID;
			OpenALSoundSourceStr.pSoundSource = new SceneSound(
				m_WorkingDir + m_LoadingSounds.getReference(i).SoundFileName, 
				m_LoadingSounds.getReference(i).pRawDataStr->size,
				(const char*)m_LoadingSounds.getReference(i).pRawDataStr->pRawData->getData(),
				m_LoadingSounds.getReference(i).pRawDataStr->channels,
				m_LoadingSounds.getReference(i).pRawDataStr->freq,
				m_LoadingSounds.getReference(i).pRawDataStr->bps,
				m_LoadingSounds.getReference(i).referenceDistance,
				m_LoadingSounds.getReference(i).maxDistance, 
				m_LoadingSounds.getReference(i).directionalSound);

			if(m_LoadingSounds.getReference(i).autoPlay)
				OpenALSoundSourceStr.pSoundSource->getSound()->Play(m_LoadingSounds.getReference(i).bLoop);
			
			m_AllSoundSource.add(OpenALSoundSourceStr);

			m_LoadingSounds.getReference(i).pRawDataStr->incReferenceCount();


			Logger::getCurrentLogger()->writeToLog(String("Create sound resource in main Thread") + m_LoadingSounds.getReference(i).SoundFileName, ELL_INFORMATION);
			
			m_LoadingSounds.remove(i);
			i--;
		}
	}
}

void SoundManagerMT::addListener(const String& name)
{
	if(m_SoundListener.pSoundListener)
	{
		delete m_SoundListener.pSoundListener;
		m_SoundListener.pSoundListener = NULL;		
	}

	m_SoundListener.pSoundListener = new SceneSoundListener(m_device);
	m_SoundListener.SoundListenerName = name;
}

void SoundManagerMT::addSceneSoundSource(const String& name, const String& soundFileName, float referenceDistance, float maxDistance, bool directionalSound, bool autoPlay, bool bLoop)
{
	MTSoundRecordStr LoadRecordStr;
	LoadRecordStr.SoundName = name;
	LoadRecordStr.SoundFileName = soundFileName;
	LoadRecordStr.referenceDistance = referenceDistance;
	LoadRecordStr.maxDistance = maxDistance;
	LoadRecordStr.directionalSound = directionalSound;
	LoadRecordStr.autoPlay = autoPlay;
	LoadRecordStr.bLoop = bLoop;

	m_LoadingSounds.add(LoadRecordStr);

}

void SoundManagerMT::deleteSceneSoundSourceByName(const String& name)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceName == name) && m_AllSoundSource.getReference(i).pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;

			MTSoundRecordStr DelRecordStr;
			DelRecordStr.SoundFileName = m_AllSoundSource.getReference(i).SoundSourceFileName;
			this->m_DeletingSounds.add(DelRecordStr);

			m_AllSoundSource.remove(i);
			return;
		}
	}
}

void SoundManagerMT::deleteSceneSoundSourceByID(int32 id)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceID == id) && m_AllSoundSource.getReference(i).pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;

			MTSoundRecordStr DelRecordStr;
			DelRecordStr.SoundFileName = m_AllSoundSource.getReference(i).SoundSourceFileName;
			this->m_DeletingSounds.add(DelRecordStr);

			m_AllSoundSource.remove(i);
			return;
		}
	}
}

void SoundManagerMT::deleteSceneSoundSource(OpenALSoundSource* pSoundSource)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( m_AllSoundSource.getReference(i) == *pSoundSource )
		{
			delete m_AllSoundSource.getReference(i).pSoundSource;
			m_AllSoundSource.getReference(i).pSoundSource = NULL;

			MTSoundRecordStr DelRecordStr;
			DelRecordStr.SoundFileName = m_AllSoundSource.getReference(i).SoundSourceFileName;
			this->m_DeletingSounds.add(DelRecordStr);

			m_AllSoundSource.remove(i);
			return;
		}
	}
}



CMovementController* SoundManagerMT::getListener()
{
	if( m_SoundListener.pSoundListener )
		return m_SoundListener.pSoundListener;
	return NULL;
}

void* SoundManagerMT::getSceneSoundSourceByID(int32 id)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if( (m_AllSoundSource.getReference(i).SoundSourceID == id) && m_AllSoundSource.getReference(i).pSoundSource )
			return &(m_AllSoundSource.getReference(i));		
	}
	return NULL;
}

void* SoundManagerMT::getSceneSoundSourceByName(const String& name)
{
	for( int i=0; i<m_AllSoundSource.size(); i++ )
	{
		if(m_AllSoundSource.getReference(i).SoundSourceName == name)
			return &(m_AllSoundSource.getReference(i));
	}
	return NULL;
}