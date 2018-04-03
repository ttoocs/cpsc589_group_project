//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#include "cloud.h"
#include "stdlib.h"
#include <vector>


using namespace std;
void cloud::create_cloud(vector<vec3> *points, vector<GLuint> *indices,vector<vec3> *norms, int rounds)
{
	vec3 p0 = vec3(0,0,0);
	vec3 p1 = vec3(0,0,0);
	vec3 p2 = vec3(0,0,0);
	
	float r0 = 0.0f;
	float r1 = 0.0f;
	float radius = ((float)rand())/((float)INT_MAX);
	
	//balls;
    points->clear();
    indices->clear();
    norms->clear();
	MetaBall::March(points,indices,norms, &balls);
	int a=0;
	for(int j = 0; j < rounds; j++)
	{
		for(int i = 0; i < indices->size()/3; i++)
		{
      if(((float)rand())/((float)INT_MAX) > 0)
      {
        //START: Random position inside square for new metaball
        p0 = (*points)[(*indices)[i*3]];
        p1 = (*points)[(*indices)[i*3+1]];
        p2 = (*points)[(*indices)[i*3+2]];
        
        r0 = ((float)rand())/((float)INT_MAX);
        r1 = (1.0f-r0)*((float)rand())/((float)INT_MAX);
        
        
        p0 = (1.0f-r1-r0)*p2 + r1*p1 + r0*p0;
        
        //END: Random position inside square for new metaball
        
        balls.push_back((new MetaBall(p0+vec3(rand()%5,rand()%5,rand()%5),0.1*((float)rand())/((float)INT_MAX),balls[0]->m_surfaceFunction)));
        std::cout << a++ << std::endl;
      }
		}
    points->clear();
    indices->clear();
    norms->clear();
		MetaBall::March(points,indices,norms, &balls);
	}
  cout <<"Done making a cloud\n";
}
