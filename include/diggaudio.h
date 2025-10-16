#ifndef DIGGAUDIO_H
#define DIGGAUDIO_H

#include <stdio.h>

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

void PrintWAV(struct DiggaWAV header);
int LoadWAV(const char *filename, struct DiggaWAV *header);
int GetPCM(short *PCM, struct DiggaWAV header, const char *filename);

#endif
