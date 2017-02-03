#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include <iostream>
#include <math.h>
#include <vector>

// Constructive Solid Geometry operations
enum Operation {UNION, INTERSECT, DIFFERENCE, XOR};

template <std::size_t dim>
struct Point{
	// data
	double x[dim];

	// constructor
	Point(double x0=0.0, double x1=0.0, double x2=0.0){
		x[0] = x0;
		if (dim > 1) x[1] = x1;
		if (dim > 2) x[2] = x2;
		if (dim > 3) throw("ERROR: That Point constructor not implemented for dim > 3");
	}

	// constructor
	Point(std::vector<double> xin){
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

	// addition
	Point operator+ (const Point & p) const{
		Point out(p);
		for (auto i=0; i<dim; i++) out.x[i] = x[i] + p.x[i];
		return out;
	}

	// subtraction
	Point operator- (const Point & p) const{
		Point out(p);
		for (auto i=0; i<dim; i++) out.x[i] = x[i] - p.x[i];
		return out;
	}

	// scalar multiplication
	Point operator* (double val) const{
		Point out(*this);
		for (auto i=0; i<dim; i++) out.x[i] = val*x[i];
		return out;
	}

	Point normalize() const{
		double magn = 0;
		for (auto i=0; i<dim; i++) magn += x[i]*x[i];
		magn = sqrt(magn);
		return 1.0/magn*(*this);
	}

	// comparison
	bool operator== (const Point & p) const {
		for (auto i=0; i<dim; i++) if (x[i] != p.x[i]) return false;
		return true;
	}

	static double dist(const Point & p1, const Point & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return sqrt(dsq);
	}

	static double distsq(const Point & p1, const Point & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return dsq;
	}

	static double dot(const Point & p1, const Point & p2){
		double dt = 0.0;
		for (auto i=0; i<dim; i++) dt += p1.x[i]*p2.x[i];
		return dt;
	}

	// print to std::out
	template<std::size_t d>
	friend std::ostream & operator<<(std::ostream & os, const Point<d> & p);

};

template<std::size_t dim>
std::ostream & operator<<(std::ostream & os, const Point<dim> & p){
	os << "(" ;
	for (auto i=0; i< dim-1; i++) os << p.x[i] << ", " ;
	os << p.x[dim-1] << ")" ;
	
	return os;
}

template<std::size_t dim>
Point<dim> operator*(double val, const Point<dim> & p){
	Point<dim> out(p);
	for (auto i=0; i<dim; i++) out.x[i] = val*p.x[i];
	return out;
}

inline Point<3> cross(Point<3> p1, Point<3> p2){
	return Point<3>(p1.x[1]*p2.x[2]-p1.x[2]*p2.x[1], p1.x[2]*p2.x[0]-p1.x[0]*p2.x[2], p1.x[0]*p2.x[1]-p1.x[1]*p2.x[0]);
}

/*
namespace Point{

	

	template<std::size_t dim>
	double distsq(const Point<dim> & p1, const Point<dim> & p2){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++) dsq += (p1.x[i] - p2.x[i])*(p1.x[i] - p2.x[i]);
		return dsq;
	}

	template<std::size_t dim>
	double dot(const Point<dim> & p1, const Point<dim> & p2){
		double dt = 0.0;
		for (auto i=0; i<dim; i++) dt += p1.x[i]*p2.x[i];
		return dt;
	}

}
*/




template <std::size_t dim>
struct Box{
	// data
	Point<dim> lo, hi;

	// empty constructor
	Box(){};

	// constructor
	Box(const Point<dim> & lopt, const Point<dim> & hipt)
	: lo(lopt), hi(hipt) {}

	
	static double dist(const Box & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return sqrt(dsq);
	}

	static double distsq(const Box & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return dsq;
	}

	static bool contains(const Box & bx, const Point<dim> & pt){
		return distsq(bx, pt) == 0;
	}

