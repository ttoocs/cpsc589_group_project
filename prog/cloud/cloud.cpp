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
	float radius = 1.0f;
	
	//balls;
	MetaBall::March(points,indices,norms, &balls);
	
	for(int j = 0; j < rounds; j++)
	{
		for(int i = 0; i < indices->size()/3; i++)
		{
			//START: Random position inside square for new metaball
			p0 = (*points)[(*indices)[i*3]];
			p1 = (*points)[(*indices)[i*3+1]];
			p2 = (*points)[(*indices)[i*3+2]];
			
			r0 = ((float)rand())/((float)INT_MAX);
			r1 = (1.0f-r0)*((float)rand())/((float)INT_MAX);
			
			
			p0 = (1.0f-r1-r0)*p2 + r1*p1 + r0*p0;
			//float t = (1.0-r1-r0);
			//vec3 tmp = ((float) t*p2;
			//tmp += r1*p1;
			//tmp += r0*p0;
			
			//END: Random position inside square for new metaball
			
			balls.push_back((new MetaBall(p0,radius,balls[0]->m_surfaceFunction)));
		}
		MetaBall::March(points,indices,norms, &balls);
	}
	
	
	
}
