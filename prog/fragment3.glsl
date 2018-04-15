//FRAGMENT3.GLSL - RayTracing metaballs


struct ray{
	vec3 origin;
	vec3 direction;
};

struct MB{
  vec4 pos;
  vec4 info;
};

#define EPSILON  0.000001
#define BEPSILON 0.001

////////////////////////////////// INPUT ///////////////////////////////
layout(std430, binding = 1) buffer allMB{
  vec4 mbInfo;
	MB mb[];
};

// uniform float FOV;

////////////////////////////////// END INPUT ///////////////////////////////

#define mbGetType(X) int(mb[int(X)].info.x)
//#define mbGetType(X) 2
#define mbGetPos(X) vec3(mb[int(X)].pos.x,mb[int(X)].pos.y,mb[int(X)].pos.z)
#define mbGetRad(X) mb[(X)].info.y
#define numMB int(mbInfo.x)
#define mbThres mbInfo.y;

#define mb_Wyvill 1
#define mb_sphere 2
#define mb_fancey 3


float thres = mbThres;
//////////////////////////////////META BALL FUNCS///////////////////////////////


float WyvillMetaBall(vec3 mbpos, vec3 tpos, float radius)
	{
		float r = length(mbpos - tpos);

		float term1, term2, term3;
		float R = r / radius;

		term1 = (-4.0 / 9.0) * pow(R, 6.0);
		term2 = (17.0 / 9.0) * pow(R, 4.0);
		term3 = (-22.0 / 9.0) * pow(R, 2.0);
		float total = term1 + term2 - term3;

		return radius / -(term1 + term2 + term3 + 1);

	}


float sphereMB(vec3 mbpos, vec3 tpos, float radius){
  float r = length(mbpos - tpos);
  return (r-(radius));  
}


