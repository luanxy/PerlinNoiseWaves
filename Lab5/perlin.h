#ifndef _perlin_
#define _perlin_

#include "vec3.h"

vec3 grad(vec3 p);

float fade(float t);

float noise(vec3 p);

#endif