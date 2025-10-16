#include "sdl_audio.h"
#include <stdlib.h>

int InitSDLAudio(void) {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) 
	{
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return FAILURE;
	}
	return SUCCESS;
}

SDL_AudioDeviceID AudioSetup(const struct DiggaWAV* header, SDL_AudioSpec* have) 
{
	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = header->SampleRate;
	want.format = AUDIO_S16LSB;
	want.channels = header->NumChannels;
	want.samples = 4096;
	want.callback = NULL;

	SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, have, 0);
	if (dev == 0)
	{
		printf("SDL_OpenAudioDevice failed: %s\n", SDL_GetError());
	}
	return dev;
}

int PlayAudio(SDL_AudioDeviceID dev, const short* PCM, size_t data_size, const SDL_AudioSpec* have)
{
	if (SDL_QueueAudio(dev, PCM, data_size) < 0) 
	{
		printf("SDL_QueueAudio failed: %s\n", SDL_GetError());
		return FAILURE;
	}
	SDL_PauseAudioDevice(dev, 0);

	unsigned int duration_ms = (data_size * 1000) / (have->freq * have->channels * sizeof(short));
	SDL_Delay(duration_ms + 100);

	return SUCCESS;
}

void Cleanup(short* PCM, SDL_AudioDeviceID dev) 
{
	if (PCM) 
	{
		free(PCM);
	}
	if (dev) 
	{
		SDL_CloseAudioDevice(dev);
	}
	SDL_Quit();
}
