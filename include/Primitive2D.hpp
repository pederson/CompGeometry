#ifndef _PRIMITIVE2D_H
#define _PRIMITIVE2D_H

#include "GeomUtils.hpp"

class Primitive2D 
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


class Circle : public Primitive2D
{
public:

	Circle(){};

	Circle(const Point<2> & center, double radius)
	: m_center(center), m_radius(radius) {};

	Box<2> get_bounding_box() const {
		return Box<2>(Point<2>(m_center.x[0]-m_radius, m_center.x[1]-m_radius)
					  Point<2>(m_center.x[0]+m_radius, m_center.x[1]+m_radius));
	}

	void translate(const Point<2> & pt) {
		m_center = m_center + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		return (pt.x[0]-m_center.x[0])*(pt.x[0]-m_center.x[0]) + 
			   (pt.x[1]-m_center.x[1])*(pt.x[1]-m_center.x[1]) <= m_radius*m_radius; 
	}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Circle: center = " ;
		os << m_center ;
		os << " radius = " << m_radius ;
	}
private:

	double m_radius;
	Point<2> m_center;
};




class Rectangle : public Primitive2D
{
public:

	Rectangle(){};

	Rectangle(const Point<2> & center, const Point<2> & dims)
	: m_diag1(center-0.5*dims, center+0.5*dims)
	, m_diag2(center+Point<2>(-0.5*dims.x[0], 0.5*dims.x[1]), center + Point<2>(0.5*dims.x[0], -0.5*dims.x[1])) {};

	Box<2> get_bounding_box() const {
		return Box<2>(Point<2>(std::min(std::min(m_diag1.begin.x[0],m_diag1.end.x[0]),std::min(m_diag2.begin.x[0], m_diag2.end.x[0])),
							   std::min(std::min(m_diag1.begin.x[1],m_diag1.end.x[1]),std::min(m_diag2.begin.x[1], m_diag2.end.x[1])))
					  Point<2>(std::max(std::max(m_diag1.begin.x[0],m_diag1.end.x[0]),std::max(m_diag2.begin.x[0], m_diag2.end.x[0])), 
					  		   std::max(std::max(m_diag1.begin.x[1],m_diag1.end.x[1]),std::max(m_diag2.begin.x[1], m_diag2.end.x[1]))));
	}

	void translate(const Point<2> & pt) {
		m_diag1.begin = m_diag1.begin + pt;
		m_diag1.end = m_diag1.end + pt;
		m_diag2.begin = m_diag2.begin + pt;
		m_diag2.end = m_diag2.end + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		double sidelong = std::max(Point::distsq(m_diag1.begin, m_diag2.end), Point::distsq(m_diag1.begin, m_diag2.begin));
		return (Point::distsq(m_diag1.begin,pt) <= sidelong) && (Point::distsq(m_diag1.end,pt) <= sidelong)
		    && (Point::distsq(m_diag2.begin,pt) <= sidelong) && (Point::distsq(m_diag2.end,pt) <= sidelong); 
	}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Rectangle: sides = " ;
		os << "(" << Point::dist(m_diag1.begin, m_diag2.end) << Point::dist(m_diag1.begin, m_diag2.begin) << ")" ;
	}
private:

	LineSegment m_diag1, m_diag2;
};





class Ellipse : public Primitive2D
{
public:

	Ellipse(){};

	Ellipse(const Point<2> & center, const Point<2> & dims)
	: m_axis1(center+Point<2>(-0.5*dims.x[1], 0), center + Point<2>(0.5*dims.x[1]), 0)
	, m_axis2(center+Point<2>(0, -0.5*dims.x[1]), center + Point<2>(0, 0.5*dims.x[1]))
	, m_rotation(0) {};

	Box<2> get_bounding_box() const {
		double asq = Point::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point::distsq(m_axis2.begin, m_axis2.end);
		return Box<2>(Point<2>(-sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)),
							   -sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation)))
					  Point<2>(sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)), 
					  		   sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))));
	}

	void translate(const Point<2> & pt) {
		m_axis1.begin = m_axis1.begin + pt;
		m_axis1.end = m_axis1.end + pt;
		m_axis2.begin = m_axis2.begin + pt;
		m_axis2.end = m_axis2.end + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		double asq = Point::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point::distsq(m_axis2.begin, m_axis2.end);
		Point cen = 0.5*(m_axis1.begin+m_axis1.end);
		Point x = pt - cen;
		return (x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))*(x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))/asq
			   (x.x[0]*sin(m_rotation)-x.x[1]*cos(m_rotation))*(x.x[0]*sin(m_rotation)-x.x[1]*cos(m_rotation))/bsq <= 1; 
	}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Ellipse: sides = " ;
		os << "(" << Point::dist(m_axis1.begin, m_axis1.end) << Point::dist(m_axis2.begin, m_axis2.end) << ")" ;
		os << " center = " << 0.5*(m_axis1.begin + m_axis1.end) ;
	}
