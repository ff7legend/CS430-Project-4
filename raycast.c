#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "include/raycast.h"


int get_camera(object *objects) {
    int i = 0;
    while (i < MAX_OBJECTS && objects[i].type != 0) {
        if (objects[i].type == CAMERA) {
            return i;
        }
        i++;
    }
    return -1;
}


void shade_pixel(double *color, int row, int col, image *img) {
    img->map[row * img->width + col].r = color[0];
    img->map[row * img->width + col].g = color[1];
    img->map[row * img->width + col].b = color[2];
}


double plane_intersect(double *Ro, double *Rd, double *Pos, double *Norm) {
    normalize(Norm);
    double vd = v3_dot(Norm, Rd);
    
    if (fabs(vd) < 0.0001) return -1;

    double vector[3];
    v3_sub(Pos, Ro, vector);
    double t = v3_dot(vector, Norm) / vd;

    if (t < 0.0)
        return -1;

    return t;
}


double sphere_intersect(double *Ro, double *Rd, double *C, double r) {
    double b, c;
    double vector_diff[3];
    v3_sub(Ro, C, vector_diff);

    //quadratic formula
    b = 2 * (Rd[0]*vector_diff[0] + Rd[1]*vector_diff[1] + Rd[2]*vector_diff[2]);
    c = sqr(vector_diff[0]) + sqr(vector_diff[1]) + sqr(vector_diff[2]) - sqr(r);

   
    double disc = sqr(b) - 4*c;
    double t;  
    if (disc < 0) {
        return -1;
    }
    disc = sqrt(disc);
    t = (-b - disc) / 2.0;
    if (t < 0.0)
        t = (-b + disc) / 2.0;

    if (t < 0.0)
        return -1;
    return t;
}

void raycast(image *img, double cam_width, double cam_height, object *objects) {
  
    int i;  // x 
    int j;  // y 
    int o;  // object 
    double vp_pos[3] = {0, 0, 1};   
    double Ro[3] = {0, 0, 0};       
    double point[3] = {0, 0, 0};    

    double pixheight = (double)cam_height / (double)img->height;
    double pixwidth = (double)cam_width / (double)img->width;
    printf("pixw = %lf\n", pixheight);
    printf("pixh = %lf\n", pixwidth);
    printf("camw = %lf\n", cam_height);
    printf("camh = %lf\n", cam_width);
    double Rd[3] = {0, 0, 0};       
    point[2] = vp_pos[2];    

    for (i = 0; i < img->height; i++) {
        point[1] = -(vp_pos[1] - cam_height/2.0 + pixheight*(i + 0.5));
        for (j = 0; j < img->width; j++) {
            point[0] = vp_pos[0] - cam_width/2.0 + pixwidth*(j + 0.5);
            normalize(point); 
            Rd[0] = point[0]; // store point as ray direction
            Rd[1] = point[1];
            Rd[2] = point[2];

            int best_o = 0;
            double best_t = INFINITY;
            for (o=0; objects[o].type != 0; o++) {
                double t = 0;
                switch(objects[o].type) {
                    case 0:
                        printf("no object found\n");
                        break;
                    case CAMERA:
                        break;
                    case SPHERE:
                        t = sphere_intersect(Ro, Rd, objects[o].sph.position,
                                                        objects[o].sph.radius);
                        break;
                    case PLANE:
                        t = plane_intersect(Ro, Rd, objects[o].pln.position,
                                                    objects[o].pln.normal);
                        break;
                    default:
                        exit(1);
                }
                if (t > 0 && t < best_t) {
                    best_t = t;
                    best_o = o;
                }
            }
            if (best_t > 0 && best_t != INFINITY) {
                if (objects[best_o].type == PLANE) {
                    shade_pixel(objects[best_o].pln.color, i, j, img);
                }
                else if (objects[best_o].type == SPHERE) {
                    shade_pixel(objects[best_o].sph.color, i, j, img);
                }
            }
            else {
            }
        }
    }
}
