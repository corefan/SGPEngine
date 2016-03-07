#pragma once

#if defined(__APPLE__) && defined(__MACH__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include "AL/al.h"
    #include "AL/alc.h"
#endif

#include "SceneSound.h"

class Sound;
class SceneSound;
class SceneSoundListener;

struct OpenALSoundSource;
struct OpenALSoundListener;

/**
*   this struct is in charge of sound file data cached in memory when running
*	some unused cache data will be deleted from memory forever when time window achieved
*/
struct CachedSoundFileRawDataStr
{
	String SoundFileName;				// NOT absolute file path

	bool bReady;						// Raw data loaded in sound thread and be ready for creating resource in other thread

	uint32 deleteTimeStamp;				// deleting time stamp 

	MemoryBlock* pRawData;				// raw sound file data MemoryBlock
	int size;							// some Parameters for creating OpenAL sound resource
	int channels;
	ALsizei freq;
	int bps;

	Atomic <int> refCount;				// reference count before resource be created / released in sound thread
	
	CachedSoundFileRawDataStr() : bReady(false), deleteTimeStamp(0), pRawData(NULL), size(0), channels(1), freq(44100), bps(16)
	{}

    inline void incReferenceCount() noexcept
    {
        ++refCount;
    }
    inline void decReferenceCount() noexcept
    {
        jassert (getReferenceCount() > 0);

        --refCount;
    }

    inline int getReferenceCount() const noexcept
	{ return refCount.get(); }
};

/**
*   this struct is in charge of Loading / Deleting OpenAL sound in multi-thread
*	Loading struct has all the Parameters setting but pRawDataStr is NULL
*	Deleting struct has only SoundFileName for finding the corresponding CachedSoundFileRawDataStr struct in memory
*/
struct MTSoundRecordStr
{
	String SoundName;
	String SoundFileName;						// NOT absolute file path

	CachedSoundFileRawDataStr *pRawDataStr;		// pointer to struct CachedSoundFileRawDataStr

	bool generateFloatBuffer;
	float referenceDistance;
	float maxDistance;
	bool directionalSound;

	bool autoPlay;
	bool bLoop;

	MTSoundRecordStr() 
		: generateFloatBuffer(false), referenceDistance(1.0f), maxDistance(FLT_MAX), directionalSound(false),
		  autoPlay(true), bLoop(false),
		  pRawDataStr(NULL)
	{}


	bool operator== (const MTSoundRecordStr& other) const noexcept
	{
		return SoundFileName.equalsIgnoreCase( other.SoundFileName );
	}
};


/**
* Controls global sound settings.
*/
class SoundManagerMT : public ISGPSoundManager, public Thread
{
public:
	SoundManagerMT(SGPDevice *device);
	virtual ~SoundManagerMT();

	// Sound Linstener Interface
	virtual void addListener(const String& name);
	virtual CMovementController* getListener();

	virtual void setListenerPosition(sgp::Vector4D position);
	virtual void setListenerOrientation(sgp::Vector4D orientation, sgp::Vector4D upVector);	
	
	// Scene Sound Source Interface
	virtual void addSceneSoundSource(const String& name, const String& soundFileName, float referenceDistance, float maxDistance, bool directionalSound = false, bool autoPlay = true, bool bLoop = false);
	virtual void* getSceneSoundSourceByID(int32 sourceid);
	virtual void* getSceneSoundSourceByName(const String& name);
	virtual void deleteSceneSoundSourceByName(const String& name);
	virtual void deleteSceneSoundSourceByID(int32 id);

	void deleteSceneSoundSource(OpenALSoundSource* pSoundSource);

	/**
	* Sets the global sound volume.
	*/ 
	virtual void setGlobalVolume(float globalVolume);

	virtual void Update(float fElapsedTime);
	virtual void UpdateInner();

	// Called from main thread to sync info
	virtual void syncSoundResource();
	
	

    bool recordSound(unsigned int rate, unsigned int sampleSize);
    Sound *stopRecording(bool generateFloatBuffer = false);

	void run();
	void initAL();
	void removeAll();
	inline void setWorkingDirection(const String & workingDir) { m_WorkingDir = workingDir; }

private:
	CriticalSection resourceArrayLock;

	SGPDevice *m_device;


	int interval;
	String m_WorkingDir;
	Array<OpenALSoundSource> m_AllSoundSource;
	OpenALSoundListener m_SoundListener;

	Array<MTSoundRecordStr> m_LoadingSounds;
	Array<MTSoundRecordStr> m_DeletingSounds;
	Array<CachedSoundFileRawDataStr> m_CachedSoundFileRawDataList;

	static const int RESOURCE_SOUND_TO_FREE_KEEPTIME = 3 * 60 * 1000;	// 3 mins
	static int32 StaticSoundSourceID;

protected:
	ALCdevice* device;
    ALCdevice* captureDevice;
    ALbyte *recordingBuffer;
    int recordingBufferSize;
    int recordingBufferRate;
	ALCcontext* context;
};