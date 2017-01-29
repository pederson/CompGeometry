#ifndef _CSGEOMETRY2D_H
#define _CSGEOMETRY2D_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

// 2D Constructive Solid Geometry model

class CSGTree2D;

class CSGeometry2D 
{
public:
	// virtual unsigned int get_flavor() const = 0;
	virtual Box<2> get_bounding_box() const = 0;
	virtual void translate(const Point<2> & pt) = 0;
	virtual void rotate(const Point<2> & anchor, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	virtual bool contains_point(const Point<2> & pt) const = 0;
	virtual bool contains_box(const Box<2> & bx) const = 0;
	virtual bool collides_box(const Box<2> & bx) const = 0;

	virtual void print_summary(std::ostream & os) const = 0;
};

#endif