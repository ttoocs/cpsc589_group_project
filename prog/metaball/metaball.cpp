//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.
// Moved the meta-ball functs out of main and put here instead.
#include "metaball.h"

std::vector<MetaBall*> MetaBall::metaballs;

MetaBall::MetaBall(vec3 newPos, double radius, float(*f)(vec3, vec3, float))
	{
		pos = newPos;
		radius = radius;
		m_surfaceFunction = f;
		
		metaballs.push_back(this);
	}

float MetaBall::valueAt(vec3 loc)
	{
		return (*m_surfaceFunction)(pos,loc,radius);
	}

//START: From Eds file
float MetaBall::accumMetaBallFuncs(vec3 point)
{
	float accum = 0.0;
	for (int i = 0; i < metaballs.size(); i++)
	{
		//accum += metaballs[i]->function(point);
		accum += metaballs[i]->valueAt(point);
	}
	return accum;
}


void MetaBall::March(std::vector<vec3> * verts, std::vector<GLuint> * idx, std::vector<MetaBall*> * mbs, vec3 * lbound, vec3 * ubound, double granularity){
  
//#define DUMMY
#ifdef DUMMY
  verts->clear(); //Temporary only.
  verts->push_back(vec3(0,0,0));
  verts->push_back(vec3(1,1,1));
  verts->push_back(vec3(1,-1,-1));
  idx->clear();
  idx->push_back(0);
  idx->push_back(1);
  idx->push_back(2);
  return;

#undef DUMMY  
#endif

  if(mbs == NULL){
    std::cout << "No metaballs given, using all metaballs." << std::endl;
    mbs = &metaballs;
  }

  vec3 lb,ub; //Just some place to store data for abit, if needed. (SHOULD BE MORE/LESS UNUSED)

  if(lbound == NULL){
    //Find the bounding box: (take furthest metaball-pos's, and use that for a box.)
    //TODO
    lb = vec3(-10,-10,-10);
    lbound = &lb;
  }
  if(ubound == NULL){
    //Find the bounding box: (take furthest metaball-pos's, and use that for a box.)
    //TODO
    ub = vec3(10,10,10);
    ubound = &ub;
  }
  
  //TODO: Make it actually only use some meta-balls... lambda functions? 

  March1((MetaBall::accumMetaBallFuncs),
         *lbound,*ubound,
          granularity,
          verts,
          idx
        );

}


/*
//This should be the marching cube algorithm
void loadPoints()
{
	float accumulator = 0.0;

	for (float z = -2.0; z < 2.0; z += 0.05)
	{
		for (float y = -2.0; y < 2.0; y += 0.05)
		{
			for (float x = -2.0; x < 2.0; x += 0.05)
			{
				accumulator = accumMetaBallFuncs(vec3(x, y, z));
				
				if (accumulator >= threshold)
				{
					vertices.push_back(x);
					if (y < 0.0)
						vertices.push_back(0.0);
					else
						vertices.push_back(y);
					vertices.push_back(z);
				}
				accumulator = 0.0;
			}
		}
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	num_points = vertices.size() / 3;

	// Clear vertices vector
	vertices.clear();
}
*/
//END: From Eds file



float WyvillMetaBall(vec3 mbpos, vec3 tpos, float radius)
	{
		float r = length(mbpos - tpos);

		if (r > radius)
			return 0;

		float term1, term2, term3;
		float R = r / radius;

		term1 = (-4.0 / 9.0) * pow(R, 6.0);
		term2 = (17.0 / 9.0) * pow(R, 4.0);
		term3 = (22.0 / 9.0) * pow(R, 2.0);
		float total = term1 + term2 - term3;
		//float total = (float) term1 + (float) term2 - (float) term3 + (float) 1.0;
		return term1 + term2 - term3 + 1.0;
	}

