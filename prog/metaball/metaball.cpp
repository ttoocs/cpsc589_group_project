//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.
// Moved the meta-ball functs out of main and put here instead.
#include "metaball.h"

std::vector<MetaBall*> MetaBall::metaballs;
std::vector<MetaBall*> * MetaBall::accumData;

MetaBall::MetaBall(vec3 newPos, double radius, float(*f)(vec3, vec3, float))
	{
		pos = newPos;
		this->radius = radius;
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
  if(accumData == NULL){
    std::cout << "No metaballs given, using all metaballs." << std::endl;
    accumData = &metaballs;
  }

  for( auto i = accumData->begin(); i != accumData->end(); i++) //Now uses accumData state-like system.
//	for (int i = 0; i < metaballs.size(); i++)
	{
		//accum += metaballs[i]->function(point);
		accum += (*i)->valueAt(point);
	}
  

//  std::cout << "\tpos:\t(" << point.x << ",\t"<< point.y << ",\t" << point.z << ")" << "\t\t" << "accum:" << accum << std::endl;
	return accum-1;
//  return 100;
}


void MetaBall::March(std::vector<vec3> * verts, std::vector<GLuint> * idx, std::vector<vec3> * norms, std::vector<MetaBall*> * mbs, vec3 * lbound, vec3 * ubound, double granularity){
  
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
    bool first = true;
    for(auto i = mbs->begin(); i != mbs->end(); i++){
      if(first){
        lb = (*i)->pos * (*i)->radius;
      }else{
        if((*i)->pos.x * (*i)->radius < lb.x)
          lb.x = (*i)->pos.x * (*i)->radius; 
        if((*i)->pos.y * (*i)->radius < lb.y)
          lb.y = (*i)->pos.y * (*i)->radius;
        if((*i)->pos.z * (*i)->radius < lb.z)
          lb.z = (*i)->pos.z * (*i)->radius;
      }
    }
//    lb = vec3(-10,-10,-10);
    lbound = &lb;
  }
  if(ubound == NULL){
    //Find the bounding box: (take furthest metaball-pos's, and use that for a box.)
    //TODO
    bool first = true;
    for(auto i = mbs->begin(); i != mbs->end(); i++){
      if(first){
        ub = (*i)->pos * (*i)->radius;
      }else{
        if((*i)->pos.x * (*i)->radius > lb.x)
          ub.x = (*i)->pos.x * (*i)->radius; 
        if((*i)->pos.y * (*i)->radius > lb.y)
          ub.y = (*i)->pos.y * (*i)->radius;
        if((*i)->pos.z * (*i)->radius> lb.z)
          ub.z = (*i)->pos.z * (*i)->radius;
      }
    }
    ubound = &ub;
  }
   
  //Setup accumMetaBallFuncs pnter,
  MetaBall::setAccumData(mbs);

  March1((MetaBall::accumMetaBallFuncs),
         *lbound,*ubound,
          granularity,
          verts,
          idx,
	  norms
        );

}

void MetaBall::setAccumData(std::vector<MetaBall*> * mbs){
  if(mbs == NULL){
    std::cout << "No metaballs given, using all metaballs." << std::endl;
    mbs = &metaballs;
  }
  MetaBall::accumData = mbs;
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

//    if( r < )
//      std::cout << "r-1\t" << r-1  << "\tpos:\t(" << tpos.x << ","<< tpos.y << "," << tpos.z << ")" << std::endl;
//    return (r-1);

//		if (r > radius)
//			return 0;

		float term1, term2, term3;
		float R = r / radius;

		term1 = (-4.0 / 9.0) * pow(R, 6.0);
		term2 = (17.0 / 9.0) * pow(R, 4.0);
		term3 = (22.0 / 9.0) * pow(R, 2.0);
		float total = term1 + term2 - term3;
		//float total = (float) term1 + (float) term2 - (float) term3 + (float) 1.0;
		return (term1 + term2 - term3  + 1 );

	}


float sphereMB(vec3 mbpos, vec3 tpos, float radius){
  float r = length(mbpos - tpos);
  return (r-radius);
  
}


float fanceyMB(vec3 mbpos, vec3 tpos, float radius){
      #define EPSILON 0.0001
      float x = tpos.x-mbpos.x;
      float y = tpos.y-mbpos.y;
      float z = tpos.z-mbpos.z;
      x*=x;
      y*=y;
      z*=z;
      float den = x+y+z;
      if(den < EPSILON  && den > EPSILON )
        return -1;
      return radius/den;
}


