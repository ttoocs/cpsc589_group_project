//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

class MetaBall
{
public:
	vec3 pos;
	float radius = -1;
	//Must create functions to pass in to here.
    float (*m_surfaceFunction)(vec3, float);
    int id = -1;
	
	virtual float function(vec3 v) = 0;
	DefaultMetaBall(vec3 newPos, double radius, float(*f)(vec3, float))
	{
		pos = newPos;
		radius = radius;
		m_surfaceFunction = f;
	}
	float valueAt(vec3 loc)
	{
		return (*m_surfaceFunction)(loc,radius);
	}
};
