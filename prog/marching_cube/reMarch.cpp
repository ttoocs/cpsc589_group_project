#include "reMarch.h"
#include "MarchingSource.h"
#include "../center.h"


vec3 getNorm(double (*f)(double,double,double), vec3 pos)
{
  double rx,ry,rz;
  numerical::numericalDeriv3D(pos.x,pos.y,pos.z,f,rx,ry,rz);
  return -vec3(rx,ry,rz);
}

void March1(double (*f)(double, double, double),
                            vec3 a_lowerBound, vec3 a_upperBound,
                            double a_granularity,
                            std::vector<vec3> * verts, std::vector<GLuint> * idx, std::vector<vec3> * norms)
{
    // variables to hold raw triangles returned from marching cubes algorithm
    GLint tcount;
    GLfloat vertices[5*3*3];

#define USEMP
#ifdef USEMP
    //OpenMP: Doesn't like floats
    // To fix this, we can scale everything to stepsize of '1'
    double scale = 1/a_granularity;
    
        #pragma omp parallel for
    for(int ix = (int) (a_lowerBound.x*scale); ix <= (int) (a_upperBound.x*scale + 1); ix++){
        #pragma omp parallel for
      for(int iy = (int) (a_lowerBound.y*scale); iy <= (int)(a_upperBound.y*scale + 1); iy++){
        #pragma omp parallel for private(vertices, tcount)
        for(int iz = (int)(a_lowerBound.z*scale); iz <= (int)(a_upperBound.z*scale + 1); iz++){
          
          double x = ((double) ix)*a_granularity;
          double y = ((double) iy)*a_granularity;
          double z = ((double) iz)*a_granularity;
//          std::cout << "(" << ix << "," << iy << "," << iz << ")\t(" << x << "," << y << "," << z << ")" << std::endl;

#else
    // sample the implicit surface by stepping through each dimension
    for (float x = a_lowerBound.x; x <= a_upperBound.x; x += a_granularity){
        for (float y = a_lowerBound.y; y <= a_upperBound.y; y += a_granularity){
            for (float z = a_lowerBound.z; z <= a_upperBound.z; z += a_granularity){
            

#endif
                // call marching cubes to get the triangular facets for this cell
                vMarchCubeCustom(x, y, z, a_granularity, f, tcount, vertices);

                // add resulting triangles (if any) to our mesh
                  //Std::vectors are _not_ multithread safe.
                #pragma omp critical
                for (int i = 0; i < tcount; ++i) {
                    int ix = i*9;
                    verts->push_back(vec3(vertices[ix+0], vertices[ix+1], vertices[ix+2]));
                    idx->push_back(verts->size()-1);
                    if(norms != NULL)
                      norms->push_back(   getNorm(f , (*verts)[verts->size()-1] ) );

                    verts->push_back(vec3(vertices[ix+3], vertices[ix+4], vertices[ix+5]));
                    idx->push_back(verts->size()-1);
                    if(norms != NULL)
                      norms->push_back(   getNorm(f , (*verts)[verts->size()-1] ) );

                    verts->push_back(vec3(vertices[ix+6], vertices[ix+7], vertices[ix+8]));
                    idx->push_back(verts->size()-1);
                    if(norms != NULL)
                      norms->push_back(   getNorm(f , (*verts)[verts->size()-1] ) );
                }
            }
        }
    }
//    std::exit(0);
}

