#ifndef _PRIMITIVE2D_H
#define _PRIMITIVE2D_H

#include "GeomUtils.hpp"
#include <memory>

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
	: m_center(center)
	, m_lx(dims.x[0])
	, m_ly(dims.x[1])
	, m_rotation(0) {};

	Box<2> get_bounding_box() const {
		Point<2> p1(0.5*m_lx*cos(m_rotation)-0.5*m_ly*sin(m_rotation)+m_center.x[0], 0.5*m_lx*sin(m_rotation)+0.5*m_ly*cos(m_rotation)+m_center.x[1]);
		Point<2> p2(-0.5*m_lx*cos(m_rotation)-0.5*m_ly*sin(m_rotation)+m_center.x[0], -0.5*m_lx*sin(m_rotation)+0.5*m_ly*cos(m_rotation)+m_center.x[1]);
		Point<2> p3(0.5*m_lx*cos(m_rotation)+0.5*m_ly*sin(m_rotation)+m_center.x[0], 0.5*m_lx*sin(m_rotation)-0.5*m_ly*cos(m_rotation)+m_center.x[1]);
		Point<2> p4(-0.5*m_lx*cos(m_rotation)+0.5*m_ly*sin(m_rotation)+m_center.x[0], -0.5*m_lx*sin(m_rotation)-0.5*m_ly*cos(m_rotation)+m_center.x[1]);
		return Box<2>(Point<2>(std::min(std::min(p1.x[0],p2.x[0]),std::min(p3.x[0], p4.x[0])),
							   std::min(std::min(p1.x[1],p2.x[1]),std::min(p3.x[1], p4.x[1]))),
					  Point<2>(std::max(std::max(p1.x[0],p2.x[0]),std::max(p3.x[0], p4.x[0])), 
					  		   std::max(std::max(p1.x[1],p2.x[1]),std::max(p3.x[1], p4.x[1]))));
	}

	void translate(const Point<2> & pt) {
		m_center = m_center + pt;
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		Point<2> x = pt - m_center;
		Point<2> rotx(cos(m_rotation)*x.x[0]+sin(m_rotation)*x.x[1], -sin(m_rotation)*x.x[0]+cos(m_rotation)*x.x[1]);
		Box<2> bx(Point<2>(-m_lx/2, -m_ly/2), Point<2>(m_lx/2, m_ly/2));
		// std::cout << "box dist: " << Box<2>::dist(bx,rotx) << std::endl;
		return Box<2>::distsq(bx, rotx) < 1.0e-16; 
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
		os << "Rectangle: center = " << m_center;
		os << " sides = (" << m_lx << ", " << m_ly << ")" ;
	}
