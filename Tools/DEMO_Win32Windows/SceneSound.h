#pragma once

class Sound;

/**
* Creates a positional 3D sound listener. There can be only one listener active at any one time.
*/
class SceneSoundListener : public sgp::CMovementController
{
public:
	SceneSoundListener(SGPDevice *pdevice);
	virtual ~SceneSoundListener();			
	virtual void Update(float fElapsedTime);

	void SetPos(Vector4D pos) { m_vcPos = pos; }
	void SetDir(Vector4D dir) { m_vcDir = dir; }
	void SetUp(Vector4D up) { m_vcUp = up; }
private:
	SGPDevice *m_device;
};

/**
* Creates a positional 3D sound.
*/	
class SceneSound : sgp::CMovementController
{
	public:
		SceneSound(const sgp::String& fileName, float referenceDistance, float maxDistance, bool directionalSound = false);
		SceneSound(const sgp::String& fileName, int size, const char *data, int channels, int freq, int bps,
			float referenceDistance, float maxDistance, bool directionalSound = false);
		virtual ~SceneSound();			
		virtual void Update(float fElapsedTime);
        
			
        bool isDirectionalSound() const;
        void setDirectionalSound(bool val);

		void SetPos(Vector4D pos) { m_vcPos = pos; }
		void SetDir(Vector4D dir) { m_vcDir = dir; }
        
		/**
		* Returns the sound object associated with this positional sound.
		*/
		Sound *getSound();
			
	protected:
		
		bool directionalSound;
		Sound *sound;
};


struct OpenALSoundSource
{
	String SoundSourceName;
	String SoundSourceFileName;
	SceneSound *pSoundSource;
	int32 SoundSourceID;

	OpenALSoundSource() : pSoundSource(NULL), SoundSourceID(-1)
	{}
	~OpenALSoundSource()
	{}

	bool operator== (const OpenALSoundSource& other) const noexcept
	{
		return SoundSourceName.equalsIgnoreCase( other.SoundSourceName );
	}
};


struct OpenALSoundListener
{
	String SoundListenerName;
	SceneSoundListener *pSoundListener;


	OpenALSoundListener() : pSoundListener(NULL)
	{}
	~OpenALSoundListener()
	{}

};