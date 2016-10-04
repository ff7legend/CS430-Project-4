#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "include/ppmrw.h"

int comments_check(FILE *fh, char c) {
    // checks for any comments in ppm file
    while (isspace(c) && c != EOF) { c = fgetc(fh); }

    // base case, current char, c, is not a pound sign
    if (c != '#') { fseek(fh, -1, SEEK_CUR); return 0;}
    
    else { // read to end of line
        while (c != '\n' && c != EOF) {
            c = fgetc(fh);
        }
        
        if (c == EOF) { fprintf(stderr, "Error: comments_check: Premature end of file\n"); return -1; }
        else { //grab char and recurse
            return comments_check(fh, fgetc(fh));
        }
    }
}

int newline_check(char c) {
    if (!isspace(c)) {fprintf(stderr, "Error: newline_check: missing newline or space\n");
        return -1;
    }
    return 0;
}

int bytes_left(FILE *fh) {
    // returns the number of bytes left in a file
    int bytes_left;
    int place = ftell(fh);    
    fseek(fh, 0, SEEK_END);
    int end = ftell(fh);
    bytes_left = end - place;
    fseek(fh, place, SEEK_SET); 
    if (bytes_left <= 0) {
        fprintf(stderr, "Error: bytes_left: bytes remaining <= 0\n");
        return -1;
    }
    return bytes_left;
}


int read_header(FILE *fh, header *hdr) {
    int ret_val;    // holds temp return value 
    char c;         // temporary char 
    boolean is_p3;  // P3 or P6

    // read magic number
    c = fgetc(fh);
    if (c != 'P') {
        fprintf(stderr, "Error: read_header: Invalid ppm file. First character is not 'P'\n");
        return -1;
    }
    c = fgetc(fh);
    if (c == '3') {
        is_p3 = TRUE;
    }
    else if (c == '6') {
        is_p3 = FALSE;
    }
    else {
        fprintf(stderr, "Error: read_header: Unsupported magic number found in header\n");
        return -1;
    }

    if (is_p3) {
        hdr->file_type = 3;
    }
    else {
        hdr->file_type = 6;
    }
    ret_val = newline_check(fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: No separator found after magic number\n");
        return -1;
    }
    ret_val = comments_check(fh, fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: Problem reading comment after magic number\n");
        return -1;
    }
    
 // read height
    ret_val = fscanf(fh, "%d", &(hdr->height));
    if (ret_val <= 0 || ret_val == EOF) {
        fprintf(stderr, "Error: read_header: Image height not found\n");
        return -1;
    }
    ret_val = newline_check(fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: No separator found after height\n");
        return -1;
    }
    ret_val = comments_check(fh, fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: Problem reading comment after height\n");
        return -1;
    }
    
    // read width
    fscanf(fh, "%d", &(hdr->width));
    if (hdr->width <= 0) {
        fprintf(stderr, "Error: read_header: Image width cannot be less than zero\n");
        return -1;
    }
    if (hdr->width == EOF) {
        fprintf(stderr, "Error: read_header: Image width not found. Premature EOF\n");
        return -1;
    }
    
    ret_val = newline_check(fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: No separator found after width\n");
        return -1;
    }
    ret_val = comments_check(fh, fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: Problem reading comment after width\n");
        return -1;
    }
    
    ret_val = fscanf(fh, "%d", &(hdr->max_color_val));
    if (ret_val <= 0 || ret_val == EOF) {
        fprintf(stderr, "Error: read_header: Max color value not found\n");
        return -1;
    }
    if (hdr->max_color_val > 255 || hdr->max_color_val < 0) {
        fprintf(stderr, "Error: max color value must be >= 0 and <= 255\n");
        return -1;
    }
    ret_val = newline_check(fgetc(fh));
    if (ret_val < 0) {
        fprintf(stderr, "Error: read_header: No separator found after max color value\n");
        return -1;
    }
        

    return 0;
}

int write_p6_data(FILE *fh, image *img) {
    int i,j;
    for (i=0; i<(img->height); i++) {
        for (j=0; j<(img->width); j++) {
            fwrite(&(img->map[i * img->width + j].r), 1, 1, fh);
            fwrite(&(img->map[i * img->width + j].g), 1, 1, fh);
            fwrite(&(img->map[i * img->width + j].b), 1, 1, fh);
        }
    }
    return 0;
}


