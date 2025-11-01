#include "diggaudio.h"
#include <stdlib.h>
#include <string.h>

void PrintWAV(struct DiggaWAV header)
{
	printf("ChunkID: %.4s\n", header.ChunkID);
	printf("ChunkSize: %d\n", header.ChunkSize);
	printf("Format: %.4s\n", header.Format);

	printf("Subchunk1ID: %.4s\n", header.Subchunk1ID);
	printf("Subchunk1Size: %d\n", header.Subchunk1Size);
	printf("AudioFormat: %d\n", header.AudioFormat);
	printf("NumChannels: %d\n", header.NumChannels);
	printf("SampleRate: %d\n", header.SampleRate);
	printf("ByteRate: %d\n", header.ByteRate);
	printf("BlockAlign: %d\n", header.BlockAlign);
	printf("BitsPerSample: %d\n", header.BitsPerSample);

	printf("Subchunk2ID: %.4s\n", header.Subchunk2ID);
	printf("Subchunk2Size %d\n", header.Subchunk2Size);
}

void PrintTimedPCM(struct TimedPCM timed)
{
    for (int i = 0; i < timed.samples; i++)
    {
        printf("PCM: %d Time: %lf\n", timed.PCMval[i], timed.time[i]);
    }
}

int LoadWAV(FILE *file, struct DiggaWAV *header)
{
	fread(header, 36, 1, file);

	char chunkID[4];
	int chunkSize;
	
	while (1) 
	{
		if (fread(chunkID, 4, 1, file) != 1)
		{
			break;
		}
		if (fread(&chunkSize, 4, 1, file) != 1)
		{
			break;
		}
		
		if (strcmp(chunkID, "data") == 0)
		{
			memcpy(header->Subchunk2ID, chunkID, 4);
			header->Subchunk2Size = chunkSize;
			return SUCCESS;
		}
		
		fseek(file, chunkSize, SEEK_CUR);
	}
	
	return FAILURE;
}

int GetPCM(FILE *file, short *PCM, struct DiggaWAV header)
{
	fread(PCM, header.Subchunk2Size, 1, file);

	return SUCCESS;
}