private:

	double m_rotation;
	LineSegment m_axis1, m_axis2;
};








class RegularPolygon : public Primitive2D
{
public:

	RegularPolygon(){};

	RegularPolygon(unsigned int nsides, const Point<2> & center, double tovertex)
	: m_center(center)
	, m_nsides(nsides)
	, m_c2v(tovertex)
	, m_rotation(0) {};

	Box<2> get_bounding_box() const {
		double asq = Point::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point::distsq(m_axis2.begin, m_axis2.end);
		return Box<2>(m_center - Point<2>(m_c2v, m_c2v),
					  m_center + Point<2>(m_c2v, m_c2v));
	}

	void translate(const Point<2> & pt) {
		m_axis1.begin = m_axis1.begin + pt;
		m_axis1.end = m_axis1.end + pt;
		m_axis2.begin = m_axis2.begin + pt;
		m_axis2.end = m_axis2.end + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		// first check bounding ball
		if ((pt.x[0]-m_center.x[0])*(pt.x[0]-m_center.x[0]) + 
			   (pt.x[1]-m_center.x[1])*(pt.x[1]-m_center.x[1]) > m_c2v*m_c2v){
			return false;
		}

		// rotate point before checking

		// do more rigorous point-in-polygon check
			}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "RegularPolygon: nsides = " << m_nsides ;
		os << " radius = " << m_c2v ;
	}
private:

	unsigned int m_nsides;
	double m_rotation;
	double m_c2v;	// center to vertex length
	Point<2> m_center;
};







class Triangle : public Primitive2D
{
public:

	Triangle(){};

	Triangle(const Point<2> & p1, const Point<2> & p2, const Point<2> & p3)
	: m_p1(p1)
	, m_p2(p2)
	, m_p3(p3) {};

	Box<2> get_bounding_box() const {
		double asq = Point::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point::distsq(m_axis2.begin, m_axis2.end);
		return Box<2>(Point<2>(std::min(std::min(m_p1.x[0],m_p2.x[0]),m_p3.x[0]), 
					  		   std::min(std::min(m_p1.x[1],m_p2.x[1]),m_p3.x[1])),
					  Point<2>(std::max(std::max(m_p1.x[0],m_p2.x[0]),m_p3.x[0]),
					  		   std::max(std::max(m_p1.x[1],m_p2.x[1]),m_p3.x[1])));
	}

	void translate(const Point<2> & pt) {
		m_p1 = m_p1 + pt;
		m_p2 = m_p2 + pt; 
		m_p3 = m_p3 + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{

		// do rigorous point-in-polygon check
		unsigned int wn = 0;
		if (m_p1.x[1] <= pt.x[1]){
			if (m_p2.x[1] > pt.x[1]){
				if (isLeft(m_p1, m_p2, pt)) wn++;
			}
		}
		else{
			if (m_p2.x[1] <= pt.x[1]){
				if (isLeft(m_p1, m_p2, pt)) wn--;
			}
		}
		if (m_p2.x[1] <= pt.x[1]){
			if (m_p3.x[1] > pt.x[1]){
				if (isLeft(m_p2, m_p3, pt)) wn++;
			}
		}
		else{
			if (m_p3.x[1] <= pt.x[1]){
				if (isLeft(m_p2, m_p3, pt)) wn--;
			}
		}
		if (m_p3.x[1] <= pt.x[1]){
			if (m_p1.x[1] > pt.x[1]){
				if (isLeft(m_p3, m_p1, pt)) wn++;
			}
		}
		else{
			if (m_p1.x[1] <= pt.x[1]){
				if (isLeft(m_p3, m_p1, pt)) wn--;
			}
		}

		return (wn==0)? false : true;
	}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Triangle: " << m_p1 << m_p2 << m_p3 ;
	}
private:

	Point<2> m_p1, m_p2, m_p3;
};







// a collection of Segments that connect
// this is a generalization of a polygon to permit curved segments
class Polycurve : public Primitive2D
{
public:

	Polycurve(){};

	Polycurve(const std::vector<Segment> & segs)
	: m_segments(segs) {};

	Box<2> get_bounding_box() const {
		return Box<2>(m_center - Point<2>(m_c2v, m_c2v),
					  m_center + Point<2>(m_c2v, m_c2v));
	}

	void translate(const Point<2> & pt) {
		for (auto i=0; i<m_segments.size(); i++){
			m_segments[i].begin = m_segments[i].begin + pt;
			m_segments[i].end = m_segments[i].end + pt;
		}
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		// first check bounding ball

		// rotate point before checking

		// do more rigorous point-in-polygon check
	}

	bool contains_box(const Box<2> & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	bool collides_box(const Box<2> & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(Point<2>(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(Point<2>(bx.hi.x[0], bx.lo.x[1]));
	}

	void print_summary(std::ostream & os) const{
		os << "Polycurve: nsides = " << m_segments.size() ;
	}
private:

	std::vector<Segment> m_segments;
};

#endif