	static Box bounding_box(const Box & bx1, const Box & bx2){
		Point<dim> lo;
		Point<dim> hi;
		for (auto i=0; i<dim; i++){
			lo.x[i] = std::min(bx1.lo.x[i], bx2.lo.x[i]);
			hi.x[i] = std::max(bx1.hi.x[i], bx2.hi.x[i]);
		}
		return Box(lo,hi);
	}

	// print to std::out
	template<std::size_t d>
	friend std::ostream & operator<<(std::ostream & os, const Box<d> & bx);

};

template<std::size_t dim>
std::ostream & operator<<(std::ostream & os, const Box<dim> & bx){
	os << "lo:" << bx.lo << " hi:" << bx.hi ;
	return os;
}

/*
namespace Box{

	template<std::size_t dim>
	double dist(const Box<dim> & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return sqrt(dsq);
	}

	template<std::size_t dim>
	double distsq(const Box<dim> & bx, const Point<dim> & pt){
		double dsq = 0.0;
		for (auto i=0; i<dim; i++){
			if (pt.x[i] < bx.lo.x[i]) dsq += (pt.x[i] - bx.lo.x[i])*(pt.x[i] - bx.lo.x[i]);
			if (pt.x[i] > bx.hi.x[i]) dsq += (pt.x[i] - bx.hi.x[i])*(pt.x[i] - bx.hi.x[i]);
		}
		return dsq;
	}

	template<std::size_t dim>
	bool contains(const Box<dim> & bx, const Point<dim> & pt){
		return distsq(bx, pt) == 0;
	}

	template<std::size_t dim>
	Box<dim> bounding_box(const Box<dim> & bx1, const Box<dim> & bx2){
		Point<dim> lo;
		Point<dim> hi;
		for (auto i=0; i<dim; i++){
			lo.x[i] = std::min(bx1.lo.x[i], bx2.lo.x[i]);
			hi.x[i] = std::max(bx1.hi.x[i], bx2.hi.x[i]);
		}
		return Box<dim>(lo,hi);
	}

}
*/



struct Plane{

	// empty constructor
	Plane()
	: origin((0,0,0)), normal((0,0,1)), posx((1,0,0)) {};
	// : origin(Point<3> (0,0,0)), normal(Point<3> (0,0,1), posx(Point<3> (1,0,0))) {};

	// constructor
	Plane(const Point<3> & p, const Point<3> & n, const Point<3> & px)
	: origin(p), normal(n), posx(px) {};

	// project a point onto the plane
	Point<2> project(const Point<3> & pt) const{
		Point<3> ptvec(pt.x[0]-origin.x[0], pt.x[1]-origin.x[1], pt.x[2]-origin.x[2]);
		double px = Point<3>::dot(ptvec, posx);

		// the y direction is Z x X
		Point<3> posy(normal.x[1]*posx.x[2] - normal.x[2]*posx.x[1],
					  normal.x[2]*posx.x[0] - normal.x[0]*posx.x[2],
					  normal.x[0]*posx.x[1] - normal.x[1]*posx.x[0]);

		double py = Point<3>::dot(ptvec, posy);
		return Point<2>(px, py);
	}

	// data
	Point<3> origin, normal, posx;

};


// typedef Plane(Point<3>(0,0,0),Point<3>(0,0,1),Point<3>(1,0,0)) XYPLANE;
// typedef Plane(Point<3>(0,0,0),Point<3>(0,0,-1),Point<3>(0,1,0)) YXPLANE;
// typedef Plane(Point<3>(0,0,0),Point<3>(1,0,0),Point<3>(0,1,0)) YZPLANE;
// typedef Plane(Point<3>(0,0,0),Point<3>(-1,0,0),Point<3>(0,0,1)) ZYPLANE;
// typedef Plane(Point<3>(0,0,0),Point<3>(0,1,0),Point<3>(0,0,1)) ZXPLANE;
// typedef Plane(Point<3>(0,0,0),Point<3>(0,-1,0),Point<3>(1,0,0)) XZPLANE;




template <std::size_t dim>
struct Line{

	// empty constructor
	Line(){};

	// constructor
	Line(const Point<dim> & p, const Point<dim> & d)
	: pt(p), dir(d) {};

