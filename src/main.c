#include "diggaudio.h"
#include "sdl_audio.h"

#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("No sound file provided\n");
		return FAILURE;
	}

	struct DiggaWAV header;
	
	// LOAD WAV AND PCM
	FILE *file = fopen(argv[1], "rb");

	if (LoadWAV(file, &header) != SUCCESS) {
		printf("Failed to load file into the WAV struct: %s\n", argv[1]);
		return FAILURE;
	}

	short* PCM = malloc(header.Subchunk2Size);
	if (!PCM || GetPCM(file, PCM, header) != SUCCESS) {
		printf("Failed to load PCM data.\n");
		free(PCM);
		return FAILURE;
	}

	fclose(file);

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

	Cleanup(PCM, dev);
	return SUCCESS;
}

