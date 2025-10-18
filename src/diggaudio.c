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

int LoadWAV(const char *filename, struct DiggaWAV *header)
{
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		return FAILURE;
	}

	fread(header, 36, 1, file);

	char chunkID[4];
	int chunkSize;
	
	while (1) {
		if (fread(chunkID, 4, 1, file) != 1)
		{
			break;
		}
		if (fread(&chunkSize, 4, 1, file) != 1)
		{
			break;
		}
		
		if (chunkID[0] == 'd' && chunkID[1] == 'a' && chunkID[2] == 't' && chunkID[3] == 'a')
		{
			memcpy(header->Subchunk2ID, chunkID, 4);
			header->Subchunk2Size = chunkSize;
			fclose(file);
			return SUCCESS;
		}
		
		fseek(file, chunkSize, SEEK_CUR);
	}
	
	fclose(file);
	return FAILURE;
}

int GetPCM(short *PCM, struct DiggaWAV header, const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		return FAILURE;
	}

	fread(&header, 36, 1, file);

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
		
		if (chunkID[0] == 'd' && chunkID[1] == 'a' && chunkID[2] == 't' && chunkID[3] == 'a')
		{
			fread(PCM, chunkSize, 1, file);
			fclose(file);
			return SUCCESS;
		}
		
		fseek(file, chunkSize, SEEK_CUR);
	}
	
	fclose(file);
	return FAILURE;
}
