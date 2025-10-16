#include "diggaudio.h"
#include "sdl_audio.h"

#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("No sound file provided\n");
		return FAILURE;
	}

	struct DiggaWAV header;

	if (LoadWAV(argv[1], &header) != SUCCESS) {
		printf("Failed to load file into the WAV struct: %s\n", argv[1]);
		return FAILURE;
	}

	short* PCM = malloc(header.Subchunk2Size);
	if (!PCM || GetPCM(PCM, header, argv[1]) != SUCCESS) {
		printf("Failed to load PCM data.\n");
		free(PCM);
		return FAILURE;
	}

	PrintWAV(header);

	if (InitSDLAudio() != SUCCESS) {
		free(PCM);
		return FAILURE;
	}

	SDL_AudioSpec have;
	SDL_AudioDeviceID dev = AudioSetup(&header, &have);
	if (dev == 0) {
		free(PCM);
		SDL_Quit();
		return FAILURE;
	}

	if (PlayAudio(dev, PCM, header.Subchunk2Size, &have) != SUCCESS) {
		Cleanup(PCM, dev);
		return FAILURE;
	}

	printf("Error: %s\n", SDL_GetError());

	Cleanup(PCM, dev);
	return SUCCESS;
}

