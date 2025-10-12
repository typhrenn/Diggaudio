#include <stdio.h>
#include <stdlib.h>

struct __attribute__((__packed__)) DiggaWAV
{
	char ChunkID[4]; //RIFF
	int ChunkSize;
	char Format[4]; //WAVE
	
	char Subchunk1ID[4];  //"fmt "
	int Subchunk1Size;
	short AudioFormat; //1
	short NumChannels; //1
	int SampleRate; //44100
	int ByteRate; // SampleRate * NumChannels * BitsPerSample/8
	short BlockAlign;
	short BitsPerSample;

	char Subchunk2ID[4]; //data
	int Subchunk2Size;
};

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


int main()
{
	FILE* digga = fopen("meow.wav", "rb");
	struct DiggaWAV header;

	fread(&header, sizeof(struct DiggaWAV), 1, digga);
	fclose(digga);

	PrintWAV(header);

	return 0;
}
