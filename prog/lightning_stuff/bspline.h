//#include "include/glad/glad.h"
//#include "include/GLFW/glfw3.h"
//#include "include/glm/glm.hpp"
//#include <vector>
#pragma once
using namespace std;
using namespace glm;

class BSpline{
	
	private:
		int k = 3; 	// order of b-spline
		int u_steps = 10; 	// resolution of b-spline (higher is better, results in smaller
							// increments)
		float point_radius = 0.05;
		
		vector<float> computeStandardKnotSequence();
		
		
		int computeDelta(vector<float> knot_sequence, float u);
		

		void computeBSplinePoint(vector<float> knot_sequence, int delta, float u);

	public:
		vector<float> control_points; 	// control points
		vector<float> bspline_points; 	// points on the physical b_spline
		
		vector<vec3> control_vecs;
		vector<vec3> bspline_vecs;
		
		int num_point_vertices; // number of control points
		int num_spline_vertices; // number of points to render b-spline
	

		BSpline();
		
		void loadBSpline();
	
		
		void loadControlPoints();
		
		void addPoint(vec3 pos);
	
		void deletePoint(vec3 pos);
		
		void movePoint(vec3 pos);
		
		void incOrder();
		
		void decOrder();
		
		void decResolution();
		
		void incResolution();
		
};
