#include "diggaudio.h"
#include "sdl_audio.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800

#define WAVE_WIDTH 0.9

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
	
	double xScale = targetWidth / timed.samples;
	double yScale = SCREEN_HEIGHT / 4.0;
	double centerY = SCREEN_HEIGHT / 2.0;
	
	for (int i = 1; i < timed.samples; i++)
	{
		int x1 = margin + (i-1) * xScale;
		int x2 = margin + i * xScale;
	
		int y1 = centerY - (timed.PCMval[i-1] / 32768.0) * yScale;
		int y2 = centerY - (timed.PCMval[i] / 32768.0) * yScale;
	
		SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
	}
	
	SDL_SetRenderTarget(renderer, NULL);
	
	bool isPlaying = false;
	double currentPlayTime = 0.0;
	double totalDuration = timed.samples / (double)header.SampleRate;
	Uint32 lastUpdateTime = 0;
	
	SDL_Event e;
	bool quit = false;
	
	// Main Loop
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
	//							PrintTimedPCM(timed);
								
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
			Uint32 currentTime = SDL_GetTicks();
			Uint32 deltaTime = currentTime - lastUpdateTime;
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
