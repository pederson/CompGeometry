#ifndef _PRIMITIVE2D_H
#define _PRIMITIVE2D_H

#include "GeomUtils.hpp"

class Primitive2D 
{
public:
	virtual Box<2> get_bounding_box() const = 0;
	virtual void translate(const Point<2> & pt) = 0;
	// virtual void rotate(const Point<2> & anchor, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	virtual bool contains_point(const Point<2> & pt) const = 0;
	virtual bool contains_box(const Box<2> & bx) const = 0;
	virtual bool collides_box(const Box<2> & bx) const = 0;

	virtual void print_summary(std::ostream & os = std::cout) const = 0;
};


class Circle : public Primitive2D
{
public:

	Circle(){};

	Circle(const Point<2> & center, double radius)
	: m_center(center), m_radius(radius) {};

	Box<2> get_bounding_box() const {
		return Box<2>(Point<2>(m_center.x[0]-m_radius, m_center.x[1]-m_radius),
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

	void print_summary(std::ostream & os = std::cout) const{
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
							   std::min(std::min(m_diag1.begin.x[1],m_diag1.end.x[1]),std::min(m_diag2.begin.x[1], m_diag2.end.x[1]))),
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
		double sidelong = std::max(Point<2>::distsq(m_diag1.begin, m_diag2.end), Point<2>::distsq(m_diag1.begin, m_diag2.begin));
		return (Point<2>::distsq(m_diag1.begin,pt) <= sidelong) && (Point<2>::distsq(m_diag1.end,pt) <= sidelong)
		    && (Point<2>::distsq(m_diag2.begin,pt) <= sidelong) && (Point<2>::distsq(m_diag2.end,pt) <= sidelong); 
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

	void print_summary(std::ostream & os = std::cout) const{
		os << "Rectangle: center = " << (0.5*(m_diag1.begin+m_diag1.end));
		os << " sides = (" << Point<2>::dist(m_diag1.begin, m_diag2.end) << ", " << Point<2>::dist(m_diag1.begin, m_diag2.begin) << ")" ;
	}
private:

	LineSegment m_diag1, m_diag2;
};





class Ellipse : public Primitive2D
{
public:

	Ellipse(){};

	Ellipse(const Point<2> & center, const Point<2> & dims)
	: m_axis1(center+Point<2>(-0.5*dims.x[0], 0), center + Point<2>(0.5*dims.x[0], 0))
	, m_axis2(center+Point<2>(0, -0.5*dims.x[1]), center + Point<2>(0, 0.5*dims.x[1]))
	, m_rotation(0) {};

	Box<2> get_bounding_box() const {
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end);
		return Box<2>(Point<2>(-sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)),
							   -sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))),
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
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end);
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end);
		Point<2> cen = 0.5*(m_axis1.begin+m_axis1.end);
		Point<2> x = pt - cen;
		return (x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))*(x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))/asq + 
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

	void print_summary(std::ostream & os = std::cout) const{
		os << "Ellipse: center = " << 0.5*(m_axis1.begin + m_axis1.end) ;
		os << " sides = (" << Point<2>::dist(m_axis1.begin, m_axis1.end) << ", " << Point<2>::dist(m_axis2.begin, m_axis2.end) << ")" ;
	}
