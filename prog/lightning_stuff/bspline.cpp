//#include "include/glad/glad.h"
//#include "include/GLFW/glfw3.h"
//#include "include/glm/glm.hpp"
//#include <vector>

using namespace std;
using namespace glm;

		vector<float> BSpline::computeStandardKnotSequence()
		{
			int m = control_points.size() / 3;
			// Type cast to floats to do division
			float float_k = (float) k;
			float float_m = (float) m;

			vector<float> knot_sequence = vector<float>();
			// Triple knot at 0
			for (int i = 0; i < k; i++)
			{
				knot_sequence.push_back(0.0);
			}

			// Intermediate knot values
			for (float i = (1.0 / (float_m - float_k + 2.0)); i < 1.0; i += ( 1.0 / (float_m - float_k + 2.0)))
			{
				knot_sequence.push_back(i);
			}

			// Triple knot at 1
			for (int i = 0; i < k; i++)
			{
				knot_sequence.push_back(1.0);
			}

			return knot_sequence;
		}
		
		int BSpline::computeDelta(vector<float> knot_sequence, float u)
		{
			int m = control_points.size() / 3;
			for (int i = 0; i < (m + k); i++)
			{
				if ((u >= knot_sequence[i]) && (u < knot_sequence[i + 1]))
				return i;
			}
			return -1;
		}

		void BSpline::computeBSplinePoint(vector<float> knot_sequence, int delta, float u)
		{
			vector<float> points_temp;
			for (int i = 0; i < k; i++)
			{
				if ((delta - i) * 3 >= control_points.size())
				{
					points_temp.push_back(control_points[control_points.size() - 3]);
					points_temp.push_back(control_points[control_points.size() - 2]);
					points_temp.push_back(control_points[control_points.size() - 1]);
				}
				else if((delta - i) * 3 < 0)
				{
					points_temp.push_back(0.0);
					points_temp.push_back(0.0);
					points_temp.push_back(0.0);
				}
				else
				{
					points_temp.push_back(control_points[(delta - i) * 3]);
					points_temp.push_back(control_points[((delta - i) * 3) + 1]);
					points_temp.push_back(control_points[((delta - i) * 3) + 2]);
				}
			}
			int i;
			float w;
			for (int r = k; r > 1; r--) 
			{
				i = delta;
				for (int s = 0; s < (r - 1); s++)
				{
					w = (u - knot_sequence[i]) / (knot_sequence[i + r - 1] - knot_sequence[i]);
					points_temp[s * 3] = w * points_temp[s * 3] + (1 - w)*points_temp[(s * 3) + 3];
					points_temp[(s * 3) + 1] = w * points_temp[(s * 3) + 1] + (1 - w)*points_temp[(s * 3) + 4];
					points_temp[(s * 3) + 2] = w * points_temp[(s * 3) + 2] + (1 - w)*points_temp[(s * 3) + 5];
					i--;
				}
			}

			bspline_points.push_back(points_temp[0]);
			bspline_points.push_back(points_temp[1]);
			bspline_points.push_back(points_temp[2]);
		}

		BSpline::BSpline(){
			control_points = vector<float>();
			bspline_points = vector<float>();
			num_point_vertices = 0;
		}
		
		void BSpline::loadBSpline()
		{
			vector<vec3> vertices;
			
			bspline_points.clear();
			
			int m = control_points.size() / 3;

			// If there is not enough control points, return
			if (!((m - k) < -1))
			{
				vector<float> knot_sequence = computeStandardKnotSequence();
				int delta;

				for (float u = 0.0; u <= 1.0; u += (1.0 / (float)u_steps))
				{
					delta = computeDelta(knot_sequence, u);
					computeBSplinePoint(knot_sequence, delta, u);
				}
			}
			
			for (int i = 0; i < bspline_points.size(); i += 3) 
			{
				bspline_vecs.push_back(vec3(bspline_points[i],
								   bspline_points[i + 1],
								   bspline_points[i + 2]));
			}
			/*
			glBindVertexArray(splineVAO);

			glBindBuffer(GL_ARRAY_BUFFER, splineVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			num_spline_vertices = vertices.size();
			*/
		}
		
		void BSpline::loadControlPoints()
		{
			vector<vec3> vertices;
			
			for (int i = 0; i < control_points.size(); i += 3) 
			{
				for (float t = 0.0; t < (2 * 3.1415); t += (2 * 3.1415 / 101))
				{
					vertices.push_back(vec3(control_points[i] + point_radius*cos(t),
											control_points[i + 1] + point_radius*sin(t),
											control_points[i + 2]));
				}
			}
			
			control_vecs = vertices;
			/*
			glBindVertexArray(pointsVAO);

			glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			num_point_vertices = vertices.size();
			*/
		}
		
		void BSpline::addPoint(vec3 pos)
		{
			control_points.push_back(pos.x);
			control_points.push_back(pos.y);
			control_points.push_back(pos.z);
		}
	
		void BSpline::deletePoint(vec3 pos)
		{
			vec3 click_pos = pos;
			vec3 point_vec;
			float distance;

			int closest_point_index = -1;
			float closest_point_dist;

			for (int i = 0; i < control_points.size(); i += 3)
			{
				point_vec = vec3(control_points[i], control_points[i + 1], control_points[i + 2]);

				distance = length(point_vec - click_pos);

				if (distance < point_radius)
				{
					if (closest_point_index == -1)
					{
						closest_point_index = i;
						closest_point_dist = distance;
					}
					else if (distance < closest_point_dist)
					{
						closest_point_index = i;
						closest_point_dist = distance;
					}
				}

			}

			if (closest_point_index != -1)
			{
				control_points.erase(control_points.begin() + closest_point_index, control_points.begin() + closest_point_index + 3);
			}
		}
		
		void BSpline::movePoint(vec3 pos)
		{
			vec3 click_pos = pos;
			vec3 point_vec;
			float distance;

			int closest_point_index = -1;
			float closest_point_dist;

			for (int i = 0; i < control_points.size(); i += 3)
			{
				point_vec = vec3(control_points[i], control_points[i + 1], control_points[i + 2]);

				distance = length(point_vec - click_pos);

				if (distance < point_radius)
				{
					if (closest_point_index == -1)
					{
						closest_point_index = i;
						closest_point_dist = distance;
					}
					else if (distance < closest_point_dist)
					{
						closest_point_index = i;
						closest_point_dist = distance;
					}
				}

			}

			if (closest_point_index != -1)
			{
				control_points[closest_point_index] = pos.x;
				control_points[closest_point_index + 1] = pos.y;
				control_points[closest_point_index + 2] = pos.z;
			}
		}
		
		void BSpline::incOrder()
		{
			k++;
		}
		
		void BSpline::decOrder()
		{
			if (k > 2)
			{
				k--;
			}
		}
		
		void BSpline::decResolution()
		{
			if ((u_steps / 2) >= 10)
			{
				u_steps = u_steps / 2;
			}
		}
		
		void BSpline::incResolution()
		{
			u_steps = u_steps * 2;
		}

