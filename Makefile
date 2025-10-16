CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lSDL2

SRC = src/main.c src/diggaudio.c src/sdl_audio.c
OUT = diggaudio

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT)

