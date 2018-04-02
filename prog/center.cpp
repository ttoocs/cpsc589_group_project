//Scott Saunders
// March 2018

//A simple center-difference numerical approximation of a derivative.

#include "center.h"
#include <functional>

namespace numerical{
  double NAEpsilon = 0.00001;
  double numericalDeriv1D(double u, std::function<double(double)> f){
    //Just a simple center-difference formula for approxmating the derivative.
    double a = u-NAEpsilon;
    double b = u+NAEpsilon;
    double numer = f(b) - f(a);
    double denom = 2*NAEpsilon;
    return numer/denom;
  }

void numericalDeriv3D(double xin, double yin, double zin, double (*f)(double x ,double y,double z ), double &x, double&y, double&z){
    //It then uses those to find the derivative in that dimention.
    auto xf = [f,yin,zin] (double x) -> double { return f(x,yin,zin); };
    auto yf = [f,xin,zin] (double y) -> double { return f(xin,y,zin); };
    auto zf = [f,xin,yin] (double z) -> double { return f(xin,yin,z); };
    double dx = numericalDeriv1D(xin,xf);
    double dy = numericalDeriv1D(yin,yf);
    double dz = numericalDeriv1D(zin,zf);
    x=dx; y=dy; z=dz;
  }

};

#ifdef CENTER_STANDALONE
#include <iostream>

//COMPILE LIKE SO (on linux): ` g++ center.cpp -D CENTER_STANDALONE `

double implicitSphere(double x, double y, double z){
  return x*x + y*y + z*z - 1.0;
}


int main(){
  double x,y,z;
  double dx,dy,dz;
  std::cout << "Enter a position" << std::endl;
  std::cin >> x >> y >> z;
  numerical::numericalDeriv3D(x,y,z,implicitSphere,dx,dy,dz);
  std::cout << "Pos:\t" << "(" << x << "," << y << "," << z << ")" << std::endl;
  std::cout << "Deriv:\t" << "(" << dx << "," << dy << "," << dz << ")" << std::endl;

}

#endif
