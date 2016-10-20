#ifndef RAYCAST_H
#define RAYCAST_H
#endif

#ifdef RAYCAST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef JSON_H
#include "json.h"
#endif
#ifndef VECTOR_MATH_H
#include "vector_math.h"
#endif
#ifndef PPMRW_H
#include "ppmrw.h"
#endif

#define MAX_COLOR_VAL 255 

typedef struct ray_t {
    double origin[3];
    double direction[3];
} ray;


void raycast(image*, double, double, object*); 

int get_camera(object*);
#endif