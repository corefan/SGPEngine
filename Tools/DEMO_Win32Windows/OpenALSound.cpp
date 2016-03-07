

#include "../../SGPLibraryCode/SGPHeader.h"
#include "OpenALSound.h"
#include "SoundManagerMT.h"
#include <vorbis/vorbisfile.h>

using namespace sgp;

Sound::Sound(const String& fileName, bool generateFloatBuffer) 
	: referenceDistance(1.0f), maxDistance(FLT_MAX), pitch(1.0f), volume(1.0f),
	coneinnerangle(360.0f), coneouterangle(360.0f), coneoutergain(0.0f),
	sampleLength(-1)
{
	checkALError("Construct: Loose error before construction");

	soundLoaded = false;	

	loadFile(fileName, generateFloatBuffer);	
	
	setIsPositional(false);
	
	checkALError("Construct from file: Finished");
}

Sound::Sound(int size, const char *data, int channels, int freq, int bps, bool generateFloatBuffer) 
	: referenceDistance(1.0f), maxDistance(FLT_MAX), pitch(1.0f), volume(1.0f),
	coneinnerangle(360.0f), coneouterangle(360.0f), coneoutergain(0.0f),
	buffer(AL_NONE), soundSource(AL_NONE), sampleLength(-1)
{
	checkALError("Construct: Loose error before construction");

	buffer = loadBytes(data, size, freq, channels, bps, generateFloatBuffer);
	
	soundSource = GenSource(buffer);
	
	setIsPositional(false);
	reloadProperties();
	
	soundLoaded = true;
	
	checkALError("Construct from data: Finished");
}

Sound::~Sound()
{
	alDeleteSources(1,&soundSource);
	checkALError("Destroying sound");
	alDeleteBuffers(1, &buffer);
	checkALError("Deleting buffer");
}

ALenum Sound::checkALError(const String& operation)
{
	ALenum error = alGetError();
	if(error != AL_NO_ERROR) 
	{
		switch(error) 
		{
			case AL_INVALID_NAME:
				soundError(operation + L":  AL error: a bad name (ID) was passed to an OpenAL function");
				break;
			case AL_INVALID_ENUM:
				soundError(operation + L":  AL error: an invalid enum value was passed to an OpenAL function");
				break;
			case AL_INVALID_VALUE:
				soundError(operation + L":  AL error: an invalid value was passed to an OpenAL function");
				break;
			case AL_INVALID_OPERATION:
				soundError(operation + L":  AL error: the requested operation is not valid");
				break;
			case AL_OUT_OF_MEMORY:
				soundError(operation + L":  AL error: the requested operation resulted in OpenAL running out of memory");
				break;
			default:
				soundError(operation + L":  AL error: unknown error");
				break;
		}		
	}
	return error;
}

void Sound::loadFile(String _fileName, bool generateFloatBuffer)
{
	if(soundLoaded) 
	{
		alDeleteSources(1,&soundSource);	
	}

	String actualFilename = _fileName;
	if (!File(actualFilename).existsAsFile())
	{
		actualFilename = "Sound/default.wav";		
	}

	
	String extension = File(actualFilename).getFileExtension();
	if(extension == ".wav" || extension == ".WAV")
	{
		buffer = loadWAV(actualFilename, generateFloatBuffer);			
	} 
	else if(extension == ".ogg" || extension == ".OGG") 
	{
		buffer = loadOGG(actualFilename, generateFloatBuffer);
	}
	
	this->fileName = actualFilename;
	
	soundSource = GenSource(buffer);	
	
	reloadProperties();
	
	soundLoaded = true;
	
	checkALError("Sound load: complete");
}

// Re-set stored properties into sound source.
void Sound::reloadProperties()
{ 
	setVolume(volume);
	setPitch(pitch);
	
	setReferenceDistance(referenceDistance);
	setMaxDistance(maxDistance);
}

void Sound::soundCheck(bool result, const String& err) 
{
	if(!result)
		soundError(err);
}

