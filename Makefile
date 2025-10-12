.PHONY: clean

diggaudio: $(wildcard src/*.c)
	gcc -Iinclude -o $@ $^
	
clean:
	rm diggaudio
