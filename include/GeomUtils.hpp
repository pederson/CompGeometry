#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

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

	template<size_t dim>
	double dot(const Point<dim> & p1, const Point<dim> & p2){
		double dt = 0.0;
		for (auto i=0; i<dim; i++) dt += p1.x[i]*p2.x[i];
		return dt;
	}

}





template <size_t dim>
struct Box{

	// empty constructor
	Box(){};

	// constructor
	Box(const Point<dim> & lopt, const Point<dim> & hipt)
	: lo(lopt), hi(hipt) {}

	// data
	Point<dim> lo, hi;

};

namespace Box{

	template<size_t dim>
	double dist(const Box<dim> & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return sqrt(dsq);
	}

	template<size_t dim>
	double distsq(const Box<dim> & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return dsq;
	}

	template<size_t dim>
	bool contains(const Box<dim> & bx, const Point<dim> & pt){
		return distsq(bx, pt) == 0;
	}

}




struct Plane{

	// empty constructor
	Plane()
	: origin(Point<3> (0,0,0)), normal(Point<3> (0,0,1), posx(Point<3> (1,0,0))) {};

	// constructor
	Plane(const Point<3> & p, const Point<3> & n, const Point<3> & px)
	: origin(p), normal(n), posx(px) {};

	// project a point onto the plane
	Point<2> project(const Point<3> & pt) const{
		Point<3> ptvec(pt.x[0]-origin.x[0], pt.x[1]-origin.x[1], pt.x[2]-origin.x[2]);
		double px = Point::dot(ptvec, posx);

		// the y direction is Z x X
		Point<3> posy(normal.x[1]*posx.x[2] - normal.x[2]*posx.x[1],
					  normal.x[2]*posx.x[0] - normal.x[0]*posx.x[2],
					  normal.x[0]*posx.x[1] - normal.x[1]*posx.x[0]);

		double py = Point::dot(ptvec, posy);
		return Point<2>(px, py);
	}

	// data
	Point<dim> origin, normal, posx;

};


typedef Plane(Point<3>(0,0,0),Point<3>(0,0,1),Point<3>(1,0,0)) XYPLANE;
typedef Plane(Point<3>(0,0,0),Point<3>(0,0,-1),Point<3>(0,1,0)) YXPLANE;
typedef Plane(Point<3>(0,0,0),Point<3>(1,0,0),Point<3>(0,1,0)) YZPLANE;
typedef Plane(Point<3>(0,0,0),Point<3>(-1,0,0),Point<3>(0,0,1)) ZYPLANE;
typedef Plane(Point<3>(0,0,0),Point<3>(0,1,0),Point<3>(0,0,1)) ZXPLANE;
typedef Plane(Point<3>(0,0,0),Point<3>(0,-1,0),Point<3>(1,0,0)) XZPLANE;




template <size_t dim>
struct Line{

	// empty constructor
	Line(){};

	// constructor
	Line(const Point<dim> & p, const Point<dim> & d)
	: pt(p), dir(d) {};

	// data
	Point<dim> pt, dir;

};



template <size_t dim>
struct Segment{

	// empty constructor
	Segment(){};

	// constructor
	Segment(const Point<dim> & b, const Point<dim> & e)
	: begin(b), end(e) {};

	virtual bool isLeft(const Point<2> & pt) const {
		return ((end.x[0]-begin.x[0])*(pt.x[1]-begin.x[1])
			  - (pt.x[0] - begin.x[0])*(end.x[1] - begin.x[1])) >= 0;
	}

	// data
	Point<dim> begin, end;

};


struct LineSegment : public Segment<2>{

	// empty constructor
	LineSegment(){};

	// constructor
	LineSegment(const Point<2> & b, const Point<2> & e)
	: Segment(b,e) {};

};


struct CircleSegment : public Segment<2>{

	// empty constructor
	CircleSegment(){};

	// constructor
	CircleSegment(const Point<2> & b, const Point<2> & e, double rad, bool leftrunning)
	: Segment(b,e) {};

	bool isLeft(const Point<2> & pt) const {
		if (begin.x[1] > end.x[1] && lrun) return true;
		else if (begin.x[1] < end.x[1] && ~lrun) return true;

		// now check the harder cases
	}

	double radius;
	bool lrun;	// if true, the circle segment runs from begin, to the left to end. 
};


#endif