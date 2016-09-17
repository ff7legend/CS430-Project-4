all: project1.c
	gcc project1.c -o ppmrw

clean:
	rm -rf project1 *~
