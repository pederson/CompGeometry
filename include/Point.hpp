#ifndef _POINT_H
#define _POINT_H

#include <iostream>
#include <math.h>

template <size_t dim>
struct Point{

	// constructor
	Point(double x0=0.0, ,double x1=0.0, double x2=0.0){
		x[0] = 0;
		if (dim > 1) x[1] = x1;
		if (dim > 2) x[2] = x2;
		if (dim > 3) throw("ERROR: That Point constructor not implemented for dim > 3");
	}

	// constructor
	Point(vector<double> xin){
		for (auto i=0; i<xin.size(); i++) x[i] = xin[i];
	}

	// copy constructor
	Point(const Point & p){
		for (auto i=0; i<dim; i++) x[i] = p.x[i];
	}

	// assignment
	Point & operator= (const Point & p){
		for (auto i=0; i<dim; i++) x[i] = p.x[i];
		return *this;
	}

	// comparison
	bool operator== (const Point & p) const {
		for (auto i=0; i<dim; i++) if (x[i] != p.x[i]) return false;
		return true;
	}

	// print to std::out
	friend ostream& operator<<(ostream& os, const Point<dim> & p);

	// data
	double x[dim];

};

template<size_t dim>
ostream& operator<<(ostream& os, const Point<dim> & p){
	os << "(" ;
	for (auto i=0; i< dim-1; i++) os << p.x[i] << ", " ;
	os << p.x[dim-1] << ")" ;
	
	return os;
}

namespace Point{

	template<size_t dim>
	double dist(const Point<dim> & p1, const Point<dim> & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return sqrt(dsq);
	}

	template<size_t dim>
	double distsq(const Point<dim> & p1, const Point<dim> & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return dsq;
	}

}


#endif