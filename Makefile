CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lSDL2

SRC = src/*.c
OUT = diggaudio

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT)

