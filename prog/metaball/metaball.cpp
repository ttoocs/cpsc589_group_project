//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.

#include "metaball.h"

MetaBall::MetaBall(vec3 newPos, double radius, float(*f)(vec3, float))
	{
		pos = newPos;
		radius = radius;
		m_surfaceFunction = f;
	}

float MetaBall::valueAt(vec3 loc)
	{
		return (*m_surfaceFunction)(loc,radius);
	}
