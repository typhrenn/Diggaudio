CFLAGS = -Iinclude
LDFLAGS = -lSDL2

.PHONY: clean

diggaudio: src/main.c
	gcc $(CFLAGS) -o diggaudio src/main.c $(LDFLAGS)

clean:
	rm diggaudio
