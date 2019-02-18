#pragma once
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

class CSound
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	CSound();
	CSound(const CSound&);
	~CSound();

	bool Initialize(HWND* hwnd, char* fileName, DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels);
	void Shutdown();
	bool PlayWaveFile(int flag);

private:
	bool InitializeDirectSound(HWND, DWORD, WORD, WORD);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**, DWORD, WORD, WORD);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

private:
	IDirectSound8 * mDirectSound;
	IDirectSoundBuffer* mPrimaryBuffer;
	IDirectSoundBuffer8* mSecondaryBuffer1;
};

