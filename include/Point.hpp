#ifndef _POINT_H
#define _POINT_H

#include <iostream>
#include <sstream>
#include <math.h>
#include <vector>
#include <array>

namespace csg{

template <std::size_t dim, class T>
class GeneralPoint{
public:
	// data
	T x[dim];

	constexpr std::size_t size() const {return dim;}; 

	// default constructor
	GeneralPoint(){};

	// constructor
	GeneralPoint(T x0){
		static_assert(dim == 1, "ERROR: That GeneralPoint constructor not implemented for dim > 1");
		x[0] = x0;
	}

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

	// division
	GeneralPoint operator/ (T val) const{
		GeneralPoint out(*this);
		for (auto i=0; i<dim; i++) out.x[i] = x[i]/val;
		return out;
	}

	// scalar addition 
	GeneralPoint operator+ (T val) const{
		GeneralPoint out(*this);
		for (auto i=0; i<dim; i++) out.x[i] + val;
		return out;
	}

	// scalar subtraction 
	GeneralPoint operator- (T val) const{
		GeneralPoint out(*this);
		for (auto i=0; i<dim; i++) out.x[i] -= val;
		return out;
	}

	double norm() const{
		double magn = 0;
		for (auto i=0; i<dim; i++) magn += x[i]*x[i];
		return sqrt(magn);
	}

	GeneralPoint normalize() const{
		double magn = norm();
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

	// print to an ostream
	template<std::size_t d, class t>
	friend std::ostream & operator<<(std::ostream & os, const GeneralPoint<d, t> & p);


	// read from an istream
	template<std::size_t d, class t>
	friend std::istream & operator>>(std::istream & os, GeneralPoint<d, t> & p);

};

template<std::size_t dim, class T>
std::ostream & operator<<(std::ostream & os, const GeneralPoint<dim, T> & p){
	os << "(" ;
	for (auto i=0; i< dim-1; i++) os << p.x[i] << ", " ;
	os << p.x[dim-1] << ")" ;
	
	return os;
}


template<std::size_t dim, class T>
std::istream & operator>>(std::istream & is, GeneralPoint<dim, T> & p){
	is.get(); // reads the opening parenthesis
	std::stringstream strbuff;
	char lin[200];
	is.get(lin, 200, ')');
	strbuff << lin ;

   	std::array<char, 30> a;
   	int i=0;
    while(strbuff.getline(&a[0], 30, ',')){
    	std::stringstream item;
    	item << &a[0];
    	item >> p.x[i];
    	i++;
    }
	return is;
}


template<std::size_t dim, class T>
GeneralPoint<dim, T> operator*(T val, const GeneralPoint<dim, T> & p){
	GeneralPoint<dim, T> out(p);
	for (auto i=0; i<dim; i++) out.x[i] = val*p.x[i];
	return out;
}




// specializations/overloads
template<> GeneralPoint<2,double>::GeneralPoint(double x0, double x1){
	x[0] = x0; x[1] = x1;
}

template<> GeneralPoint<3,double>::GeneralPoint(double x0, double x1, double x2){
	x[0] = x0; x[1] = x1; x[2] = x2;
}

template<> GeneralPoint<1,int>::GeneralPoint(int x0){
	x[0] = x0;
}

template<> GeneralPoint<2,int>::GeneralPoint(int x0, int x1){
	x[0] = x0; x[1] = x1;
}

template<> GeneralPoint<3,int>::GeneralPoint(int x0, int x1, int x2){
	x[0] = x0; x[1] = x1; x[2] = x2;
}

// return double when double arithmetic is performed on int
template<std::size_t dim> 
GeneralPoint<dim, double> operator*(double val, const GeneralPoint<dim, int> & p){
	GeneralPoint<dim, double> out;
	for (auto i=0; i<dim; i++) out.x[i] = val*p.x[i];
	return out;
}

template<std::size_t dim> 
GeneralPoint<dim, double> operator*(const GeneralPoint<dim, double> & pd, const GeneralPoint<dim, int> & pi){
	GeneralPoint<dim, double> out;
	for (auto i=0; i<dim; i++) out.x[i] = pd.x[i]*pi.x[i];
	return out;
}

template<std::size_t dim> 
GeneralPoint<dim, int> operator%(const GeneralPoint<dim, int> & p, std::size_t m){
	GeneralPoint<dim, int> out;
	for (auto i=0; i<dim; i++) out.x[i] = p.x[i]%m;
	return out;
}


// add template typedefs for special GeneralPoint types
template<std::size_t dim>
using Point = GeneralPoint<dim, double>;

template<std::size_t dim>
using IntPoint = GeneralPoint<dim, int>;

using Point2 = Point<2>;
using Point3 = Point<3>;
using IntPoint2 = IntPoint<2>;
using IntPoint3 = IntPoint<3>;





inline Point3 cross(Point3 p1, Point3 p2){
	return Point3(p1.x[1]*p2.x[2]-p1.x[2]*p2.x[1], p1.x[2]*p2.x[0]-p1.x[0]*p2.x[2], p1.x[0]*p2.x[1]-p1.x[1]*p2.x[0]);
}

inline double cross(Point2 p1, Point2 p2){
	return p1.x[0]*p2.x[1]-p1.x[1]*p2.x[0];
}


}

#endif