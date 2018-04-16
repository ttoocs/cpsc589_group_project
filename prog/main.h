#pragma once


#include "types.h"
#include "lightning_stuff/bspline.h"

extern Tris renderTris;
extern int MODE;

extern BSpline spline;

extern void loadSpline();

#define MODE_CLOUD   0
#define MODE_BSPLINE 1
#define MODE_RAY     2
