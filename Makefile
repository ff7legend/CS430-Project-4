PROG=raycast
INPUT=main.c json.c raycast.c ppmrw.c
CFLAGS=-O3 -lm -g -Wall

all:
	if [ ! -e bin ]; then mkdir bin; fi
	gcc $(CFLAGS) $(INPUT) -o bin/$(PROG)

clean:
	rm -rf bin

clean-all: clean
	rm -rf bin