private:

	double m_rotation;
	LineSegment m_axis1, m_axis2;
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
		LineSegment L1(m_p1, m_p2);
		if (m_p1.x[1] <= pt.x[1]){
			if (m_p2.x[1] > pt.x[1]){
				if (L1.isLeft(pt)) wn++;
			}
		}
		else{
			if (m_p2.x[1] <= pt.x[1]){
				if (L1.isLeft(pt)) wn--;
			}
		}
		LineSegment L2(m_p2, m_p3);
		if (m_p2.x[1] <= pt.x[1]){
			if (m_p3.x[1] > pt.x[1]){
				if (L2.isLeft(pt)) wn++;
			}
		}
		else{
			if (m_p3.x[1] <= pt.x[1]){
				if (L2.isLeft(pt)) wn--;
			}
		}
		LineSegment L3(m_p3, m_p1);
		if (m_p3.x[1] <= pt.x[1]){
			if (m_p1.x[1] > pt.x[1]){
				if (L3.isLeft(pt)) wn++;
			}
		}
		else{
			if (m_p1.x[1] <= pt.x[1]){
				if (L3.isLeft(pt)) wn--;
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

	void print_summary(std::ostream & os = std::cout) const{
		os << "Triangle: " << m_p1 << "-->" << m_p2 << "-->" << m_p3 ;
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

	Polycurve(const std::vector<Segment<2>> & segs)
	: m_segments(segs) {};

	Box<2> get_bounding_box() const {
		Point<2> lo = m_segments[0].begin;
		Point<2> hi = lo;
		for (auto i=0; i<m_segments.size(); i++){
			lo.x[0] = std::min(lo.x[0],m_segments[i].begin.x[0]);
			lo.x[0] = std::min(lo.x[0],m_segments[i].end.x[0]);
			lo.x[1] = std::min(lo.x[1],m_segments[i].begin.x[1]);
			lo.x[1] = std::min(lo.x[1],m_segments[i].end.x[1]);

			hi.x[0] = std::max(hi.x[0],m_segments[i].begin.x[0]);
			hi.x[0] = std::max(hi.x[0],m_segments[i].end.x[0]);
			hi.x[1] = std::max(hi.x[1],m_segments[i].begin.x[1]);
			hi.x[1] = std::max(hi.x[1],m_segments[i].end.x[1]);
		}
		return Box<2>(lo,hi);
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
		// first check bounding Box
		Box<2> bx = get_bounding_box();
		if (Box<2>::distsq(bx, pt) > 0) return false;

		// do more rigorous point-in-polygon check
		unsigned int wn = 0;
		for (auto i=0; i<m_segments.size(); i++){
			Point<2> p1 = m_segments[i].begin;
			Point<2> p2 = m_segments[i].end;
			if (p1.x[1] <= pt.x[1]){
				if (p2.x[1] > pt.x[1]){
					if (m_segments[i].isLeft(pt)) wn++;
				}
			}
			else{
				if (p2.x[1] <= pt.x[1]){
					if (m_segments[i].isLeft(pt)) wn--;
				}
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

	virtual void print_summary(std::ostream & os = std::cout) const{
		os << "Polycurve: nsides = " << m_segments.size() ;
		os << " " << m_segments[0].begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i].begin ;
		}
	}

protected:

	std::vector<Segment<2>> m_segments;
};





// a collection of Segments that connect
// this is a generalization of a polygon to permit curved segments
class Polygon : public Polycurve
{
public:

	Polygon(){};

	Polygon(const std::vector<LineSegment> & segs)
	: m_segments(segs) {};

	void print_summary(std::ostream & os = std::cout) const{
		os << "Polygon: nsides = " << m_segments.size() ;
		os << " " << m_segments[0].begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i].begin ;
		}
	}

protected:
	std::vector<LineSegment> m_segments;
};








class RegularPolygon : public Polygon
{
public:

	RegularPolygon(){};

	RegularPolygon(unsigned int nsides, const Point<2> & center, double tovertex)
	{
		// angle between consecutive segments
		double dtheta = 360.0/nsides;

		// beginning angle
		double theta = -90 - dtheta*0.5;

		// calculate locations of line segments
		for (auto i=0; i<nsides; i++){
			Point<2> begin (tovertex*cos(theta), tovertex*sin(theta));
			Point<2> end (tovertex*cos(theta+dtheta), tovertex*sin(theta+dtheta));
			LineSegment s(begin+center, end+center);
			m_segments.push_back(s);
			theta += dtheta;
		}


	};

	void print_summary(std::ostream & os = std::cout) const{
		os << "RegularPolygon: nsides = " << m_segments.size() ;
		os << " " << m_segments[0].begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i].begin ;
		}
	}

};
#endif