#include "diggaudio.h"
#include "sdl_audio.h"

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800

#define WAVE_WIDTH 0.9

double get_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1000000.0);
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("No sound file provided\n");
		return FAILURE;
	}

	struct DiggaWAV header;
	
	FILE *file = fopen(argv[1], "rb");

	if (LoadWAV(file, &header) != SUCCESS)
	{
		printf("Failed to load file into the WAV struct: %s\n", argv[1]);
		return FAILURE;
	}

	short* PCM = malloc(header.Subchunk2Size);
	if (!PCM || GetPCM(file, PCM, header) != SUCCESS)
	{
		printf("Failed to load PCM data.\n");
		free(PCM);
		return FAILURE;
	}

	fclose(file);

	PrintWAV(header);

	if (InitSDL() != SUCCESS)
	{
		free(PCM);
		return FAILURE;
	}

	SDL_AudioSpec have;
	SDL_AudioDeviceID dev = AudioSetup(&header, &have);
	if (dev == 0)
	{
		free(PCM);
		SDL_Quit();
		return FAILURE;
	}
  
	struct TimedPCM timed;
	
	timed.PCMval = PCM;
	timed.time = malloc(sizeof(double)*header.Subchunk2Size/(header.BitsPerSample >> 3));
	for (int i = 0; i < header.Subchunk2Size/(header.BitsPerSample >> 3); i++)
	{
		timed.time[i] = i * (double)(1/(double)header.SampleRate);
	}
	timed.samples = header.Subchunk2Size/(header.BitsPerSample >> 3);
	
	SDL_Window* window = SDL_CreateWindow("Diggaudio", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (window == NULL)
	{
		printf("SLD_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return FAILURE;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		printf("SLD_CreateCreated Error: %s\n", SDL_GetError());
		SDL_Quit();
		return FAILURE;
	}
	
	SDL_Texture* waveTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (waveTexture == NULL)
	{
		printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
		SDL_Quit();
		return FAILURE;
	}
	
	SDL_SetRenderTarget(renderer, waveTexture);
	SDL_SetRenderDrawColor(renderer, 57, 255, 20, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 105, 180, 255);

	double targetWidth = SCREEN_WIDTH * WAVE_WIDTH;
	double margin = (SCREEN_WIDTH - targetWidth) / 2.0;
	
	double yScale = SCREEN_HEIGHT / 4.0;
	double centerY = SCREEN_HEIGHT / 2.0;
	
	double start_time = get_time_ms();
	
	int samplesPerSegment = timed.samples / (SCREEN_WIDTH * WAVE_WIDTH);
	if (samplesPerSegment < 1) samplesPerSegment = 1;
	
	for (int segment = 0; segment < SCREEN_WIDTH * WAVE_WIDTH; segment++)
	{
		int startSample = segment * samplesPerSegment;
		int endSample = startSample + samplesPerSegment;
		
		if (endSample >= timed.samples)
		{
			endSample = timed.samples - 1;
		}
		
		short minVal = timed.PCMval[startSample];
		short maxVal = timed.PCMval[startSample];
		
		for (int j = startSample + 1; j < endSample; j++)
		{
			if (timed.PCMval[j] < minVal)
			{
				minVal = timed.PCMval[j];
			}

			if (timed.PCMval[j] > maxVal)
			{
				maxVal = timed.PCMval[j];
			}
		}
		
		int x = margin + segment;
		int y1 = centerY - (minVal / 32768.0) * yScale;
		int y2 = centerY - (maxVal / 32768.0) * yScale;
		
		SDL_RenderDrawLine(renderer, x, y1, x, y2);
	}
	
	double end_time = get_time_ms();
	double generation_time = end_time - start_time;
	
	printf("Wave visualization time: %.3f milliseconds\n", generation_time);
	
	SDL_SetRenderTarget(renderer, NULL);
	
	bool isPlaying = false;
	double currentPlayTime = 0.0;
	double totalDuration = timed.samples / (double)header.SampleRate;
	Uint32 lastUpdateTime = 0;
	
	SDL_Event e;
	bool quit = false;
	
	while(!quit)
	{
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT:
					quit = true;
					break;
				
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
						case SDLK_SPACE:
							if (!isPlaying)
							{
								struct AudioArgs* args = malloc(sizeof(struct AudioArgs));
								
								args->dev = dev;
								args->PCM = timed.PCMval;
								args->dataSize = header.Subchunk2Size;
								args->have = &have;
								
								SDL_Thread* audioThread = SDL_CreateThread(AudioThread, "AudioThread", args);
								SDL_DetachThread(audioThread);
								
								isPlaying = true;
								currentPlayTime = 0.0;
								lastUpdateTime = SDL_GetTicks();
							}
							break;
						
						case SDLK_q:
							quit = true;
							break;
							
						case SDLK_r:
							currentPlayTime = 0.0;
							break;
					}
			}
		}
		
		if (isPlaying)
		{
			int currentTime = SDL_GetTicks();
			int deltaTime = currentTime - lastUpdateTime;
			lastUpdateTime = currentTime;
			
			currentPlayTime += deltaTime / 1000.0;
			
			if (currentPlayTime >= totalDuration)
			{
				currentPlayTime = 0.0;
				isPlaying = false;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 57, 255, 20, 255);
		SDL_RenderClear(renderer);
		
		SDL_RenderCopy(renderer, waveTexture, NULL, NULL);
		
		if (isPlaying)
		{
			double progress = currentPlayTime / totalDuration;
			int trackerX = margin + (targetWidth * progress);

			int lineWidth = SCREEN_WIDTH * 0.003;
			int startX = trackerX - (lineWidth / 2);

			SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
			SDL_Rect trackerRect = {startX, 0, lineWidth, SCREEN_HEIGHT};
			SDL_RenderFillRect(renderer, &trackerRect);
		}	

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(waveTexture);
	SDL_DestroyWindow(window);
	SDL_CloseAudioDevice(dev);
	SDL_Quit();
	free(timed.PCMval);
	free(timed.time);
	return SUCCESS;
}