int P6_Read(FILE *fh, image *img) {

    int b = bytes_left(fh);
    if (b < 0) {
        fprintf(stderr, "Error: P6_Read: Problem reading remaining bytes in image\n");
        return -1;
    }

    unsigned char data[b+1];
    int read;

    if ((read = fread(data, 1, b, fh)) < 0) {
        fprintf(stderr, "Error: P6_Read: fread() returned an error when reading data\n");
        return -1;
    }

    if (read < b || read > b) {
        fprintf(stderr, "Error: P6_Read: image data doesn't match header dimensions\n");
        return -1;
    }

    int ptr = 0;       
    int i, j, k;     
    unsigned char num; 

    for (i=0; i<img->height; i++) {
        for (j=0; j<img->width; j++) {
            RGBPixel px;
            for (k=0; k<3; k++) {
                if (ptr >= b) {
                    fprintf(stderr, "Error: P6_Read: Image data is missing or header dimensions are wrong\n");
                    return -1;
                }
                num = data[ptr++];
                if (num < 0 || num > img->max_color_val) {
                    fprintf(stderr, "Error: P6_Read: found a pixel value out of range\n");
                    return -1;
                }

                if (k == 0) {
                    px.r = num;
                }
                else if (k == 1) {
                    px.g = num;
                }
                else {
                    px.b = num;
                }
            }
            img->map[i * img->width + j] = px;
        }
    }
    if (ptr < b) {
        fprintf(stderr, "Error: P6_Read: Extra image data was found in file\n");
        return -1;
    }
    return 0;
}

int p3_read(FILE *fh, image *img) {
    

    int b = bytes_left(fh); 
    if (b < 0) {
        fprintf(stderr, "Error: p3_read: reading remaining bytes\n");
        return -1;
    }

    // create temp buffer 
    char data[b+1];
    char *data_p = data;
    int read;

    if ((read = fread(data, 1, b, fh)) < 0) {
        fprintf(stderr, "Error: p3_read: fread returned an error when reading data\n");
        return -1;
    }
    
    if (read < b || read > b) {
        fprintf(stderr, "Error: p3_read: image data doesn't match header dimensions\n");
        return -1;
    }
    
    data[b] = '\0';
    // make sure we're not starting at a space or newline
    while (isspace(*data_p) && (*data_p != '\0')) { data_p++; };
    
    int i, j, k;        
    int ptr;           
    char num[4];       

    for (i=0; i<img->height; i++) {
        for (j=0; j<img->width; j++) {
            RGBPixel px;
            for (k=0; k<3; k++) {
                ptr = 0;
                while (TRUE) {
                    if (*data_p == '\0') {
                        fprintf(stderr, "Error: p3_read: Image data is missing or header dimensions are wrong\n");
                        return -1;
                    }
                    if (isspace(*data_p)) {
                        *(num + ptr) = '\0';
                        while (isspace(*data_p) && (*data_p != '\0')) { 
                            data_p++; 
                        }
                        break;
                    }
                    else {
                        *(num + ptr) = *data_p++;
                        ptr++;
                    }
                }

                if (atoi(num) < 0 || atoi(num) > img->max_color_val) {
                    fprintf(stderr, "Error: p3_read: found a pixel value out of range\n");
                    return -1;
                }

                if (k == 0) {
                    px.r = atoi(num);
                }
                else if (k == 1) {
                    px.g = atoi(num);
                }
                else {
                    px.b = atoi(num);
                }
                img->map[i * img->width + j] = px;
            }
        }
    }

    while (isspace(*data_p) && (*data_p != '\0')) { data_p++; };
 
    if (*data_p != '\0') {
        fprintf(stderr, "Error: p3_read: Extra image data was found in file\n");
        return -1;
    }
    return 0;
}


int p3_write(FILE *fh, image *img) {
    int i,j;
    for (i=0; i<(img->height); i++) {
        for (j=0; j<(img->width); j++) {
            fprintf(fh, "%d ", img->map[i * img->width + j].r);
            fprintf(fh, "%d ", img->map[i * img->width + j].g);
            fprintf(fh, "%d\n", img->map[i * img->width + j].b);
        }
    }
    return 0;
}

int header_write(FILE *fh, header *hdr) {
    int ret_val = 0;
    ret_val = fputs("P", fh);
    if (ret_val < 0) {
        return -1;
    }
    ret_val = fprintf(fh, "%d", hdr->file_type);
    if (ret_val < 0) {
        return -2;
    }
    ret_val = fputs("\n", fh);
    if (ret_val < 0) {
        return -3;
    }
    ret_val = fprintf(fh, "%d %d\n%d\n", hdr->width,
                                         hdr->height,
                                         hdr->max_color_val);
    if (ret_val < 0) {
        return -4;
    }
    return ret_val;
}

void ppm_create(FILE *fh, int type, image *img) {
    // error checking
    if (type != 3 && type != 6) {
        fprintf(stderr, "Error: ppm_create: type must be 3 or 6\n");
        exit(1);
    }
    header hdr;
    hdr.file_type = type;
    hdr.width = img->width;
    hdr.height = img->height;
    hdr.max_color_val = 255;
    int res = header_write(fh, &hdr);
    if (res < 0) {
        fprintf(stderr, "Error: ppm_create: Problem writing header to file\n");
        exit(1);
    }
    // write data
    if (type == 3)
        p3_write(fh, img);
    else
        write_p6_data(fh, img);
} 