float fanceyMB(vec3 mbpos, vec3 tpos, float radius){
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

//////////////////////////////////END META BALL FUNCS///////////////////////////////




int ERROR=0;
#define E_OTHER 1
#define E_TYPE 2

void set_error(int type){
	if(ERROR ==0)				//ONLY SET THE 1st ERROR.
		ERROR=type;
}
void error_out(){
  if(ERROR != 0)	
  	FragmentColour = vec4(1,1,0,1);			//MAKE SCREEN YELLOW ON OBJECT ERROR
}


////////////////////////////////// INTERSECTS ///////////////////////////////
vec2 ray_intersect_sphere(ray r, vec3 pos, float rad){
	
	float t1,t2;
	float a = dot(r.direction,r.direction);
	float b = 2* dot(r.direction, (r.origin - pos))  ;
	float c = dot((r.origin - pos),(r.origin - pos))-pow(rad,2);


	float det = (pow(b,2) - 4*a*c);
	if(det < EPSILON){return vec2(-1,-1);}
	t1 = (-b + sqrt(det))/2*a;
	t2 = (-b - sqrt(det))/2*a;

  vec2 re = vec2(-1,-1);
	if(t1 < t2 && t1 > 0){
		re.x = t1;
    re.y = t2;
	}else{
		re.x=t2;
		re.y=t1;
	}
  return re;
}
#ifdef OTHERINTERSECTS

#define TEST_CULL
float ray_intersect_triangle(ray r, uint obj){
	//Möller–Trumbore algorithm
	vec3 e1 = tri_p2(obj) - tri_p1(obj);	//SUB
	vec3 e2 = tri_p3(obj) - tri_p1(obj);	//SUB
	
	vec3 pvec = cross(r.direction, e2);		//CROSS
	float det = dot(e1,pvec);					//DOT

	#ifdef TEST_CULL
	if(det < EPSILON)
	#else
	if(det < EPSILON && det >-EPSILON)
	#endif
		return(-1);	
	vec3 tvec = r.origin - tri_p1(obj);
	float u = dot(tvec,pvec);
	if (u < 0.0 || u > det)
		return(-1);
	vec3 qvec = cross(tvec,e1);
	float v = dot(r.direction,qvec);
	if (v < 0.0 || u + v > det)
		return(-1);
	float t = dot(e2,qvec);
	t /= det;
	u /= det;
	v /= det;	
	return t;
}

////////////////////////////////// STILL INTERSECTS ///////////////////////////////
float ray_intersect_plane(ray r, uint obj){
	//(dot(O +td -q, n) =0

//	if(plane_ned(obj)==0 && ivec2(gl_GlobalInvocationID.xy) == ivec2(0)){		//Probably causes some error on 1st frame.
//		OBJ_SETV3(obj,3,normalize(plane_n(obj)));	
//		plane_ned(obj) = 1;
//	}
	vec3 norm = normalize(plane_n(obj));

	float d = dot(r.direction,norm);
	if(d < EPSILON && d > -EPSILON)
		return -1;
	float t = dot(norm,(plane_p(obj)-r.origin));
	t /= d;

	if(t <= EPSILON)
		return -1;

	return t;

}

float ray_intersect_point(ray r, uint obj){
	vec3 t= (-r.origin + light_p(obj));
	t.x /= r.direction.x;
	t.y /= r.direction.y;
	t.z /= r.direction.z;

	//Approxmation, ends up very poor.

	if(abs(t.x - t.y) < BEPSILON || abs(r.direction.x) < BEPSILON || abs(r.direction.y) < BEPSILON ){
		if(abs(t.x -t.z ) < BEPSILON || abs(r.direction.x) < BEPSILON || abs(r.direction.z) < BEPSILON ){
			if(abs(t.y - t.z) < BEPSILON || abs(r.direction.y) < BEPSILON || abs(r.direction.z) < BEPSILON ){
				return(min(t.x,min(t.y,t.z)));
			}
		}
	} 
	return -1;


	if(abs(t.x - t.y)< BEPSILON && abs(t.x - t.z) < BEPSILON && abs(t.y - t.z) < BEPSILON){
		return t.x;
	} else
		return -1;
	

}

#endif //OTHER INTERSECTS

////////////////////////////////// END INTERSECTS ///////////////////////////////




////////////////////////////////// METABALLS  ///////////////////////////////

//Meta-ball func based on pos
float MB_F(int ball, vec3 tpos){ // The meta-ball function.
 
  vec3 mbpos = mbGetPos(ball);

  switch(mbGetType(ball)){
    case 1:
      return WyvillMetaBall(mbpos,tpos,mbGetRad(ball));
    case 2:
      return sphereMB(mbpos,tpos,mbGetRad(ball));
    case 3: 
      return fanceyMB(mbpos,tpos,mbGetRad(ball));
  }
  return -1;

}

////////////////////////////////// METABALLS  ///////////////////////////////

//Finds sum of MB at pos/ray t.
float metaBall_func(vec3 pos){
  float s=0;
  for(int i=0; i<numMB; i++){
    s+= MB_F(i,pos);    //BLEND
//    s = max(s,MB_F(i,pos));    //UNION
//    s = min(s,MB_F(i,pos));    //INTER
//    s = min(s,-MB_F(i,pos));    //SUB
    
  }
  return s;
}
float metaBall_func(ray r, float t){ 
  vec3 pos= r.origin + r.direction*t;
  return metaBall_func(pos);
}

/////// INTERSECT



float ray_intersect_metaBalls(ray r, float minT=0){

  float t=minT;
  bool found = false;

  #define h 0.0001
  #define f(X) (metaBall_func(r,X)-thres)
  #define fp(X) ((f(X+h)-f(X-h))/2*h)
 

//  #define SGRAD_SEARCH  //Simple Gradient decent. //DOESNT DO VOLUMETRIC
//  #define SPHERE_SEARCH //Uses the sphere collision alg/
  #define LINEAR_SEARCH //Iterates
//  #define Newtons     //DOESNT DO VOLUMETRIC

  #ifdef SGRAD_SEARCH
    float stopThres = 0.0001;
    int maxSteps = 100;
    
    float s_init = f(0); //Now always checks for sign change.
    bool positive=false;
    if(s_init == abs(s_init))
       positive=true;

    while( maxSteps > 0 ){
      maxSteps --;
      if( (positive && (f(t) < 0)) || ((!positive && (f(t) > 0)))){
        found = true;
        break;
      }
      t += fp(t);
    }
  #endif

  #ifdef LINEAR_SEARCH 
  //////////////////////////////////////////Linear
  float maxT=20;
  float dt = 0.05;
  t=minT;

  float s_init = f(t); //Now always checks for sign change.
  bool positive=false;
  if(s_init == abs(s_init))
    positive=true;


  while( t < maxT  ){
    if( (positive && (f(t) < 0)) || ((!positive && (f(t) > 0))))
    {
//      t -= (dt/2);
      found = true;
      break;
    }
    if(fp(t) > 0){
      t += max(fp(t),dt);
    }else
      t+= dt;
  }

  #endif 

  #ifdef SPHERE_SEARCH ////////////////////////////////End linear search
    //Spherical collision
    t=minT;
    for(int i=0; i <= numMB ; i++){
      vec2 tn = ray_intersect_sphere(r, mbGetPos(i), mbGetRad(i)-0.05); //just under 1 values seem to look nicer
      if( tn.x > minT && (tn.x < t || t == minT)){
        t = tn.x;
        found = true;
      }
      if( tn.y > minT && (tn.x < t || t == minT)){
        t = tn.y;
        found = true;
      }
    }

  #endif

   #ifdef Newtons
    //////////////////////////////////////////////Newtons
      //Newtons method!
    if(found){
      int maxIter = 400;
      found = false;
      while(maxIter > 0){
        //Center difference:
        float yp = fp(t);

        if(abs(yp) < EPSILON){
           found = true;
            break;
           maxIter=-3; //Kill it if floats will screw us -> also assumes non-convergence
        }
      
        float tnew = t - (f(t)/yp);
        float delta = tnew - t;
        if(abs(delta) < 0.1){
          t=tnew;
          found= true;
          break; //Kill it if we're close enough enough.
        }

//        if(abs(delta) > 0.01){
//          0.01*tnew/abs(delta);
//        }

        t=tnew;
        maxIter --;
      }
      if(maxIter == -3 || !found){
        return -1; //Does not converge
      }else
        return t;
     
    }else
      return -1;
    #endif

  //End-catch
  if(found){
    return t;
  }else{
    return -1;
  }
  ////////////////////////////////////////////////EndNewtons
//  #undef h
//  #undef f
//  #undef fp
}

////////////////////////////////// END METABALLS  ///////////////////////////////





////////////////////////////////// RAYTRACE STUFF ///////////////////////////////

vec4 test_objects_intersect(ray r){ //Tests _ALL_ objects
  vec4 ret=vec4(0,0,0,0);
  float mb = ray_intersect_metaBalls(r,0);
  if(mb > 0)
  {
    ret.x = mb;
  }
  //Get second intercepts
  if(ret.x != 0){
    float mb2 = ray_intersect_metaBalls(r,ret.x);
    if(mb2 > 0)
    {
      ret.y = mb2;
    }
  }
	return(ret);
}

////////////////////////////////// RAYTRACE STUFF ///////////////////////////////
vec4 rtrace(ray cray){

	vec4 c=vec4(0);
	//////////////////BASIC RAY-TRACING///////////////////
//	check_light_hit=false;
	vec4 res = test_objects_intersect(cray);

	if(res.x <= 0)
    return c=vec4(-1);

  c = vec4(0,0,0,res.x);
  
  #define r cray

  //Simple colors from samthing
  const int numColors = 3;
  vec3 norms[numColors];
  vec3 colors[numColors];
  vec3 ambient = vec3(0.4);
 
  //Note: the following are + / -
  //Note: X: R/L 
  //Note: Y: Up/down,
  //Note: Z: Into/Outof (screen)

  //Sunlite air:
    colors[0] = vec3(.8);  //White
    norms[0]  = vec3(0,1,0);   //Up
  
  //Under-side:
//    rgb(35, 41, 51)
    colors[1] = vec3(0.13,0.16,0.20);
    norms[1]  = vec3(0,-1,0);
  
  //left-sunglowyness
    colors[2] = vec3(.7,0.5,0.25);
    norms[2]  = vec3(-1,0.5,-0.2);


  vec3 t1pos = cray.origin + (res.x)*cray.direction;
  vec3 t2pos = cray.origin + (res.y)*cray.direction;
  vec3 Norm1 = vec3(
    ((metaBall_func(t1pos + vec3(h,0,0))) - (metaBall_func(t1pos - vec3(h,0,0)))),
    ((metaBall_func(t1pos + vec3(0,h,0))) - (metaBall_func(t1pos - vec3(0,h,0)))),
    ((metaBall_func(t1pos + vec3(0,0,h))) - (metaBall_func(t1pos - vec3(0,0,h))))
  );
  Norm1 /= 2*h;

  return vec4(abs(t2pos-t1pos),0);


  vec3 curNorm = normalize(Norm1);

  vec4 colour = vec4(ambient,0);
  for(int i = 0; i < numColors; i++){
    float p = dot(curNorm,norms[i]);
    if(p > 0)
      colour += vec4( p* colors[i], 0);
  }

  #undef r

  return colour;
  
  // #define FANCEY
  #ifdef FANCEY
	vec3 hitpos = res.x * cray.direction + cray.origin;
	vec3 surface_norm  = get_surface_norm(hitpos, int(res.y));	
	
	/////////////////////////////UPDATE GLOBALS FOR REFLECTIONS//////////////
  ray newray;
  
	hitobj = int(res.y);
	newray.direction = normalize(reflect(cray.direction, surface_norm));
	newray.origin = hitpos + newray.direction*FUDGE;
	
	///////////////////BASIC SHADOWS////////////////////////////////
	//Iterates through each object to find lights, making check_light_hit rays and testing them as it goes.
	int lcnt=0;
	int scnt=0;
	vec4 stest;
	ray sray;
	check_light_hit = true;
	vec3 svect;
	float svlen;

	#ifdef hard_shadows
	
	for(int i=0; i < num_objs ; i++){
		if(obj_type(i) == T_LIGHT){
			lcnt++;
			svect = light_p(i) - hitpos;
			svlen = sqrt(dot(svect,svect));

			sray.direction = normalize(svect);
			sray.origin = hitpos + sray.direction * FUDGE;

			stest = test_objects_intersect(sray);

			if(int(obj_type(int(stest.y))) == T_LIGHT){				 //If it intersects a light.
				scnt++;
			}
			
		}
	}
//	if(scnt = 0)
//		c *= ((lcnt-scnt)/lcnt);		//Apply check_light_hits.
	c *= (lcnt/scnt);

	
	#else
	////////////////////Diffuse Lights/////////////////////

  
//	vec4 c_scaler = vec4(ambient,0);	//Set-up minimum of ambient.
//	vec4 p_scaler = vec4(0);
//	
//	check_light_hit = true;
//	if(hitobj != T_NONE && (1-obj_reflec(hitobj)) >= BEPSILON){
//	for(int i=0; i < num_objs ; i++){
//	if(obj_type(i) == T_LIGHT){
//		svect = light_p(i) - hitpos;
//		svlen = sqrt(dot(svect,svect));
//		sray.direction = normalize(svect);
//
//		sray.origin = hitpos + sray.direction *FUDGE;
//		stest = test_objects_intersect(sray);
//
///		if((int(obj_type(int(stest.y))) == T_LIGHT)){
//			//DIFFUSE
///			c_scaler += vec4((obj_colour(i)*max(0,dot(sray.direction,surface_norm))),0);
//			//PHONG
//			vec3 h = normalize((-cray.direction + sray.direction));
//			float a = max(0,(dot(h,surface_norm)));
//			p_scaler += vec4( pow(a,obj_phong(hitobj)) * obj_colour(i)  ,0  );
//
//		}
//		}
//	}
//	}
//	
//	c = (c * vec4(c_scaler)) + vec4(p_scaler);
//  */

	#endif
  #endif
	return(c);
}

////////////////////////////////// RAYTRACE STUFF ///////////////////////////////

vec4 main_c(){


 	vec4 pixel = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 colour = vec4(0);

  float z = -(1.f/tan(90.0/2.f));
  vec2 coords = vec2(vp.x, vp.y);

  ray newray;

	//BASIC	pos
	newray.direction = vec3(coords, -1/tan(FOV/2));	
	
  newray.origin = vec3(0,0,0);	
	//newray.origin = vec3(0.5,0,15);	

  mat4 t = cameraMatrix*modelviewMatrix;

	//TRANSFORMATIONS
  vec4 ot = t*vec4(newray.origin,1);

	newray.origin = vec3(ot.x,ot.y,ot.z);

  vec4 pt = t*vec4(newray.direction,0);
  newray.direction = vec3(pt.x,pt.y,pt.z);


	newray.direction = normalize(newray.direction);

	/////////////////INIT////////////////////////////////
	colour = rtrace(newray);


#ifdef FANCEY
	vec4 c2;
	float ref_pwr=1;

//#define stack_reflect 10

#ifndef stack_reflect
	//Iterative Reflection
//	#define reflect_by_num 2

	#ifdef reflect_by_num
	for(int i=0; i < reflect_by_num ; i ++){	//By fixed ammount
	#else
	while(ref_pwr >= BEPSILON){			//Dynamicly
	#endif
		c2 = rtrace(newray);			
		colour += c2 * (ref_pwr) * (1-obj_reflec(hitobj));
		ref_pwr*=obj_reflec(hitobj);
		if(hitobj == T_NONE)
			break;
	}
		
#else
	//Stack based-reflection. (Way slower)
	vec4 Cstack[stack_reflect];

	for(int i=0; i < stack_reflect ; i++){
		Cstack[i]=vec4(rtrace(newray).xyz,obj_reflec(hitobj));
	}
	for(int i=stack_reflect-1 ; i >= 0 ; i--){
		colour.xyz =  mix(Cstack[i].xyz, colour.xyz, Cstack[i].w); 
	}
	
#endif

  #endif
	//MOVES PARTICLES. NOTE: NO COLLISONS YET.
//	if(pixel_coords.y == 0 && pixel_coords.x <= num_objs && obj_type(pixel_coords.x) == T_PARTICLE){
//		obj_move(pixel_coords.x);
//	}
	if(ERROR ==0)
    return vec4(colour);
	else
	  	error_out();
}

