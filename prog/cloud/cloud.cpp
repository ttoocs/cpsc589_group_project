//Scott Saunders
// 1: The most basic of basic cloud, a meta-ball.
// 3: Place them randomly in the 5x5 box
// 4: Remove 5x5 box-thing, make it randomly select X
// 5: Make it's granularity Q_GRAN, -> faster (lower res march).

//Cory Jenson
// 2: Create_cloud
#include "cloud.h"
#include "stdlib.h"
#include <vector>
#include <math.h>

#include "../main.h"


#define Q_GRAN .3
#define F_GRAN .15
#define A_GROW false //Make this true if you want balls growing
//In all the directions.  False if you only want them growing up.
#define HEIGHT_ANGLE PI/2.0f

float AvgNew = 10;
float AvgPer = 0.5;


std::vector<cloud*> cloud::allClouds;

using namespace std;
/*
void cloud::process_cloud_naive(vector<vec3> *points, vector<GLuint> *indices,vector<vec3> *norms, int rounds)
{
  
  std::cout << "DO NOT USE" << std::endl;
  exit(1);
  
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
  //March getting points/indexs/norms, on balls, unknown bounding boxes, granularity =Q_GRAN (half-res of new default)
	  MetaBall::March(points,indices,norms, &balls, NULL, NULL, Q_GRAN);
		for(int i = 0; i < indices->size()/3; i++)
		{
		  float rng = (((float)rand())/((float)INT_MAX));
		  if(rng < (AvgNew)/((float)(indices->size()/3)))
		  {
        //START: Random position inside square for new metabalal
        p0 = (*points)[(*indices)[i*3]];
        p1 = (*points)[(*indices)[i*3+1]];
        p2 = (*points)[(*indices)[i*3+2]];
        
        r0 = ((float)rand())/((float)INT_MAX);
        r1 = (1.0f-r0)*((float)rand())/((float)INT_MAX);
        
        
        p0 = (1.0f-r1-r0)*p2 + r1*p1 + r0*p0 +(*norms)[i]*(((float)rand())/((float)INT_MAX)*j*5);
        
        //END: Random position inside square for new metaball
        //Random size:
        float rad = ((float)rand())/((float)INT_MAX)*exp(1.0/(j+1));
        
        balls.push_back((new MetaBall(p0,rad,balls[0]->m_surfaceFunction)));
    //		std::cout << a++ << std::endl;
		  }
		}
   // std::cout << "cloud round done" << std::endl;
	}
  points->clear();
  indices->clear();
  norms->clear();
	MetaBall::March(points,indices,norms, &balls, NULL, NULL, F_GRAN);
  cout <<"Done making a cloud\n";
}
*/
//void cloud::process_cloud_paper(vector<vec3> *points, vector<GLuint> *indices,vector<vec3> *norms, int rounds)
void cloud::process_cloud_paper(Tris& t, int rounds)
{
	vec3 p0 = vec3(0,0,0);
	vec3 p1 = vec3(0,0,0);
	vec3 p2 = vec3(0,0,0);
  vec3 test_norm = vec3(0,0,0);
  float test_angle = 0.0f;
  int num_of_new_balls = 0;
	
	float r0 = 0.0f;
	float r1 = 0.0f;
	float radius = ((float)rand())/((float)INT_MAX);
	
  vector<vec3>* points = new vector<vec3>;
  vector<vec3>* norms = new vector<vec3>;
  vector<GLuint>* indices = new vector<GLuint>;

	int a=0;
	for(int j = 0; j < rounds; j++)
	{
		points->clear();
		indices->clear();
		norms->clear();
		//March getting points/indexs/norms, on balls, unknown bounding boxes, granularity =Q_GRAN (half-res of new default)
		MetaBall::March(points,indices,norms, &balls, NULL, NULL, Q_GRAN);
		for(int i = 0; i < indices->size()/3; i++)
		{
      test_norm = ((*norms)[(*indices)[i*3]]+(*norms)[(*indices)[i*3+1]]+(*norms)[(*indices)[i*3+2]])/3.0f;
      test_angle = acos(dot(normalize(test_norm),vec3(0,-1,0)));
      if((A_GROW)||(test_angle > HEIGHT_ANGLE))
       {
          float rng = (((float)rand())/((float)INT_MAX));
          if(rng < (AvgNew)/((float)(indices->size()/3)*(fabs(PI-HEIGHT_ANGLE)/PI)))
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
            float rad = 1.0 + ((float)rand())/((float)INT_MAX)*pow(0.5,j+1);
            
            balls.push_back((new MetaBall(p0,rad,balls[0]->m_surfaceFunction)));
            num_of_new_balls++;
        //		std::cout << a++ << std::endl;
            }
        }
        else
        {
          
        }
		}
   // std::cout << "cloud round done" << std::endl;
	}
	points->clear();
	indices->clear();
	norms->clear();
	MetaBall::March(points,indices,norms, &balls, NULL, NULL, F_GRAN);
	cout <<"Done processing a cloud\n";
  
  t = mergeTris(t,toTris(points,norms,indices));

}


