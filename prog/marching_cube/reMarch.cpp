#include "reMarch.h"
#include "MarchingSource.h"

void March1(double (*f)(double, double, double),
                            vec3 a_lowerBound, vec3 a_upperBound,
                            double a_granularity,
                            std::vector<vec3> * verts, std::vector<GLuint> * idx)
{
    // variables to hold raw triangles returned from marching cubes algorithm
    GLint tcount;
    GLfloat vertices[5*3*3];

    // sample the implicit surface by stepping through each dimension
    for (GLfloat x = a_lowerBound.x; x <= a_upperBound.x; x += a_granularity)
        for (GLfloat y = a_lowerBound.y; y <= a_upperBound.y; y += a_granularity)
            for (GLfloat z = a_lowerBound.z; z <= a_upperBound.z; z += a_granularity)
            {
                // call marching cubes to get the triangular facets for this cell
                vMarchCubeCustom(x, y, z, a_granularity, f, tcount, vertices);

                // add resulting triangles (if any) to our mesh
                for (int i = 0; i < tcount; ++i) {
                    int ix = i*9;
                    verts->push_back(vec3(vertices[ix+0], vertices[ix+1], vertices[ix+2]));
                    idx->push_back(verts->size()-1);
                    verts->push_back(vec3(vertices[ix+3], vertices[ix+4], vertices[ix+5]));
                    idx->push_back(verts->size()-1);
                    verts->push_back(vec3(vertices[ix+6], vertices[ix+7], vertices[ix+8]));
                    idx->push_back(verts->size()-1);
                }
            }
}

