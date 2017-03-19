#ifndef _POINT_H
#define _POINT_H

#include <math.h>
#include <vector>

namespace csg{

template <std::size_t dim, class T>
class GeneralPoint{
public:
	// data
	T x[dim];

	// default constructor
	GeneralPoint(){};

	// constructor
	GeneralPoint(T x0, T x1){
		x[0] = x0;
		if (dim > 1) x[1] = x1;
		if (dim > 2) throw("ERROR: That GeneralPoint constructor not implemented for dim > 3");
	}

	// // constructor
	GeneralPoint(T x0, T x1, T x2){
		x[0] = x0;
		if (dim > 1) x[1] = x1;
		if (dim > 2) x[2] = x2;
		if (dim > 3) throw("ERROR: That GeneralPoint constructor not implemented for dim > 3");
	}

	// constructor
	GeneralPoint(std::vector<T> xin){
		for (auto i=0; i<xin.size(); i++) x[i] = xin[i];
	}

	// copy constructor
	GeneralPoint(const GeneralPoint & p){
		for (auto i=0; i<dim; i++) x[i] = p.x[i];
	}

	// assignment
	GeneralPoint & operator= (const GeneralPoint & p){
		for (auto i=0; i<dim; i++) x[i] = p.x[i];
		return *this;
	}

	// addition
	GeneralPoint operator+ (const GeneralPoint & p) const{
		GeneralPoint out(p);
		for (auto i=0; i<dim; i++) out.x[i] = x[i] + p.x[i];
		return out;
	}

	// subtraction
	GeneralPoint operator- (const GeneralPoint & p) const{
		GeneralPoint out(p);
		for (auto i=0; i<dim; i++) out.x[i] = x[i] - p.x[i];
		return out;
	}

	// scalar multiplication
	GeneralPoint operator* (T val) const{
		GeneralPoint out(*this);
		for (auto i=0; i<dim; i++) out.x[i] = val*x[i];
		return out;
	}

	GeneralPoint normalize() const{
		double magn = 0;
		for (auto i=0; i<dim; i++) magn += x[i]*x[i];
		magn = sqrt(magn);
		return 1.0/magn*(*this);
	}

	// comparison
	bool operator== (const GeneralPoint & p) const {
		for (auto i=0; i<dim; i++) if (x[i] != p.x[i]) return false;
		return true;
	}

	static double dist(const GeneralPoint & p1, const GeneralPoint & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return sqrt(dsq);
	}

	static double distsq(const GeneralPoint & p1, const GeneralPoint & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return dsq;
	}

	static double dot(const GeneralPoint & p1, const GeneralPoint & p2){
		double dt = 0.0;
		for (auto i=0; i<dim; i++) dt += p1.x[i]*p2.x[i];
		return dt;
	}

	// print to std::out
	template<std::size_t d, class t>
	friend std::ostream & operator<<(std::ostream & os, const GeneralPoint<d, t> & p);

};

template<std::size_t dim, class T>
std::ostream & operator<<(std::ostream & os, const GeneralPoint<dim, T> & p){
	os << "(" ;
	for (auto i=0; i< dim-1; i++) os << p.x[i] << ", " ;
	os << p.x[dim-1] << ")" ;
	
	return os;
}

template<std::size_t dim, class T>
GeneralPoint<dim, T> operator*(T val, const GeneralPoint<dim, T> & p){
	GeneralPoint<dim, T> out(p);
	for (auto i=0; i<dim; i++) out.x[i] = val*p.x[i];
	return out;
}


// // add specialization for doubles
// template<std::size_t dim>
// class GeneralPoint<dim, double> {
// public:

// 	// constructor
// 	GeneralPoint(double x0=0.0, double x1=0.0, double x2=0.0){
// 		x[0] = x0;
// 		if (dim > 1) x[1] = x1;
// 		if (dim > 2) x[2] = x2;
// 		if (dim > 3) throw("ERROR: That GeneralPoint constructor not implemented for dim > 3");
// 	}

// 	// normalization
// 	GeneralPoint normalize() const{
// 		double magn = 0;
// 		for (auto i=0; i<dim; i++) magn += x[i]*x[i];
// 		magn = sqrt(magn);
// 		return 1.0/magn*(*this);
// 	}
// };


// // add specialization for ints
// template<std::size_t dim>
// class GeneralPoint<dim, int> {
// public:
// 	int x[dim];

// 	// constructor
// 	GeneralPoint(int x0=0.0, int x1=0.0, int x2=0.0){
// 		x[0] = x0;
// 		if (dim > 1) x[1] = x1;
// 		if (dim > 2) x[2] = x2;
// 		if (dim > 3) throw("ERROR: That GeneralPoint constructor not implemented for dim > 3");
// 	}

// };


template<> GeneralPoint<2,double>::GeneralPoint(double x0, double x1){
	x[0] = x0; x[1] = x1;
}

template<> GeneralPoint<3,double>::GeneralPoint(double x0, double x1, double x2){
	x[0] = x0; x[1] = x1; x[2] = x2;
}


// add template typedefs for special GeneralPoint types
template<std::size_t dim>
using Point = GeneralPoint<dim, double>;

template<std::size_t dim>
using IntPoint = GeneralPoint<dim, double>;

using Point2 = Point<2>;
using Point3 = Point<3>;
using IntPoint2 = IntPoint<2>;
using IntPoint3 = IntPoint<3>;





inline Point3 cross(Point3 p1, Point3 p2){
	return Point3(p1.x[1]*p2.x[2]-p1.x[2]*p2.x[1], p1.x[2]*p2.x[0]-p1.x[0]*p2.x[2], p1.x[0]*p2.x[1]-p1.x[1]*p2.x[0]);
}


}

#endif