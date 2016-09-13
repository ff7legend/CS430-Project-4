#include <stdio.h>
#include <stdlib.h>

int main(void) {
	FILE *fh = fopen("cam.ppm", "r");
   	char c = fgetc(fh);
      	if (c != 'P') {
          printf("This is not a ppm file\n");
	} 
	fclose(fh);
	return 0;
}
