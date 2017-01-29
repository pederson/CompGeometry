#ifndef _PRIMITIVE3D_H
#define _PRIMITIVE3D_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

class Primitive2D 
{
public:
	// virtual unsigned int get_flavor() const = 0;
	virtual Box<3> get_bounding_box() const = 0;
	virtual void translate(const Point<3> & pt) = 0;
	virtual void rotate(const Point<3> & axis, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	virtual bool contains_point(const Point<3> & pt) const = 0;
	virtual bool contains_box(const Box<3> & bx) const = 0;
	virtual bool collides_box(const Box<3> & bx) const = 0;

	virtual void print_summary(std::ostream & os) const = 0;
};


class Sphere : public Primitive3D
{
public:

	Sphere(){};

	Sphere(const Point<3> & center, double radius)
	: m_center(center), m_radius(radius) {};

	Box<3> get_bounding_box() const {
		return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius)
					  Point<3>(m_center.x[0]+m_radius, m_center.x[1]+m_radius, m_center.x[2]+m_radius));
	}

	void translate(const Point<3> & pt) {
		m_center = m_center + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		return (pt.x[0]-m_center.x[0])*(pt.x[0]-m_center.x[0]) + 
			   (pt.x[1]-m_center.x[1])*(pt.x[1]-m_center.x[1]) + 
			   (pt.x[2]-m_center.x[2])*(pt.x[2]-m_center.x[2]) <= m_radius*m_radius; 
	}

	bool contains_box(const Box<3> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Sphere: center = " ;
		os << m_center ;
		os << " radius = " << m_radius ;
	}
private:

	double m_radius;
	Point<3> m_center;
};






class Cylinder : public Primitive3D
{
public:

	Cylinder(){};

	Cylinder(const Point<3> & center, const Point<3> & normal, double radius, double height)
	: m_plane(Plane(center, normal))
	, m_circle(Circle(Point<2>(0,0), radius))
	, m_height(height) {};


	// Box<3> get_bounding_box() const {
	// 	return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius)
	// 				  Point<3>(m_center.x[0]+m_radius, m_center.x[1]+m_radius, m_center.x[2]+m_radius));
	// }

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		return m_circle.contains_point(pp);
	}

	bool contains_box(const Box<3> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Cylinder: center = " ;
		os << m_center ;
		os << " radius = " << m_circle.m_radius ;
		os << " height = " << m_height ;
	}
private:

	double m_height;
	Plane m_plane;
	Circle m_circle;
};






class Pyramid : public Primitive3D
{
public:

	Pyramid(){};

	Pyramid(const Primitive2D * base, const Point<3> & center, const Point<3> & normal, double height)
	: m_plane(Plane(center, normal))
	, m_base(base)
	, m_height(height) {};


	// Box<3> get_bounding_box() const {
	// 	return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius)
	// 				  Point<3>(m_center.x[0]+m_radius, m_center.x[1]+m_radius, m_center.x[2]+m_radius));
	// }

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point::dot(ptvec, m_plane.normal);
		if (proj >= m_height || proj < 0) return false;

		// now project the point onto the plane
		double M = 1.0/(1.0-proj/m_height); // need to scale the plane projection b/c pyramid shape
		Point<2> pp = m_plane.project(pt);
		pp = M*pp;
		return m_base->contains_point(pp);
	}

	bool contains_box(const Box<3> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Pyramid: center = " ;
		os << m_plane.origin ;
		os << " height = " << m_height ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	const Primitive2D * m_base;
	double m_height;
	Plane m_plane;

};







class Extrusion : public Primitive3D
{
public:

	Extrusion(){};

	Extrusion(const Primitive2D * base, const Point<3> & center, const Point<3> & normal, double height)
	: m_plane(Plane(center, normal))
	, m_base(base)
	, m_height(height) {};


	// Box<3> get_bounding_box() const {
	// 	return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius)
	// 				  Point<3>(m_center.x[0]+m_radius, m_center.x[1]+m_radius, m_center.x[2]+m_radius));
	// }

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		return m_base->contains_point(pp);
	}

	bool contains_box(const Box<3> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Extrusion: center = " ;
		os << m_plane.origin ;
		os << " height = " << m_height ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	const Primitive2D * m_base;
	double m_height;
	Plane m_plane;

};







class Sweep : public Primitive3D
{
public:

	Sweep(){};

	Sweep(const Primitive2D * base, const Point<3> & center, const Point<3> & normal, Line ln, double angle)
	: m_plane(Plane(center, normal))
	, m_base(base)
	, m_line(ln)
	, m_angle(angle) {};


	// Box<3> get_bounding_box() const {
	// 	return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius)
	// 				  Point<3>(m_center.x[0]+m_radius, m_center.x[1]+m_radius, m_center.x[2]+m_radius));
	// }

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
		m_line.pt = m_line.pt + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// want to somehow use 2D projection

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		pp = M*pp;
		return m_base->contains_point(pp);
	}

	bool contains_box(const Box<3> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Sweep: center = " ;
		os << m_plane.origin ;
		os << " height = " << m_height ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	const Primitive2D * m_base;
	double m_angle;
	Plane m_plane;
	Line m_line;

};

#endif