void Sound::soundError(const String& err)
{
	Logger::getCurrentLogger()->writeToLog(String("SOUND ERROR: ") + err, ELL_ERROR);
}

unsigned long Sound::readByte32(const unsigned char data[4])
{
#if SGP_BIG_ENDIAN
    return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
#else
    return (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
#endif
}

unsigned short Sound::readByte16(const unsigned char data[2]) 
{
#if SGP_BIG_ENDIAN
    return (data[0] << 8) + data[1];
#else
    return (data[1] << 8) + data[0];
#endif	
}

void Sound::Play(bool loop) 
{
	if(!loop)
	{
		alSourcei(soundSource, AL_LOOPING, AL_FALSE);
	}
	else
	{
		alSourcei(soundSource, AL_LOOPING, AL_TRUE);		
	}
	checkALError("Play: loop");
	alSourcePlay(soundSource);
	checkALError("Play: play");
}

bool Sound::isPlaying()
{
	ALenum state;
	alGetSourcei(soundSource, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

void Sound::Stop() 
{
	alSourceStop(soundSource);
	checkALError("Stop");
}

void Sound::setVolume(float newVolume) 
{
	this->volume = newVolume;
	alSourcef(soundSource, AL_GAIN, newVolume);
	checkALError("Set volume");
}

void Sound::setPitch(float newPitch) 
{
	this->pitch = newPitch;
	alSourcef(soundSource, AL_PITCH, newPitch);
	checkALError("Set pitch");
}

void Sound::setSoundPosition(Vector4D position) 
{
	if(isPositional)
		alSource3f(soundSource, AL_POSITION, position.x, position.y, position.z);
	checkALError("Set sound position");
}

void Sound::setSoundVelocity(Vector4D velocity)
{
	if(isPositional)
		alSource3f(soundSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	checkALError("Set sound velocity");
}

void Sound::setSoundDirection(Vector4D direction)
{
	if(isPositional)
		alSource3f(soundSource, AL_DIRECTION, direction.x, direction.y, direction.z);
	checkALError("Set sound direction");
}

void Sound::setOffset(int off) 
{
	alSourcei(soundSource, AL_SAMPLE_OFFSET, off);
}


float Sound::getPlaybackTime()
{
	float result = 0.0;
	alGetSourcef(soundSource, AL_SEC_OFFSET, &result);
	return result;
}

float Sound::getPlaybackDuration() 
{
	ALint sizeInBytes;
	ALint channels;
	ALint bits;
	ALint bufferID;
	alGetSourcei(soundSource, AL_BUFFER, &bufferID);
	
	alGetBufferi(bufferID, AL_SIZE, &sizeInBytes);
	alGetBufferi(bufferID, AL_CHANNELS, &channels);
	alGetBufferi(bufferID, AL_BITS, &bits);

	int lengthInSamples = sizeInBytes * 8 / (channels * bits);

	ALint frequency;
	alGetBufferi(bufferID, AL_FREQUENCY, &frequency);
	float durationInSeconds = (float)lengthInSamples / (float)frequency;
	
	return durationInSeconds;
}
		
int Sound::getOffset() 
{
	ALint off = -1;
	alGetSourcei(soundSource, AL_SAMPLE_OFFSET, &off);
	return off;
}

void Sound::seekTo(float time)
{
	if(time > getPlaybackDuration())
		return;
	alSourcef(soundSource, AL_SEC_OFFSET, time);
	checkALError("Seek");
}



void Sound::setPositionalProperties(float _referenceDistance, float _maxDistance)
{ 
	setReferenceDistance(_referenceDistance);
	setMaxDistance(_maxDistance);
}

void Sound::setReferenceDistance(float _referenceDistance)
{
	this->referenceDistance = _referenceDistance;
	alSourcef(soundSource, AL_REFERENCE_DISTANCE, _referenceDistance);
	checkALError("Set reference distance");
}

void Sound::setMaxDistance(float _maxDistance)
{
	this->maxDistance = _maxDistance;
	alSourcef(soundSource, AL_MAX_DISTANCE, _maxDistance);	
	checkALError("Set max distance");
}

void Sound::setConeInnerAngle(float angle)
{
	this->coneinnerangle = angle;
	alSourcef(soundSource, AL_CONE_INNER_ANGLE, angle);	
	checkALError("Set Cone Inner Angle");
}
void Sound::setConeOuterAngle(float angle)
{
	this->coneouterangle = angle;
	alSourcef(soundSource, AL_CONE_OUTER_ANGLE, angle);	
	checkALError("Set Cone Outer Angle");
}
void Sound::setConeOuterGain(float gain)
{
	this->coneoutergain = gain;
	alSourcef(soundSource, AL_CONE_OUTER_GAIN, gain);	
	checkALError("Set Cone outer gain");
}


void Sound::setIsPositional(bool _isPositional) 
{
	this->isPositional = _isPositional;
	if(_isPositional)
	{
		alSourcei(soundSource, AL_SOURCE_RELATIVE, AL_FALSE);
	} 
	else
	{
		alSourcei(soundSource, AL_SOURCE_RELATIVE, AL_TRUE);	
		alSource3f(soundSource,AL_POSITION, 0,0,0);
		alSource3f(soundSource,AL_VELOCITY, 0,0,0);
		alSource3f(soundSource,AL_DIRECTION, 0,0,0);				
	}
	checkALError("Set is-positional");
}

ALuint Sound::GenSource()
{
	ALuint source;
	bool looping = false;
	ALfloat sourcePos[] = {0.0, 0.0, 0.0};
	ALfloat sourceVel[] = {0.0, 0.0, 0.0};
	
	alGetError();
	
	alGenSources(1, &source);
	checkALError("Generating sources");
	
	alSourcef(source, AL_PITCH, 1.0);
	alSourcef(source, AL_GAIN, 1.0);
	alSourcefv(source, AL_POSITION, sourcePos);
	alSourcefv(source, AL_VELOCITY, sourceVel);
	alSourcei(source, AL_LOOPING, looping);

	checkALError("Setting source properties");
	
	return source;
}

ALuint Sound::GenSource(ALuint _buffer)
{
	alGetError();
	ALuint source = GenSource();
	alSourcei(source, AL_BUFFER, _buffer);	
	checkALError("Setting source buffer");
	return source;
}

ALuint Sound::loadBytes(const char *data, int size, int freq, int channels, int bps, bool generateFloatBuffer) 
{
	ALenum format;
	if (channels == 1)
		format = (bps == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
	else
		format = (bps == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
	
	sampleLength = bps > 8 ? size / (bps/8) : -1;

	checkALError("LoadBytes: pre-generate buffer");
	
	alGenBuffers(1, &buffer);
	checkALError("LoadBytes: generate buffer");
	soundCheck(AL_NONE != buffer, "LoadBytes: Did not generate buffer");
	
	alBufferData(buffer, format, data, size, freq);
	checkALError("LoadBytes: load buffer data");
    
	if(generateFloatBuffer)
	{
		int32_t *ptr32 = (int32_t*) &data[0];
		for(int i=0; i < size/4; i++ )
		{
			floatBuffer.add(((float)ptr32[i])/((float)INT32_MAX));
		}
	}
    
	return buffer;
}


ALuint Sound::loadWAV(const String& _fileName, bool generateFloatBuffer)
{
	Array <char> data;
	ALsizei freq;
	
	checkALError("loadWAV: pre-generate buffer");

	uint32 iFileSize = 0;
	uint8* ucpBuffer = NULL;
	uint8* pBufferHeader = NULL;

	{
		ScopedPointer<FileInputStream> fileStream( File(_fileName).createInputStream() );
		if( fileStream == nullptr )
		{
			soundError("LoadWav: Could not load wav from " + _fileName);
			return buffer;
		}

		iFileSize = (uint32)fileStream->getTotalLength();

		ucpBuffer = new uint8 [iFileSize];
		pBufferHeader = ucpBuffer;
		fileStream->read(ucpBuffer, iFileSize);
	}

	// buffers
	char magic[5];
	magic[4] = '\0';
	unsigned char data32[4];
	unsigned char data16[2];

	memcpy(magic, ucpBuffer, 4);
	ucpBuffer += 4;
	// check magic
	soundCheck(String(magic) == "RIFF", "LoadWav: Wrong wav file format. This file is not a .wav file (no RIFF magic): "+ _fileName );

	// skip 4 bytes (file size)
	ucpBuffer += 4;

	// check file format
	memcpy(magic, ucpBuffer, 4);
	ucpBuffer += 4;
	soundCheck(String(magic) == "WAVE", "LoadWav: Wrong wav file format. This file is not a .wav file (no WAVE format): "+ _fileName );

	// check 'fmt ' sub chunk (1)
	memcpy(magic, ucpBuffer, 4);
	ucpBuffer += 4;
	soundCheck(String(magic) == "fmt ", "LoadWav: Wrong wav file format. This file is not a .wav file (no 'fmt ' subchunk): "+ _fileName );

	// read (1)'s size
	memcpy(data32, ucpBuffer, 4);
	ucpBuffer += 4;
	unsigned long subChunk1Size = readByte32(data32);
	soundCheck(subChunk1Size >= 16, "Wrong wav file format. This file is not a .wav file ('fmt ' chunk too small, truncated file?): "+ _fileName );

	// check PCM audio format
	memcpy(data16, ucpBuffer, 2);
	ucpBuffer += 2;
	unsigned short audioFormat = readByte16(data16);
	soundCheck(audioFormat == 1, "LoadWav: Wrong wav file format. This file is not a .wav file (audio format is not PCM): "+ _fileName );

	// read number of channels
	memcpy(data16, ucpBuffer, 2);
	ucpBuffer += 2;
	unsigned short channels = readByte16(data16);
		
	// read frequency (sample rate)
	memcpy(data32, ucpBuffer, 4);
	ucpBuffer += 4;
	unsigned long frequency = readByte32(data32);

	// skip 6 bytes (Byte rate (4), Block align (2))
	ucpBuffer += 6;

	// read bits per sample
	memcpy(data16, ucpBuffer, 2);
	ucpBuffer += 2;
	unsigned short bps = readByte16(data16);
		
	// check 'data' sub chunk (2)
	memcpy(magic, ucpBuffer, 4);
	ucpBuffer += 4;
	soundCheck(String(magic) == "data", "LoadWav: Wrong wav file format. This file is not a .wav file (no data subchunk): "+ _fileName );
		
	memcpy(data32, ucpBuffer, 4);
	ucpBuffer += 4;
	unsigned long subChunk2Size = readByte32(data32);

	// The frequency of the sampling rate
	freq = frequency;
	soundCheck(sizeof(freq) == sizeof(frequency), "LoadWav: freq and frequency different sizes");
		

	if (ucpBuffer - pBufferHeader + subChunk2Size <= iFileSize)
	{
		data.addArray((const char*)ucpBuffer, subChunk2Size);
	}
	else
	{
		data.addArray((const char*)ucpBuffer, subChunk2Size - (ucpBuffer - pBufferHeader + subChunk2Size - iFileSize));
	}

	delete [] pBufferHeader;
	pBufferHeader = NULL;	
			
	return loadBytes(data.getRawDataPointer(), data.size(), freq, channels, bps, generateFloatBuffer);
}


size_t custom_readfunc(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	FileInputStream *pfilestream = (FileInputStream*) datasource;
	return pfilestream->read(ptr, size * nmemb) / size;
}

int custom_seekfunc(void *datasource, ogg_int64_t offset, int whence)
{
	FileInputStream *pfilestream = (FileInputStream*) datasource;
	ogg_int64_t curoffset = 0;
	ogg_int64_t fileLength = 0;
	switch(whence)
	{
		case SEEK_SET:
			return (pfilestream->setPosition(offset)) ? 0 : -1;
			break;
		case SEEK_CUR:
			curoffset = pfilestream->getPosition();					
			return (pfilestream->setPosition(curoffset + offset)) ? 0: -1;
			break;
		case SEEK_END:
			fileLength = pfilestream->getTotalLength();
			return (pfilestream->setPosition(fileLength - offset)) ? 0 : -1;
			break;
	}
	return -1;
}

int custom_closefunc(void *datasource) 
{
	FileInputStream *pfilestream = (FileInputStream*) datasource;
	if (pfilestream)
	{
		delete pfilestream;
		pfilestream = NULL;
		return 0;
	}
	else 
		return -1;
	
}

long custom_tellfunc(void *datasource) 
{
	FileInputStream *pfilestream = (FileInputStream*) datasource;

	return (long)pfilestream->getPosition();
}


ALuint Sound::loadOGG(const String& _fileName, bool generateFloatBuffer)
{
	Array<char> data;
	
	alGenBuffers(1, &buffer);
	int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
	int bitStream;
	long bytes;
	char array[BUFFER_SIZE];    // Local fixed size array
	ALenum format;
	ALsizei freq;
	


	// check file is valid
	FileInputStream *pFileStream = File(_fileName).createInputStream();
	if( pFileStream == NULL )
	{
		soundError("LoadOGG: Could not load OGG from " + _fileName);
		return buffer;
	}
	

	vorbis_info *pInfo;
	OggVorbis_File oggFile;	
	
	ov_callbacks callbacks;
	callbacks.read_func = custom_readfunc;
	callbacks.seek_func = custom_seekfunc;
	callbacks.close_func = custom_closefunc;
	callbacks.tell_func = custom_tellfunc;

	ov_open_callbacks( (void*)pFileStream, &oggFile, NULL, 0, callbacks);
	pInfo = ov_info(&oggFile, -1);

	// Check the number of channels... always use 16-bit samples
	if (pInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	// The frequency of the sampling rate
	freq = pInfo->rate;	
	do {
		// Read up to a buffer's worth of decoded sound data
		bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);
		// Append to end of buffer
		data.addArray(array, bytes);
	} while (bytes > 0);
	ov_clear(&oggFile);
	
	sampleLength = data.size() / sizeof(unsigned short);


	alBufferData(buffer, format, data.getRawDataPointer(), static_cast<ALsizei>(data.size()), freq);
	
	if(generateFloatBuffer)
	{
		int32_t *ptr32 = (int32_t*) data.getRawDataPointer();
		for(int i=0; i < data.size()/4; i++ )
		{
			floatBuffer.add(((float)ptr32[i])/((float)INT32_MAX));
		}	
	}	

	return buffer;
}

MemoryBlock* Sound::LoadSoundFileMT(const String& fileName, CachedSoundFileRawDataStr& SoundFileData)
{
	MemoryBlock* data = NULL;
	ALsizei freq;
	uint32 iFileSize = 0;
	uint8* ucpBuffer = NULL;
	uint8* pBufferHeader = NULL;

	int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
	int bitStream;
	long bytes;
	char array[BUFFER_SIZE];    // Local fixed size array
	ALenum format;

	String actualFilename = fileName;
	if (!File(actualFilename).existsAsFile())
	{
		return NULL;		
	}
	
	String extension = File(actualFilename).getFileExtension();
	if(extension == ".wav" || extension == ".WAV")
	{
		{
			ScopedPointer<FileInputStream> fileStream( File(actualFilename).createInputStream() );
			if( fileStream == nullptr )
			{				
				return NULL;
			}

			iFileSize = (uint32)fileStream->getTotalLength();

			ucpBuffer = new uint8 [iFileSize];
			pBufferHeader = ucpBuffer;
			fileStream->read(ucpBuffer, iFileSize);
		}

		data = new MemoryBlock();

		// buffers
		char magic[5];
		magic[4] = '\0';
		unsigned char data32[4];
		unsigned char data16[2];

		memcpy(magic, ucpBuffer, 4);
		ucpBuffer += 4;
		// check magic
		jassert(String(magic) == "RIFF");

		// skip 4 bytes (file size)
		ucpBuffer += 4;

		// check file format
		memcpy(magic, ucpBuffer, 4);
		ucpBuffer += 4;
		jassert(String(magic) == "WAVE");

		// check 'fmt ' sub chunk (1)
		memcpy(magic, ucpBuffer, 4);
		ucpBuffer += 4;
		jassert(String(magic) == "fmt ");

		// read (1)'s size
		memcpy(data32, ucpBuffer, 4);
		ucpBuffer += 4;
		unsigned long subChunk1Size = readByte32(data32);
		jassert(subChunk1Size >= 16);

		// check PCM audio format
		memcpy(data16, ucpBuffer, 2);
		ucpBuffer += 2;
		unsigned short audioFormat = readByte16(data16);
		jassert(audioFormat == 1);

		// read number of channels
		memcpy(data16, ucpBuffer, 2);
		ucpBuffer += 2;
		unsigned short channels = readByte16(data16);
		
		// read frequency (sample rate)
		memcpy(data32, ucpBuffer, 4);
		ucpBuffer += 4;
		unsigned long frequency = readByte32(data32);

		// skip 6 bytes (Byte rate (4), Block align (2))
		ucpBuffer += 6;

		// read bits per sample
		memcpy(data16, ucpBuffer, 2);
		ucpBuffer += 2;
		unsigned short bps = readByte16(data16);
		
		// check 'data' sub chunk (2)
		memcpy(magic, ucpBuffer, 4);
		ucpBuffer += 4;
		jassert(String(magic) == "data");
		
		memcpy(data32, ucpBuffer, 4);
		ucpBuffer += 4;
		unsigned long subChunk2Size = readByte32(data32);

		// The frequency of the sampling rate
		freq = frequency;
		jassert(sizeof(freq) == sizeof(frequency));
		

		if (ucpBuffer - pBufferHeader + subChunk2Size <= iFileSize)
		{
			data->append((const char*)ucpBuffer, subChunk2Size);
		}
		else
		{
			data->append((const char*)ucpBuffer, subChunk2Size - (ucpBuffer - pBufferHeader + subChunk2Size - iFileSize));
		}

		delete [] pBufferHeader;
		pBufferHeader = NULL;

		SoundFileData.bps = bps;
		SoundFileData.channels = channels;
		SoundFileData.freq = freq;
		SoundFileData.size = data->getSize();
	} 
	else if(extension == ".ogg" || extension == ".OGG") 
	{
		FileInputStream *pFileStream = File(fileName).createInputStream();
		if( pFileStream == NULL )
		{
			return NULL;
		}

		data = new MemoryBlock();

		vorbis_info *pInfo;
		OggVorbis_File oggFile;	
	
		ov_callbacks callbacks;
		callbacks.read_func = custom_readfunc;
		callbacks.seek_func = custom_seekfunc;
		callbacks.close_func = custom_closefunc;
		callbacks.tell_func = custom_tellfunc;

		ov_open_callbacks( (void*)pFileStream, &oggFile, NULL, 0, callbacks);
		pInfo = ov_info(&oggFile, -1);

		// Check the number of channels... always use 16-bit samples
		int channels = pInfo->channels;
		if (pInfo->channels == 1)
			format = AL_FORMAT_MONO16;
		else
			format = AL_FORMAT_STEREO16;

		// The frequency of the sampling rate
		freq = pInfo->rate;	
		do {
			// Read up to a buffer's worth of decoded sound data
			bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);
			// Append to end of buffer
			data->append(array, bytes);
		} while (bytes > 0);
		ov_clear(&oggFile);



		SoundFileData.bps = 16;				// always use 16-bit samples
		SoundFileData.channels = channels;
		SoundFileData.freq = freq;
		SoundFileData.size = data->getSize();

	}

	
	return data;
}
