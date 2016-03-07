#pragma once

#if defined(__APPLE__) && defined(__MACH__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include "AL/al.h"
    #include "AL/alc.h"
#endif

struct CachedSoundFileRawDataStr;

#define BUFFER_SIZE 32768

/**
* Loads and plays a sound. This class can load and play an OGG or WAV sound file.
*/
class Sound
{
public:

	static sgp::MemoryBlock* LoadSoundFileMT(const sgp::String& fileName, CachedSoundFileRawDataStr& SoundFileData);

	/**
	* Constructor.
	* @param fileName Path to an OGG or WAV file to load.
	*/ 
	Sound(const sgp::String& fileName, bool generateFloatBuffer = false);
	Sound(int size, const char *data, int channels = 1, ALsizei freq = 44100, int bps = 16, bool generateFloatBuffer = false);
	virtual ~Sound();
		
	void loadFile(sgp::String fileName, bool generateFloatBuffer);
		
	void reloadProperties();
		
	/**
	* Play the sound once or in a loop.
	* @param once If this is true, play it once, otherwise, loop.
	*/
	void Play(bool loop=false);
		
	/**
	* Stop the sound playback.
	*/		
	void Stop();
		
	/**
	* Sets the volume of this sound.
	* @param newVolume A float 0-1, where 0 is no sound and 1 is the loudest.
	*/
	void setVolume(float newVolume);

	float getVolume() { return volume; }

	/**
	* Sets the pitch of this sound.
	* @param newPitch A float 0-1.
	*/		
	void setPitch(float newPitch);
		
	float getPitch() { return pitch; }
		
	/**
	* Returns true if the sound is playing.
	* @return True if sound is playing, false if otherwise.
	*/
	bool isPlaying();
				
	void setIsPositional(bool isPositional);
		
	void setSoundPosition(sgp::Vector4D position);
	void setSoundVelocity(sgp::Vector4D velocity);
	void setSoundDirection(sgp::Vector4D direction);
		
	/**
	* Sets the current sample offset of this sound.
	* @param off A number 0 <= off < sound sample length
	*/
	void setOffset(int off);
		
	sgp::String getFileName() { return fileName; }
	void setFileName(sgp::String name) { fileName = name; }
		
	float getPlaybackDuration();
		
	float getPlaybackTime();
		
	void seekTo(float time);
	/**
	* Returns the current sample offset (playback progress) of this sound.
	* @return The sample offset if it is known, -1 otherwise.
	*/
	int getOffset();
		
	/**
	* Returns the number of samples in the sound.
	* @return The sample length if it is known, -1 otherwise.
	*/
	int getSampleLength() { return sampleLength; }
		
	void setPositionalProperties(float referenceDistance, float maxDistance);
		
	void setReferenceDistance(float referenceDistance);
	void setMaxDistance(float maxDistance);
	void setConeInnerAngle(float angle);
	void setConeOuterAngle(float angle);
	void setConeOuterGain(float gain);
		
	float getReferenceDistance() { return referenceDistance; }
	float getMaxDistance() { return maxDistance; }
		
	ALuint loadBytes(const char *data, int size, int channels = 1, ALsizei freq = 44100, int bps = 16, bool generateFloatBuffer = false);
	ALuint loadWAV(const sgp::String& fileName, bool generateFloatBuffer);
	ALuint loadOGG(const sgp::String& fileName, bool generateFloatBuffer);


		
	ALuint GenSource(ALuint buffer);
	ALuint GenSource();
	
	ALenum checkALError(const sgp::String& operation);
	void soundError(const sgp::String& err);
	void soundCheck(bool result, const sgp::String& err);
	static unsigned long readByte32(const unsigned char buffer[4]);		
	static unsigned short readByte16(const unsigned char buffer[2]);


		
	sgp::Array<float> *getFloatBuffer() { return &floatBuffer; }

protected:
	
	float referenceDistance;		// Default is 1.0
	float maxDistance;
			
	float pitch;					// Range:   [0.5-2.0] Default: 1.0
	float volume;

	float coneinnerangle;			// Default:  360
	float coneouterangle;			// Default:  360
	float coneoutergain;			// Default:  0.0
	
	sgp::String fileName;
		
	bool soundLoaded;
	
	bool isPositional;
	ALuint buffer; // Kept around only for deletion purposes
	ALuint soundSource;
	int sampleLength;
		
	sgp::Array<float> floatBuffer;
};