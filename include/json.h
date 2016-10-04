#ifndef JSON_H
#define JSON_H
#endif

#ifdef JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_OBJECTS 128    
#define CAMERA 1
#define SPHERE 2
#define PLANE 3

typedef struct camera_t {
    double width;
    double height;
} camera;

typedef struct sphere_t {
    double *color;
    double *position;
    double radius;
} sphere;

typedef struct plane_t {
    double *color;
    double *position;
    double *normal;
} plane;

typedef struct object_t {
    int type; 
    union {
        camera cam;
        sphere sph;
        plane pln;
    };
} object;

extern int line;
extern object objects[MAX_OBJECTS];

void read_json(FILE *json);
void print_objects(object *obj);

#endif