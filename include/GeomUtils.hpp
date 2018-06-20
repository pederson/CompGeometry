#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include <iostream>
#include <math.h>
#include <vector>
#include <memory>
#include <iterator>
#include <algorithm>

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "Point.hpp"

namespace csg{

const double pi = 3.14159265358979323846264338327950288;

// Constructive Solid Geometry operations
enum Operation {UNION, INTERSECT, DIFFERENCE, XOR};




template <typename T> int sgn(T val) {
    return (static_cast<T>(0) < val) - (val < static_cast<T>(0));
}





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

	static Box translate(const Box & bx1, const Point<dim> & pt){
		return Box(bx1.lo+pt, bx1.hi+pt);
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


template <typename BoxType>
BoxType bounding_box(const BoxType & bx1, const BoxType & bx2){
	return BoxType::bounding_box(bx1, bx2);
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
	: origin(0,0,0), normal(0,0,1), posx(1,0,0) {};
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

// static const Plane XYPlane = Plane(Point3(0,0,0),Point3(0,0,1),Point3(1,0,0));

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
	: pt(p), dir(d) {dir.normalize();};

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
		return false;
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


template<std::size_t dim>
struct Hull{
	// list of points assumed to be in consecutive order
	std::vector<Point<dim>> points;

	// empty constructor
	Hull(){};

	// constructor
	Hull(const std::vector<Point<dim>> & pts)
	: points(pts) {};


	void print_summary(std::ostream & os = std::cout) const{
			os << "<Hull>" ;
			for (auto i=0; i<points.size(); i++) os << points[i] ;
			os << "</Hull>" ;
	}
};


template <std::size_t dim>
struct Triangulation{

	std::vector<Point<dim>> points;		// list of points
	std::vector<IntPoint3> triangles;	// list of triangles as indices in the points vector

	Triangulation() {};

	// static std::shared_ptr<Triangulation<3>> read_STL(std::string filename, unsigned int byte_offset=0);

};


#pragma pack(push,1)
struct stl_tri{
	float norm_x;
	float norm_y;
	float norm_z;

	float v1_x;
	float v1_y;
	float v1_z;

	float v2_x;
	float v2_y;
	float v2_z;

	float v3_x;
	float v3_y;
	float v3_z;

	unsigned short attrib_byte_count;
};
#pragma pack(pop)

inline std::shared_ptr<Triangulation<3>> read_STL(std::string filename, unsigned int byte_offset=0){
	// declare vars
	std::shared_ptr<Triangulation<3>> out(new Triangulation<3>());
	int fd;
	unsigned int tricount;
	char * stlmap;

	// open file and fast forward
	fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0){
		std::cerr << "Triangulation<3>: Error opening file in read_STL" << std::endl;
	throw -1;
	}
	lseek(fd, byte_offset, SEEK_SET);

	// find the triangle count
	lseek(fd, 80, SEEK_CUR); // skip the header
	read(fd, &tricount, 4); // read the triangle count


  lseek(fd, byte_offset, SEEK_CUR); // back to the beginning
	stlmap = (char *)mmap(NULL, 84 + sizeof(stl_tri)*tricount, PROT_READ, MAP_PRIVATE, fd, 0);
	if (stlmap == MAP_FAILED){
		std::cerr << "Triangulation<3>: Failed to map STL file" << std::endl;
	throw -1;
	}

	// copy the triangle data into structures
	// stl_tri * triangles = new stl_tri[tricount];
	std::vector<stl_tri> triangles(tricount);
	// memcpy(triangles, &stlmap[84], sizeof(stl_tri)*tricount);
	std::copy(&stlmap[84], &stlmap[84+sizeof(stl_tri)*tricount], (char *) &triangles.front());

	// copy the structure data into the member data
	out->points.resize(tricount*3);
	out->triangles.resize(tricount);
	for (unsigned int i=0; i<tricount; i++){
		out->points[i*3] = Point<3>(triangles[i].v1_x, triangles[i].v1_y, triangles[i].v1_z);
		out->points[i*3+1] = Point<3>(triangles[i].v2_x, triangles[i].v2_y, triangles[i].v2_z);
		out->points[i*3+2] = Point<3>(triangles[i].v3_x, triangles[i].v3_y, triangles[i].v3_z);

		out->triangles[i] = IntPoint3(i*3, i*3+1, i*3+2);
	}

	if (munmap(stlmap, 84 + sizeof(stl_tri)*tricount) < 0){
		std::cerr << "Triangulation<3>: ruh roh! problem unmapping STL file" << std::endl;
		throw -1;
	}
	close(fd);

	// delete[] triangles;
	return out;
}


}
#endif