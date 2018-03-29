//Cory Jensen
//This is based entirely off of Eds code.  I was just trying to 
//generalize it more.

//Scott Saunders
// Moved the .cpp stuff out of the .h stuff.
// Moved the meta-ball functs out of main and put here instead.
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

//START: From Eds file
float accumMetaBallFuncs(vec3 point)
{
	float accum = 0.0;
	for (int i = 0; i < metaballs.size(); i++)
	{
		accum += metaballs[i].function(point);
	}
	return accum;
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

