
#include "lightning.h"
#include "bspline.cpp"

#include <iostream>
#include <random>

#define GL_SHADER_STORAGE_BUFFER 0x90D2
	#define vPrint(X)   "(" << X.x << "," <<  X.y << "," << X.z << ")"

using namespace std;
using namespace glm;

Plane plane;

float lightning::meter = 0.004;
GLuint lightning::segmentBuffer =0;


float lightning::random_50_50()
{
	return pow(-1.0, rand() % 2);
}


mat4 lightning::rotateAbout(vec3 axis, float radians)
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
void lightning::trace_lightning_recursion(vec3 init_point, vec3 init_direction, vector<Segment> *storage, int num_segs, float max_h, int recursion_depth, Plane pl)
{
	default_random_engine norm_gen, uni_gen;
	normal_distribution<double> norm_distribution(16.0, 0.1);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 60.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 50);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));

	while ((dot(pl.normal, (current_point - pl.point)) > 0) && (num_segs > 0))
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		rand_segment *= ( uni_distribution_length(uni_gen));

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
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, recursion_depth + 1, pl);
		}
		num_segs--;
	}
}


float lightning::uni_distribution(float min, float max, unsigned seed)
{
	default_random_engine uni_gen(seed);
	uniform_real_distribution<double> uni_distribution(min, max);

	return uni_distribution(uni_gen);
}

void lightning::trace_lightning(vector<vec3> targets, vector<Segment> *storage, float max_h)
{
	std::random_device rd; 
	unsigned seed = rd();//glfwGetTime();
	default_random_engine norm_gen(seed), uni_gen(seed);
	normal_distribution<double> norm_distribution(16.0, 0.1);
	uniform_real_distribution<double> uni_distribution_length(meter, 10.0*meter);
	uniform_real_distribution<double> uni_distribution_branch(0.0, 1.0);
	uniform_real_distribution<double> uni_distribution_new_dir(0.0, 45.0);
	uniform_int_distribution<int> uni_distribution_segs(10, 50);

	vec3 rand_segment = vec3(0, 0, 0);
	vec3 current_point = targets[0];
	vec3 init_direction = normalize(targets[1] - targets[0]);
	double rand_angle = 0.0f;
	double angle;

	vec3 T = normalize(init_direction);
	vec3 N = normalize(cross(init_direction, vec3(0.0, 1.0, 0.0)));
	vec3 B = normalize(cross(T, N));
	
	float endDistance = 0.0;
	for (int i = 0; i  < targets.size() - 1; i++)
	{
		endDistance += length(targets[i + 1] - targets[i]);
	}
	
	float accumDistance = 0.0;
	int last_index = 0;
	float thresholdDistance = length(targets[last_index + 1] - targets[last_index]);
	
	while (accumDistance < endDistance)
	{
		Segment s;
		s.p0 = current_point;
		//START: Random angles, get rand segment
		rand_angle = norm_distribution(norm_gen);
		
		//cout << rand_angle << endl;
		//cout << random_50_50() << endl;
		rand_segment = rotateAbout(T, radians(random_50_50() * rand_angle)) * rotateAbout(N, radians(random_50_50() * rand_angle)) * vec4(init_direction, 0.0);
		//rand_segment = uni_distribution_length(uni_gen) * normalize(rand_segment);
		rand_segment = normalize(rand_segment);
		rand_segment *= ( uni_distribution_length(uni_gen));

		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length
		
		s.p1 = current_point + rand_segment;
		vec3 dir = (targets[last_index + 1] - targets[last_index]);
		
		accumDistance += /*length(rand_segment);*/length(dot(rand_segment,dir) / dot(dir, dir) * dir);
		//cout << accumDistance << endl;
		if (accumDistance >= thresholdDistance)
		{
			last_index++;
			//init_direction = normalize(targets[last_index + 1] - targets[last_index]);
			thresholdDistance = thresholdDistance + length(targets[last_index + 1] - targets[last_index]);
		}
		init_direction = normalize(targets[last_index + 1] - current_point);

		current_point = s.p1; //current_point + rand_segment;
		
		storage->push_back(s);
		
		//std::cout << "s:\t" << vPrint(s.p0) << "\t" << vPrint(s.p1) << std::endl;
		
		/*if (uni_distribution_branch(uni_gen) <= (0.1*pow(10, -(current_point.y/max_h))))
		{
			vec3 new_dir = rotateAbout(T, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * rotateAbout(N, radians(random_50_50() * uni_distribution_new_dir(uni_gen))) * vec4(rand_segment, 0.0);
			trace_lightning_recursion(current_point, normalize(new_dir), storage, uni_distribution_segs(uni_gen), current_point.y, 0, plane);
		}*/
	}
}



