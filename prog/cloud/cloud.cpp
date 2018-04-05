//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.
// 3: Place them randomly in the 5x5 box
// 4: Remove 5x5 box-thing, make it randomly select X
// 5: Make it's granularity 0.25, -> faster (lower res march).

//Cory Jenson
// 2: Create_cloud
#include "cloud.h"
#include "stdlib.h"
#include <vector>

#include "../main.h"

int AvgNew = 10;

using namespace std;
void cloud::create_cloud(vector<vec3> *points, vector<GLuint> *indices,vector<vec3> *norms, int rounds)
{
	vec3 p0 = vec3(0,0,0);
	vec3 p1 = vec3(0,0,0);
	vec3 p2 = vec3(0,0,0);
	
	float r0 = 0.0f;
	float r1 = 0.0f;
	float radius = ((float)rand())/((float)INT_MAX);
	
	int a=0;
	for(int j = 0; j < rounds; j++)
	{
    points->clear();
    indices->clear();
    norms->clear();
  //March getting points/indexs/norms, on balls, unknown bounding boxes, granularity =0.25 (half-res of new default)
	  MetaBall::March(points,indices,norms, &balls, NULL, NULL, 0.25);
		for(int i = 0; i < indices->size()/3; i++)
		{
      float rng = (((float)rand())/((float)INT_MAX));
      if(rng < ((float) AvgNew)/((float)(indices->size()/3)))
      {
        //START: Random position inside square for new metaball
        p0 = (*points)[(*indices)[i*3]];
        p1 = (*points)[(*indices)[i*3+1]];
        p2 = (*points)[(*indices)[i*3+2]];
        
        r0 = ((float)rand())/((float)INT_MAX);
        r1 = (1.0f-r0)*((float)rand())/((float)INT_MAX);
        
        
        p0 = (1.0f-r1-r0)*p2 + r1*p1 + r0*p0;
        
        //END: Random position inside square for new metaball
        //Random size:
        float rad = 1*((float)rand())/((float)INT_MAX);
        
        balls.push_back((new MetaBall(p0,rad,balls[0]->m_surfaceFunction)));
        std::cout << a++ << std::endl;
      }
		}
    std::cout << "cloud round done" << std::endl;
	}
  points->clear();
  indices->clear();
  norms->clear();
	MetaBall::March(points,indices,norms, &balls, NULL, NULL, 0.25);
  cout <<"Done making a cloud\n";
}
