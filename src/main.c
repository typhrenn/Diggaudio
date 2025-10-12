#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

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

int main(int argc, char **argv)
{
	FILE* digga = fopen(argv[1], "rb");
	struct DiggaWAV header;

	fread(&header, sizeof(struct DiggaWAV), 1, digga);

	PrintWAV(header);

	short* PCM = malloc(header.Subchunk2Size);
	fread(PCM, header.Subchunk2Size, 1, digga);

	fclose(digga);	

	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
        	printf("SDL_Init Error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_AudioSpec want, have;
	SDL_zero(want);
	want.freq = header.SampleRate;
	want.format = AUDIO_S16LSB;
	want.channels = header.NumChannels;
	want.samples = 4096;
	want.callback = NULL;

	SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0)
	{
        printf("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
       	SDL_Quit();
        return -1;
    }

	size_t byteCount = header.Subchunk2Size;

	if (SDL_QueueAudio(dev, PCM, byteCount) < 0)
	{
		printf("SDL_QueueAudio failed: %s\n", SDL_GetError());
		SDL_CloseAudioDevice(dev);
		SDL_Quit();
		return -1;
	}

	SDL_PauseAudioDevice(dev, 0);

	unsigned int duration_ms = (byteCount * 1000) / (have.freq * have.channels * sizeof(short));
	SDL_Delay(duration_ms + 100);
	
	free(PCM);
	SDL_CloseAudioDevice(dev);
	SDL_Quit();

	return 0;
}
