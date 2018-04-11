
#include "lightning.h"

#include <random>

namespace lightning{

using namespace std;
using namespace glm;


float meter = 0.004;

float random_50_50()
{
	default_random_engine uni_gen;
	uniform_real_distribution<double> uni_distribution_50_50(0.0, 1.0);

	if (uni_distribution_50_50(uni_gen) >= 0.5)
		return 1.0;
	else
		return -1.0;
}


mat4 rotateAbout(vec3 axis, float radians)
{
	mat4 matrix;

	matrix[0][0] = cos(radians) + axis.x*axis.x*(1 - cos(radians));
	matrix[1][0] = axis.x*axis.y*(1 - cos(radians)) - axis.z*sin(radians);
	matrix[2][0] = axis.x*axis.z*(1 - cos(radians)) + axis.y*sin(radians);

	matrix[0][1] = axis.y*axis.x*(1 - cos(radians)) + axis.z*sin(radians);
	matrix[1][1] = cos(radians) + axis.y*axis.y*(1 - cos(radians));
	matrix[2][1] = axis.y*axis.z*(1 - cos(radians)) - axis.x*sin(radians);

	matrix[0][2] = axis.z*axis.x*(1 - cos(radians)) - axis.y*sin(radians);
	matrix[1][2] = axis.z*axis.y*(1 - cos(radians)) + axis.x*sin(radians);
	matrix[2][2] = cos(radians) + axis.z*axis.z*(1 - cos(radians));

	return matrix;
}


//This assumes ground is at z = 0
void trace_lightning_recursion(vec3 init_point, vec3 init_direction, vector<Segment> *storage, int num_segs, float max_h, int recursion_depth)
{
	default_random_engine norm_gen, uni_gen;
	normal_distribution<double> norm_distribution(45.0, 20.0);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 60.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 150);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));

	while ((current_point.y > 0.0) && (num_segs > 0))
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		rand_segment = uni_distribution_length(uni_gen) * rand_segment;

		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length

		s.p1 = current_point + rand_segment;
		current_point = s.p1; //current_point + rand_segment;
		
		storage->push_back(s);
		
		if (uni_distribution_branch(uni_gen) <= ((0.1*pow(10, -(current_point.y/max_h))) / 100.0*recursion_depth))
		{
			vec3 new_dir = rotateAbout(T, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* rotateAbout(N, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* rotateAbout(B, radians(random_50_50() * uni_distribution_new_dir(uni_gen)))
							* vec4(rand_segment, 0.0);
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, recursion_depth + 1);
		}
		num_segs--;
	}
}


float uni_distribution(float min, float max, unsigned seed)
{
	default_random_engine uni_gen(seed);
	uniform_real_distribution<double> uni_distribution(min, max);

	return uni_distribution(uni_gen);
}

void trace_lightning(vec3 init_point, vec3 init_direction, vector<Segment> *storage, float max_h)
{
  std::random_device rd; 
	unsigned seed = rd();//glfwGetTime();
	default_random_engine norm_gen(seed), uni_gen(seed);
	normal_distribution<double> norm_distribution(45.0, 20.0);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 45.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 150);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));
	
	
	
	while (current_point.y > 0.0)
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		rand_segment = uni_distribution_length(uni_gen) * normalize(rand_segment);

		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length
		
		s.p1 = current_point + rand_segment;
		current_point = s.p1; //current_point + rand_segment;
		
		storage->push_back(s);
		
		//std::cout << "s:\t" << vPrint(s.p0) << "\t" << vPrint(s.p1) << std::endl;

		if (uni_distribution_branch(uni_gen) <= (0.1*pow(10, -(current_point.y/max_h))))
		{
			vec3 new_dir = rotateAbout(T, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * rotateAbout(N, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * vec4(rand_segment, 0.0);
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, init_point.y);
		}
	}
}

};