private:

	double m_rotation;	// rotation angle in radians
	double m_lx, m_ly;	// length of the x and y sides
	Point<2> m_center;	
	// LineSegment m_diag1, m_diag2;
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
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end)*0.25;
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end)*0.25;
		Point<2> m_center = 0.5*(m_axis1.begin+m_axis1.end);
		return Box<2>(Point<2>(m_center.x[0]-sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)),
							   m_center.x[1]-sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))),
					  Point<2>(m_center.x[0]+sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)), 
					  		   m_center.x[1]+sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))));
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
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end)*0.25;
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end)*0.25;
		Point<2> cen = 0.5*(m_axis1.begin+m_axis1.end);
		Point<2> x = pt - cen;
		return (x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))*(x.x[0]*cos(m_rotation)-x.x[1]*sin(m_rotation))/asq + 
			   (x.x[0]*sin(m_rotation)+x.x[1]*cos(m_rotation))*(x.x[0]*sin(m_rotation)+x.x[1]*cos(m_rotation))/bsq <= 1; 
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

	double m_rotation;	// rotation angle in radians
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
		int wn = 0;
		LineSegment L1(m_p1, m_p2);
		// std::cout << "point: " << pt << std::endl;
		// std::cout << "isleft? " << (L1.isLeft(pt)) << std::endl;
		if (m_p1.x[1] <= pt.x[1]){
			if (m_p2.x[1] > pt.x[1]){
				if (L1.isLeft(pt) >=0) wn++;
			}
		}
		else{
			if (m_p2.x[1] <= pt.x[1]){
				if (L1.isLeft(pt) <0) wn--;
			}
		}
		// std::cout << "after segment 1: " << wn << std::endl;
		LineSegment L2(m_p2, m_p3);
		// std::cout << "isleft? " << (L2.isLeft(pt)) << std::endl;
		if (m_p2.x[1] <= pt.x[1]){
			if (m_p3.x[1] > pt.x[1]){
				if (L2.isLeft(pt) >=0) wn++;
			}
		}
		else{
			if (m_p3.x[1] <= pt.x[1]){
				if (L2.isLeft(pt) <0) wn--;
			}
		}
		// std::cout << "after segment 2: " << wn << std::endl;
		LineSegment L3(m_p3, m_p1);
		// std::cout << "isleft? " << (L3.isLeft(pt)) << std::endl;
		if (m_p3.x[1] <= pt.x[1]){
			if (m_p1.x[1] > pt.x[1]){	// downward edge
				if (L3.isLeft(pt) >=0) wn++;
			}
		}
		else{	
			if (m_p1.x[1] <= pt.x[1]){	// upward edge
				if (L3.isLeft(pt) <0) wn--;
			}
		}
		// std::cout << "after segment 3: " << wn << std::endl;

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

	Polycurve(const std::vector<std::shared_ptr<Segment<2>>> & segs)
	: m_segments(segs) {};

	Box<2> get_bounding_box() const {
		Point<2> lo = m_segments[0]->begin;
		Point<2> hi = lo;
		for (auto i=0; i<m_segments.size(); i++){
			lo.x[0] = std::min(lo.x[0],m_segments[i]->begin.x[0]);
			lo.x[0] = std::min(lo.x[0],m_segments[i]->end.x[0]);
			lo.x[1] = std::min(lo.x[1],m_segments[i]->begin.x[1]);
			lo.x[1] = std::min(lo.x[1],m_segments[i]->end.x[1]);

			hi.x[0] = std::max(hi.x[0],m_segments[i]->begin.x[0]);
			hi.x[0] = std::max(hi.x[0],m_segments[i]->end.x[0]);
			hi.x[1] = std::max(hi.x[1],m_segments[i]->begin.x[1]);
			hi.x[1] = std::max(hi.x[1],m_segments[i]->end.x[1]);
		}
		return Box<2>(lo,hi);
	}

	void translate(const Point<2> & pt) {
		for (auto i=0; i<m_segments.size(); i++){
			m_segments[i]->begin = m_segments[i]->begin + pt;
			m_segments[i]->end = m_segments[i]->end + pt;
		}
	}

	// void rotate(const Point<2> & anchor, double degrees) {

	// }

	bool contains_point(const Point<2> & pt) const{
		// first check bounding Box
		Box<2> bx = get_bounding_box();
		if (Box<2>::dist(bx, pt) > 1.0e-16) return false;
		std::cout << "checking polycurve" << std::endl;

		// do more rigorous point-in-polygon check
		unsigned int wn = 0;
		for (auto i=0; i<m_segments.size(); i++){

			Point<2> p1 = m_segments[i]->begin;
			Point<2> p2 = m_segments[i]->end;
			// std::cout << p1 << "-->" << p2 << std::endl;
			if (p1.x[1] <= pt.x[1]){
				if (p2.x[1] > pt.x[1]){
					if (m_segments[i]->isLeft(pt)>=0) wn++;
				}
			}
			else{
				if (p2.x[1] <= pt.x[1]){
					if (m_segments[i]->isLeft(pt)<0) wn--;
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
		os << " " << m_segments[0]->begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i]->begin ;
		}
	}

protected:

	std::vector<std::shared_ptr<Segment<2>>> m_segments;
};





// a collection of Segments that connect
// this is a generalization of a polygon to permit curved segments
class Polygon : public Polycurve
{
public:

	Polygon(){};

	Polygon(const std::vector<LineSegment> & segs)
	{
		for (auto i=0; i<segs.size(); i++) m_segments.push_back(std::shared_ptr<Segment<2>>(new LineSegment(segs[i])));
	}

	void print_summary(std::ostream & os = std::cout) const{
		os << "Polygon: nsides = " << m_segments.size() ;
		os << " " << m_segments[0]->begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i]->begin ;
		}
	}
};








class RegularPolygon : public Polygon
{
public:

	RegularPolygon(){};

	RegularPolygon(unsigned int nsides, const Point<2> & center, double tovertex)
	{
		// angle between consecutive segments
		double pi = 3.14159265358979323846;
		double dtheta = 360.0/nsides * pi/180.0;

		// beginning angle
		double theta = -pi/2 - dtheta*0.5;

		// std::cout << "dtheta: " << dtheta << " theta0: " << theta << std::endl;

		// calculate locations of line segments
		for (auto i=0; i<nsides; i++){
			// std::cout << "theta: " << theta << std::endl;
			Point<2> begin (tovertex*cos(theta), tovertex*sin(theta));
			Point<2> end (tovertex*cos(theta+dtheta), tovertex*sin(theta+dtheta));
			// std::cout << begin << "-->" << end << std::endl;
			LineSegment s(begin+center, end+center);
			m_segments.push_back(std::shared_ptr<Segment<2>>(new LineSegment(s)));
			theta += dtheta;
		}


	};

	void print_summary(std::ostream & os = std::cout) const{
		os << "RegularPolygon: nsides = " << m_segments.size() ;
		os << " " << m_segments[0]->begin ;
		for (auto i=1; i<m_segments.size(); i++){
			os << "-->" << m_segments[i]->begin ;
		}
	}

};
#endif