void lightning::loadPoints(BSpline spline)
{
  if(lightning::segmentBuffer == 0){
    std::cout << "Note: Lightening SSBO buffer is 0.. Attempting to auto-generate it." << std::endl;
    std::cout << "May not work, assumes VAO is already bounded. If blank this is why." << std::endl;    
//	  glBindVertexArray(VAO);
  	glGenBuffers(1, &lightning::segmentBuffer);
  	glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,lightning::segmentBuffer);

  }
	/*
	BSpline spline = BSpline();
	spline.addPoint(vec3(-1.0, 0.0, 0.0));
	spline.addPoint(vec3(0.0, 1.0, 0.0));
	spline.addPoint(vec3(1.0, 0.0, 0.0));
	spline.loadBSpline();
	*/
	cout << spline.bspline_vecs.size() << endl;
	/*
	vector<vec3> lightning_targets;
	lightning_targets = spline.bspline_vecs;
	*/
	std::vector<Segment> lightning_segs;
	
	lightning_segs.clear();
	lightning::trace_lightning(spline.bspline_vecs, &lightning_segs, 2.0);
	//lightning::trace_lightning(vec3(0.0, 2.0, 0.0), vec3(-1.0, 0.0, 0.0), &lightning_segs, 2.0);

	/*
	for (int i = 0; lightning_targets.size() - 1; i++)
	{
		lightning::trace_lightning(lightning_targets[i], lightning_targets[i + 1], &lightning_segs, 2.0);
	}
	*/
	cout << lightning_segs.size() << endl;
	cout << vPrint(lightning_segs[0].p0) << endl;
	float temp[((lightning_segs.size() * 2 * 4) + 4)];
	for (int i = 0; i < lightning_segs.size(); i++)
	{
		temp[i * 8 + 4] = lightning_segs[i].p0.x;
		temp[i * 8 + 4 + 1] = lightning_segs[i].p0.y;
		temp[i * 8 + 4 + 2] = lightning_segs[i].p0.z;
		temp[i * 8 + 4 + 3] = 0;
		
		temp[i * 8 + 4 + 4] = lightning_segs[i].p1.x;
		temp[i * 8 + 4 + 5] = lightning_segs[i].p1.y;
		temp[i * 8 + 4 + 6] = lightning_segs[i].p1.z;
		temp[i * 8 + 4 + 7] = 0;
	}


	//for (int i = 2; i < lightning_segs.size(); i++){
//		if( lightning_segs[i] != lightning_segs[i-2] || lightning_segs[i] != lightning_segs[i-1] )
//			std::cout << "Diff:\t" <<  vPrint(lightning_segs[i]) << "\t" << vPrint(lightning_segs[i-2]) << std::endl;
		//cout << vPrint(lightning_segs[i].p0) << "\t" << vPrint(lightning_segs[i].p1) << "\n" << endl;
	//}

    temp[0] = lightning_segs.size() * 2;
  /*
	int lightningLoc = glGetUniformLocation(program, "lightning_segs");
	glUniform3fv(lightningLoc, lightning_segs.size(), temp);

	int numSegsLoc = glGetUniformLocation(program, "numSegs");
	glUniform1i(numSegsLoc, lightning_segs.size());

  */
	//cout << lightning_segs.size() << endl;
 /* //Working example via testing colors:
int OFF=4;
temp[OFF+0] = 0;
temp[OFF+1] = 1.0;
temp[OFF+2] = -2.0;

temp[OFF+3] = 0; //

temp[OFF+4] = -1.0;
temp[OFF+5] = 0;
temp[OFF+6] = -2;

temp[OFF+7] = 1; //??

temp[OFF+8] = -1;
temp[OFF+9] = 0;
temp[OFF+10] = -2;

temp[OFF+11] = 0;// ??

temp[OFF+12] = 0;
temp[OFF+13] = -1;
temp[OFF+14] = -2;
// */
  //EX: a single sphere:
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightning::segmentBuffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER,sizeof(temp),&temp,GL_DYNAMIC_COPY);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
	

}

