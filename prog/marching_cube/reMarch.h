#include <vector>
#include "../types.h"

//#include "GL/glut.h"
#include "../gl_helpers.h"
//typedef uint GLuint;

void March1(double (*f)(double, double, double),
                            vec3 a_lowerBound, vec3 a_upperBound,
                            double a_granularity,
                            std::vector<vec3> * verts, std::vector<GLuint> * idx,
                            std::vector<vec3> * norms=NULL);

