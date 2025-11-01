#ifndef DIGGAUDIO_H
#define DIGGAUDIO_H

#include <stdio.h>
#include <SDL2/SDL.h>

#define SUCCESS 0
#define FAILURE -1

struct __attribute__((__packed__)) DiggaWAV
{
	char ChunkID[4];
	int ChunkSize;
	char Format[4];

	char Subchunk1ID[4];
	int Subchunk1Size;
	short AudioFormat;
	short NumChannels;
	int SampleRate;
	int ByteRate;
	short BlockAlign;
	short BitsPerSample;

	char Subchunk2ID[4];
	int Subchunk2Size;
};

struct __attribute__((__packed__)) AudioArgs
{
    SDL_AudioDeviceID dev;
    const short* PCM;
    size_t dataSize;
    const SDL_AudioSpec* have;
};

struct TimedPCM
{
    short* PCMval;
    double* time;
    int samples;
};

void PrintTimedPCM(struct TimedPCM timed);
short FindDataChunk(FILE *file);
void PrintWAV(struct DiggaWAV header);
int LoadWAV(FILE *file, struct DiggaWAV *header);
int GetPCM(FILE *file, short *PCM, struct DiggaWAV header);

#endif
