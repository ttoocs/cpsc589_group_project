//Scott Saunders
// March 2018

//A simple center-difference numerical approximation of a derivative.
#pragma once
#include <functional>

namespace numerical{
  double numericalDeriv1D(double u, std::function<double(double)> f);
  void numericalDeriv3D(double xin, double yin, double zin, double (*f)(double x ,double y,double z ), double &x, double&y, double&z);
};
