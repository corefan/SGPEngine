#pragma once



#if defined(__APPLE__) && defined(__MACH__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include "AL/al.h"
    #include "AL/alc.h"
#endif

class Sound;
struct OpenALSoundSource;
struct OpenALSoundListener;

/**
* Controls global sound settings.
*/
class SoundManager : public ISGPSoundManager
{
public:
	SoundManager(SGPDevice *device);
	virtual ~SoundManager();	

	// Sound Linstener Interface
	virtual void addListener(const sgp::String& name);
	virtual sgp::CMovementController* getListener();
	virtual void setListenerPosition(sgp::Vector4D position);
	virtual void setListenerOrientation(sgp::Vector4D orientation, sgp::Vector4D upVector);

	// Scene Sound Source Interface
	virtual void addSceneSoundSource(const sgp::String& name, const sgp::String& soundFileName, float referenceDistance, float maxDistance, bool directionalSound = false, bool autoPlay = true, bool bLoop = false);
 
	virtual void* getSceneSoundSourceByID(int32 sourceid);
	virtual void* getSceneSoundSourceByName(const String& name);
	virtual void deleteSceneSoundSourceByName(const String& name);
	virtual void deleteSceneSoundSourceByID(int32 id);

	/**
	* Sets the global sound volume.
	*/ 
	virtual void setGlobalVolume(float globalVolume);

	virtual void Update(float fElapsedTime);
	virtual void UpdateInner();

	// Called from main thread to sync info (single thread not used)
	virtual void syncSoundResource() { return; }


    bool recordSound(unsigned int rate, unsigned int sampleSize);
    Sound *stopRecording(bool generateFloatBuffer = false);

	inline void setWorkingDirection(const sgp::String & workingDir) { m_WorkingDir = workingDir; }
	void initAL();		

private:
	sgp::SGPDevice *m_device;
	sgp::String m_WorkingDir;

	Array<OpenALSoundSource> m_AllSoundSource;
	OpenALSoundListener m_SoundListener;

	static int32 StaticSoundSourceID;

protected:
	ALCdevice* device;
    ALCdevice* captureDevice;
    ALbyte *recordingBuffer;
    int recordingBufferSize;
    int recordingBufferRate;
	ALCcontext* context;


};