#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ppmrw.h"
#include "json.h"
#include "base.h"
#include "vector_math.h"

#define MAX_COLOR_VAL 255   // maximum color to support for RGB

/* custom types */
typedef struct ray_t {
    double origin[3];
    double direction[3];
} Ray;

/* functions */
void raycast_scene(image*, double, double, object*);
void reflection_vector(V3 direction, V3 position, int obj_index, V3 reflection);
int get_camera(object*);
#endif