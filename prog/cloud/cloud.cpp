//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.

#include "cloud.h"
#include "stdlib.h"

void create_cloud()
{
	vector<vec3> points;
	vector<int> indices;
	vec3 p0 = vec3(0,0,0);
	vec3 p1 = vec3(0,0,0);
	vec3 p2 = vec3(0,0,0);
	double r0 = 0.0f;
	double r1 = 0.0f;
	double radius = 1.0f;
	
	//balls;
	MetaBall::March(&points,&indices, balls);
	
	for(int i = 0; i < indicies.size()/3; i++)
	{
		//START: Random position inside square for new metaball
		p0 = points[indices[i*3]];
		p1 = points[indices[i*3+1]];
		p2 = points[indices[i*3+2]];
		
		r0 = ((double)rand())/((double)INT_MAX);
		r1 = (1.0-r0)*((double)rand())/((double)INT_MAX);
		
		p0 = (1.0-r1-r0)*p2+(r1)*p1+(r0)*p0;
		//END: Random position inside square for new metaball
		
		balls.push_back(&(new MetaBall(p0,radius,balls[0]->m_surfaceFunction)));
	}
	
	
	
}
