#pragma once

//main.h


#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include <Eigen/Dense>


#include "types.h"
#include "gl_helpers.h"
#include "shapes.h"
#include "input.h"
#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
//Used for perspective matrix.


//#define WIREFRAME
#define DEBUG

#ifdef DEBUG
	#define DEBUGMSG	printf("\n\n\t\t DEBUG MESSAGE AT LINE:\t%d\t In file:\t%s\n\n",__LINE__,__FILE__);
#else
	#define DEBUGMSG	;
#endif


#define torad(X)	((float)(X*PI/180.f))
