#include <stdio.h>
#include <stdlib.h>
typedef struct { 
	unsigned char red;
	unsigned char green;
	unsigned char blue;	
	
} Pixel;

typedef struct {
	int x, y;
	Pixel *data;
} Image;

Pixel *buffer;

int main( int argc, char *argv[] );
static Image *readP6(char *filename)
{
	char buff[16];
	Image *image;
	FILE *fp;
	int c, rgb_color;
	
	fp=fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Cannot open file '%s'\n", filename);
		exit(1);
	}

	if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }

	//Make sure image is in PPM format
	if (buff[0] != 'P' || buff[1] != '6'){ 
		fprintf(stderr, "Image is not P3 or P6 format\n");
		exit(1);
	 }

	//malloc memory for the image
	image = (Image *)malloc(sizeof(Image));
	if (!image) {
		fprintf(stderr, "unable to allocate memory\n");
		exit(1);
	}
	
	//have to account for comments
	c = getc(fp);
    	while (c == '#') {
    	while (getc(fp) != '\n') ;
      		c = getc(fp);
    	}
	ungetc(c, fp);

	//set x and y values for the size of the image
	if (fscanf(fp, "%d %d", &image->x,&image->y) != 2){
	//if more than 2 values for size, throw error
		fprintf(stderr, "wrong image size for '%s'\n", filename);
		exit(1);
	}

	//check if the file has correct color component
	if (fscanf(fp, "%d", &rgb_color) != 1) {
        	fprintf(stderr, "inccorect RGB values in '%s')\n", filename);
		exit(1);
      	}	

	//make sure color has correct depth
	if (rgb_color != 255) {
		fprintf(stderr, "'%s' is not 8bit\n", filename);
		exit(1);
	}

	while (fgetc(fp) != '\n') ;

   	image->data = (Pixel*)malloc(image->x * image->y * sizeof(Pixel));

	if (!image){ fprintf (stderr, "cannot allocate memory\n");}

	if (fread(image->data, 3 * image->x, image->y, fp) != image->y){
		fprintf(stderr, "can't load image: '%s'", filename);
		exit(1);
	}

	fclose(fp);
	return image;
}

void writeP6(char *filename, Image *image){
	FILE *fp;
	fp = fopen(filename, "wb");
	if (!fp){ 
		fprintf(stderr, "Unable to open file '%s'\n", filename); 
		exit(1);
	}


	//file type
	fprintf(fp, "P6\n");

	fprintf(fp, "# Created by DATBOI");

	//size
	fprintf(fp, "%d %d\n",image->x,image->y);

	//depth
	fprintf(fp, "%d\n", 255);


	//write data for image
	fwrite(image->data, 3 * image->x, image->y, fp);
	
	fclose(fp);
}

int main(int argc, char* argv[]) {
	//if( *argv[2] != '3' || *argv[2] != '6'){
	//	fprintf(stderr, "invalid arguments given\n");
	//}
	Image *image;
	image = readP6(argv[2]);
	writeP6(argv[3], image);
	return 0;
}
