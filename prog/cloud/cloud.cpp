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

//START: In here FALSE means ON
#define Q_GRAN .6
#define F_GRAN .3
#define A_GROW false //Make this true if you want balls growing
//In all the directions.  False if you only want them growing up.
#define HEIGHT_ANGLE PI/2.5f
#define HEIGHT_ANGLE_DOWN PI/2.5f
#define TT_GROW true
#define U_GROW false
#define D_GROW false
//END: In here FALSE means ON

float AvgNew = 3;
float AvgPer = 0.5;

std::vector<cloud*> cloud::allClouds;

using namespace std;
void cloud::process_cloud_paper(Tris& t, int rounds)
{
	vec3 p0 = vec3(0,0,0);
	vec3 p1 = vec3(0,0,0);
	vec3 p2 = vec3(0,0,0);
  vec3 test_norm = vec3(0,0,0);
  float test_angle = 0.0f;
  float test_angle_down = 0.0f;
  int num_of_new_balls = 0;
  double prob_to_get_ball = 0.0f;
	
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
      test_angle_down = acos(dot(normalize(test_norm),vec3(0,1,0)));

      if((A_GROW)||(U_GROW)||(test_angle > HEIGHT_ANGLE))
       {
        if((A_GROW)||(D_GROW)||(test_angle_down > HEIGHT_ANGLE_DOWN))
         {
            float rng = (((float)rand())/((float)INT_MAX));
            prob_to_get_ball =((float)(indices->size()/3));
            if(!U_GROW)
            {
              prob_to_get_ball *= (fabs(PI-HEIGHT_ANGLE)/PI);
            }
            if(!D_GROW)
            {
              prob_to_get_ball *= (fabs(PI-HEIGHT_ANGLE_DOWN)/PI);
            }
            
            if(rng < (AvgNew)/prob_to_get_ball)
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


cloud::cloud(float(*f)(vec3, vec3, float) , vec3 * pos, int initBalls, int rounds, int rad,float gap, int type){
{
  //Default pos/etc.
  vec3 npos;
  float angle = 0.0f;
  if(pos == NULL){ //Make a position if there isn't one given.
    float x = gap - 2*gap*(((float)rand())/((float)INT_MAX));
    float y = gap - 2*gap*(((float)rand())/((float)INT_MAX));
    float z = gap - 2*gap*(((float)rand())/((float)INT_MAX));
    npos = vec3(x,y,z);
    pos = &npos; 
  }
  //Default func
  if(f == NULL){
    f=WyvillMetaBall;
  }

	AvgNew *= initBalls;
  { //Gen balls
   // balls.push_back(new MetaBall(*pos, rad, f));
    float x = (*pos).x;
    float y = (*pos).y;
    float z = (*pos).z;
    for(int j = 0; j < initBalls;j++)
    {
		if(type == 0)
		{
			x = (*pos).x+gap - 2*gap*(((float)rand())/((float)INT_MAX));
			y = y;//+gap - 2*gap*(((float)rand())/((float)INT_MAX));
			z = (*pos).z+gap - 2*gap*(((float)rand())/((float)INT_MAX));
		}
		else if(type ==1)
		{
			angle = 2*PI*(((float)rand())/((float)INT_MAX));
			x = gap*cos(angle);
			y = y;//+gap - 2*gap*(((float)rand())/((float)INT_MAX));
			z = gap*sin(angle);
		}
		else if(type == 3) //The old-style one.
		{
			x = (*pos).x - gap*(1 - 2*(((float)rand())/((float)INT_MAX)));
			y = (*pos).y - gap*(1 - 2*(((float)rand())/((float)INT_MAX)));
			z = (*pos).z - gap*(1 - 2*(((float)rand())/((float)INT_MAX)));
		}

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



MBS cloud::getMBs(){
  MBS r;
  for(auto it = balls.begin(); it != balls.end(); it++){
    MB t;
    t.pos = vec4((*it)->pos,0);
    int type=-1;
    if( (*it)->m_surfaceFunction ==  WyvillMetaBall)
      type=1;
    if( (*it)->m_surfaceFunction ==  sphereMB)
      type=2;
    if( (*it)->m_surfaceFunction ==  fanceyMB)
      type=3;

    t.info = vec4(type,(*it)->radius,0,0);
    r.push_back(t);
  }
  return r;
}

MBS cloud::getAllMBs(){
  MBS r;
  for(auto it = allClouds.begin(); it != allClouds.end(); it++){
    MBS m = (*it)->getMBs();
    for(auto is = m.begin(); is != m.end() ; is++){
    
    r.push_back(*is);

    }
  }
  return r;
}
