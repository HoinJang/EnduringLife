#include "Sound.h"

CSound::CSound()
{
	mDirectSound = 0;
	mPrimaryBuffer = 0;
	mSecondaryBuffer1 = 0;
}

CSound::CSound(const CSound& other)
{

}


CSound::~CSound()
{

}

bool CSound::Initialize(HWND* hwnd, char* fileName, DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels)
{
	bool result;

	result = InitializeDirectSound(*hwnd, nSamplesPerSec, wBitsPerSample, nChannels);
	if (!result)
		return false;

	result = LoadWaveFile(fileName, &mSecondaryBuffer1, nSamplesPerSec, wBitsPerSample, nChannels);
	if (!result)
		return false;

	return true;
}

void CSound::Shutdown()
{
	ShutdownWaveFile(&mSecondaryBuffer1);
	ShutdownDirectSound();
}

bool CSound::InitializeDirectSound(HWND hwnd, DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	result = DirectSoundCreate8(NULL, &mDirectSound, NULL);
	if (FAILED(result))
		return false;

	result = mDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
		return false;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = mDirectSound->CreateSoundBuffer(&bufferDesc, &mPrimaryBuffer, NULL);
	if (FAILED(result))
		return false;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = nSamplesPerSec;
	waveFormat.wBitsPerSample = wBitsPerSample;
	waveFormat.nChannels = nChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8 * waveFormat.nChannels);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec*waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	result = mPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
		return false;

	return true;
}

void CSound::ShutdownDirectSound()
{
	if (mPrimaryBuffer)
	{
		mPrimaryBuffer->Release();
		mPrimaryBuffer = 0;
	}

	if (mDirectSound)
	{
		mDirectSound->Release();
		mDirectSound = 0;
	}

	return;
}

bool CSound::LoadWaveFile(char* fileName, IDirectSoundBuffer8** secondaryBuffer, DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	error = fopen_s(&filePtr, fileName, "rb");
	if (error != 0)
		return false;

	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
		return false;

	if ((waveFileHeader.chunkId[0] != 'R') ||
		(waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') ||
		(waveFileHeader.chunkId[3] != 'F'))
		return false;

	if ((waveFileHeader.format[0] != 'W') ||
		(waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') ||
		(waveFileHeader.format[3] != 'E'))
		return false;

	if ((waveFileHeader.subChunkId[0] != 'f') ||
		(waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') ||
		(waveFileHeader.subChunkId[3] != ' '))
		return false;

	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		return false;

	if (waveFileHeader.numChannels != nChannels)
		return false;

	if (waveFileHeader.sampleRate != nSamplesPerSec)
		return false;

	if (waveFileHeader.bitsPerSample != wBitsPerSample)
		return false;

	if ((waveFileHeader.dataChunkId[0] != 'd') ||
		(waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') ||
		(waveFileHeader.dataChunkId[3] != 'a'))
		return false;

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = nSamplesPerSec;
	waveFormat.wBitsPerSample = wBitsPerSample;
	waveFormat.nChannels = nChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = mDirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
		return false;

	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
		return false;

	tempBuffer->Release();
	tempBuffer = 0;

	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
		return false;

	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
		return false;

	error = fclose(filePtr);
	if (error != 0)
		return false;

	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
		return false;

	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
		return false;

	delete[] waveData;
	waveData = 0;

	return true;
}

void CSound::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}
	return;
}

bool CSound::PlayWaveFile(int flag)
{
	HRESULT result;

	result = mSecondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result))
		return false;

	result = mSecondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
		return false;

	//한번만
	if (flag == 0)				result = mSecondaryBuffer1->Play(0, 0, 0);
	//반복
	else if (flag == 1)		result = mSecondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING);
	
	if (FAILED(result))
		return false;

	return true;
}

