#include "diggaudio.h"
#include <stdlib.h>

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
        printf("Failed to open file: %s\n", filename);
        return FAILURE;
    }

    fread(header, sizeof(struct DiggaWAV), 1, file);

    fclose(file);

    return SUCCESS;
}

int GetPCM(short *PCM, struct DiggaWAV header, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("Failed to open file: %s\n", filename);
        return FAILURE;
    }

    fread(PCM, header.Subchunk2Size, 1, file);

    fclose(file);

    return SUCCESS;
}
