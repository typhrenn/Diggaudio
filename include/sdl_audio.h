#ifndef SDL_AUDIO_H
#define SDL_AUDIO_H

#include <SDL2/SDL.h>
#include "diggaudio.h"

int InitSDL(void);
SDL_AudioDeviceID AudioSetup(const struct DiggaWAV* header, SDL_AudioSpec* have);
int PlayAudio(SDL_AudioDeviceID dev, const short* PCM, size_t data_size, const SDL_AudioSpec* have);
int AudioThread(void* data);

#endif