// NOT THIS ONE
/*
void cloud::create_cloud(vector<vec3> *verts, vector<GLuint> *idx,vector<vec3> *norms, int numOfClouds, int m_in_cloud, int rounds)
{

  std::cout << "Despite my efforts, this seems to have issues now." << std::endl;
  std::cout << "You have been warned." << std::endl;
//  cloud clouds[numOfClouds];  //This caused issues with new cosntructor, hence:
  std::vector<cloud> clouds;
  for(int i = 0; i < numOfClouds ; i++){
    clouds.push_back(cloud(NULL,NULL,0,0,0,true)); //All but last ignored, the skip.
  }
  
  std::vector<vec3> verts_s;
  std::vector<GLuint> idx_s;
  std::vector<vec3> norms_s;
  for(int i = 0; i < numOfClouds;i++)
  {
    float x = -1+3*i;
    float y = 10 - 20*(((float)rand())/((float)INT_MAX));
    float z = 10 - 20*(((float)rand())/((float)INT_MAX));
    for(int j = 0; j < m_in_cloud;j++)
    {
      x = x+2 - 20*(((float)rand())/((float)INT_MAX));
      y = y+2 - 20*(((float)rand())/((float)INT_MAX));
      z = z+2 - 20*(((float)rand())/((float)INT_MAX));
   //   clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, fanceyMB));
      clouds[i].balls.push_back(new MetaBall(vec3(x,y,z), 1, WyvillMetaBall));
    }
  }

  verts->clear();
  idx->clear();
  norms->clear();
  
  //aCloud.process_cloud_paper(&verts, &idx,&norms, 3);
  for(int i = 0; i < numOfClouds;i++)
  {
    verts_s.clear();
    norms_s.clear();
    idx_s.clear();
    clouds[i].process_cloud_paper(&verts_s, &idx_s,&norms_s, rounds);

    Tris ms = mergeTris(toTris(verts,norms,idx),toTris(&verts_s,&norms_s,&idx_s));
    verts = ms.verts;
    norms = ms.norms;
    idx = ms.idx;
/*
    for(int j = 0; j < idx_s.size();j++) // J is rediculously large here, probably the issue. (Some compression does happen now.)
    {
      verts->push_back(verts_s[j]);
      norms->push_back(norms_s[j]); //Seems to segfault?q
      idx->push_back(idx->size());
    }


  }
 
}
*/

cloud::cloud(float(*f)(vec3, vec3, float) , vec3 * pos, int initBalls, int rounds, int rad, bool skip){
  if(! skip){
  //Default pos/etc.
  vec3 npos;
  if(pos == NULL){ //Make a position if there isn't one given.
    float x = -1+3;
    float y = 10 - 20*(((float)rand())/((float)INT_MAX));
    float z = 10 - 20*(((float)rand())/((float)INT_MAX));
    npos = vec3(x,y,z);
    pos = &npos; 
  }
  //Default func
  if(f == NULL){
    f=WyvillMetaBall;
  }

  { //Gen balls
    balls.push_back(new MetaBall(*pos, rad, f));
    float x = (*pos).x;
    float y = (*pos).y;
    float z = (*pos).z;
    for(int j = 0; j < initBalls;j++)
    {
      x = x+3 -	6*(((float)rand())/((float)INT_MAX));
      y = y+3 - 6*(((float)rand())/((float)INT_MAX));
      z = z+3 - 6*(((float)rand())/((float)INT_MAX));
      balls.push_back(new MetaBall(vec3(x,y,z), rad, f));
    }
  }  

  process_cloud_paper(tris, rounds);
  
  std::cout << "New cloud made." << std::endl;
  }
  allClouds.push_back(this);
}

/* The above is a default constructor as all args optional.
cloud::cloud(){
  std::cout << "Improperly(?) initalized cloud, fun may insue." << std::endl;
  allClouds.push_back(*this);
}
*/

Tris cloud::getAllTris(){
  Tris t;
  for(auto it = allClouds.begin(); it != allClouds.end(); it++){
    t = mergeTris(t, (*it)->getTris());
  }
  return t;
}

Tris cloud::getTris(){
//  return  toTris(&verts,&norms,&idx);
  return tris;
}
