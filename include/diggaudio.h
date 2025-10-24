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

short FindDataChunk(FILE *file);
void PrintWAV(struct DiggaWAV header);
int LoadWAV(FILE *file, struct DiggaWAV *header);
int GetPCM(FILE *file, short *PCM, struct DiggaWAV header);

#endif