	// data
	Point<dim> pt, dir;

};



template <std::size_t dim>
struct Segment{

	// empty constructor
	Segment(){};

	// constructor
	Segment(const Point<dim> & b, const Point<dim> & e)
	: begin(b), end(e) {};

	// return where a point pt is to the left of this segment
	// positive value indicates yes it is, 
	// negative indicates it is not
	// zero indicates it lies on the line
	double isLeft(const Point<2> & pt) const {return isLeftImp(pt);};
	virtual double isLeftImp(const Point<2> & pt) const = 0;
	// {
	// 	std::cout << "SEGMENT isLeft" << std::endl;
	// 	return ((end.x[0]-begin.x[0])*(pt.x[1]-begin.x[1])
	// 		  - (pt.x[0] - begin.x[0])*(end.x[1] - begin.x[1]));
	// }

	virtual void print_summary(std::ostream & os = std::cout) const = 0;
	// template<std::size_t d>
	// friend std::ostream & operator<<(std::ostream & os, const Segment<d> & p);

	// data
	Point<dim> begin, end;

};


struct LineSegment : public Segment<2>{

	// empty constructor
	LineSegment(){};

	// constructor
	LineSegment(const Point<2> & b, const Point<2> & e)
	: Segment<2>(b,e) {};

	double isLeftImp(const Point<2> & pt) const {
		// std::cout << "LINESEGMENT isLeft" << std::endl;
		return ((end.x[0]-begin.x[0])*(pt.x[1]-begin.x[1])
			  - (pt.x[0] - begin.x[0])*(end.x[1] - begin.x[1]));
	}

	void print_summary(std::ostream & os = std::cout) const{
			os << "<LineSegment>(" ;
			os << begin.x[0] << ", " ;
			os << begin.x[1] << ")" ;
			os << ",(" ;
			os << end.x[0] << ", " ;
			os << end.x[1] << ")</LineSegment>" ;
	}

};



struct CircleSegment : public Segment<2>{

	// empty constructor
	CircleSegment(){};

	// constructor
	CircleSegment(const Point<2> & b, const Point<2> & e, double rad, bool leftcenter, bool leftrunning)
	: Segment(b,e) {};

	double isLeftImp(const Point<2> & pt) const {
		if (lcen && !lrun){
			return ((end.x[0]-begin.x[0])*(pt.x[1]-begin.x[1])
			  - (pt.x[0] - begin.x[0])*(end.x[1] - begin.x[1]));
		}
		if (~lcen && lrun) return false;
		double dsq = Point<2>::distsq(begin,end);
		double x = sqrt(radius*radius - 0.25*dsq);
		Point<2> yhat = (end-begin).normalize();
		Point<2> mid = 0.5*(end+begin);
		Point<2> xhat(-yhat.x[1]/yhat.x[0],1); xhat.normalize();
		if (xhat.x[0]*yhat.x[1] - xhat.x[1]*yhat.x[0] < 0) xhat = -1.0*xhat;

		Point<2> cen;
		if (lcen) cen = mid + sqrt(dsq)*yhat - x*xhat;
		else cen = mid + sqrt(dsq)*yhat + x*xhat;

		// std::cout << "CIRCLESEGMENT isLeft" << std::endl;
		if (begin.x[1] > end.x[1] && lrun) return false;
		else if (begin.x[1] < end.x[1] && ~lrun) return false;

		// now check the harder cases
	}


	void print_summary(std::ostream & os = std::cout) const{
			os << "<CircleSegment>(" ;
			os << begin.x[0] << ", " ;
			os << begin.x[1] << ")" ;
			os << ",(" ;
			os << end.x[0] << ", " ;
			os << end.x[1] << ")" ;
			os << "," << radius << "," << int(lcen) << "," << int(lrun) << "</CircleSegment>" ;
	}


	double radius;
	bool lcen;	// if true, the circle center is to the left when viewed from begin to end
	bool lrun;	// if true, the circle segment runs left when viewed from begin to end 
};


#endif