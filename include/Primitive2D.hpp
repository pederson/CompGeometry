#ifndef _PRIMITIVE2D_H
#define _PRIMITIVE2D_H

#include "GeomUtils.hpp"
#include "PrimitiveTypes.hpp"
#include <memory>

namespace csg{



class Circle : public Primitive2D
{
public:

	Circle(){};

	Circle(const Circle & c) 
	: m_radius(c.m_radius), m_center(c.m_center) {};

	Circle(const Point<2> & center, double radius)
	: m_center(center), m_radius(radius) {};

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Circle>(*this);};

	double radius() const {return m_radius;};

	Point<2> center() const {return m_center;};

	Box<2> get_bounding_box() const {
		return Box<2>(Point<2>(m_center.x[0]-m_radius, m_center.x[1]-m_radius),
					  Point<2>(m_center.x[0]+m_radius, m_center.x[1]+m_radius));
	}

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		Hull<2> h1;
		h1.points.resize(npts);
		double angle = 2*pi/npts;	// angle between points in radians
		for (auto i=0; i<npts; i++){
			h1.points[i] = Point<2>(m_center.x[0]+m_radius*cos(angle*i), m_center.x[1]+m_radius*sin(angle*i));
		}
		return {h1};
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

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Circle>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_center << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Radius>" << m_radius << "</Radius>" << std::endl ;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Circle>" << std::endl;
	}

	static Circle circumcircle(const Point<2> & p1, const Point<2> & p2, const Point<2> & p3){
		double a0, a1, c0, c1, det, asq, csq, ctr0, ctr1, rad2;
		a0 = p1.x[0] - p2.x[0]; a1 = p1.x[1] - p2.x[1];
		c0 = p3.x[0] - p2.x[0]; c1 = p3.x[1] - p2.x[1];
		det = a0*c1 - c0*a1;
		if (det == 0.0) throw("no circle thru colinear points");
		det = 0.5/det;
		asq = a0*a0 + a1*a1;
		csq = c0*c0 + c1*c1;
		ctr0 = det*(asq*c1 - csq*a1);
		ctr1 = det*(csq*a0 - asq*c0);
		rad2 = ctr0*ctr0 + ctr1*ctr1;
		return Circle(Point<2>(ctr0 + p2.x[0], ctr1 + p2.x[1]), sqrt(rad2));
	}

private:

	double 			m_radius;
	Point<2> 		m_center;
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

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Rectangle>(*this);};

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

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		Hull<2> h1;
		// h1.points.resize(npts);
		
		// fraction of points allocated to x side
		double fx = m_lx/(m_lx+m_ly), fy = m_ly/(m_lx+m_ly);
		// bottom side
		unsigned int nB = (fx*npts)/2;
		for (auto i=0; i<nB; i++) h1.points.push_back(Point<2>(m_center.x[0]-m_lx/2.0+m_lx*double(i)/double(nB), m_center.x[1]-m_ly/2.0));
		// right side
		unsigned int nR = (fy*npts)/2;
		for (auto i=0; i<nR; i++) h1.points.push_back(Point<2>(m_center.x[0]+m_lx/2.0, m_center.x[1]-m_ly/2.0 + m_ly*double(i)/double(nR)));
		// top side
		for (auto i=0; i<nB; i++) h1.points.push_back(Point<2>(m_center.x[0]+m_lx/2.0-m_lx*double(i)/double(nB), m_center.x[1]+m_ly/2.0));
		// right side
		for (auto i=0; i<nR; i++) h1.points.push_back(Point<2>(m_center.x[0]-m_lx/2.0, m_center.x[1]+m_ly/2.0 - m_ly*double(i)/double(nR)));


		return {h1};
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

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Rectangle>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_center << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Sides>(" << m_lx << ", " << m_ly << ")</Sides>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Rotation>" << m_rotation << "</Rotation>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Rectangle>" << std::endl;
	}
private:

	double 			m_rotation;	// rotation angle in radians
	double 			m_lx, m_ly;	// length of the x and y sides
	Point<2> 		m_center;	
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

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Ellipse>(*this);};

	Box<2> get_bounding_box() const {
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end)*0.25;
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end)*0.25;
		Point<2> m_center = 0.5*(m_axis1.begin+m_axis1.end);
		return Box<2>(Point<2>(m_center.x[0]-sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)),
							   m_center.x[1]-sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))),
					  Point<2>(m_center.x[0]+sqrt(asq*cos(m_rotation)*cos(m_rotation) + bsq*sin(m_rotation)*sin(m_rotation)), 
					  		   m_center.x[1]+sqrt(asq*sin(m_rotation)*sin(m_rotation) + bsq*cos(m_rotation)*cos(m_rotation))));
	}

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		Hull<2> h1;
		// h1.points.resize(npts);
		double angle = 2*pi/npts;	// angle between points in radians
		double asq = Point<2>::distsq(m_axis1.begin, m_axis1.end)*0.25;
		double bsq = Point<2>::distsq(m_axis2.begin, m_axis2.end)*0.25;
		Point<2> m_center = 0.5*(m_axis1.begin+m_axis1.end);
		for (auto i=0; i<npts; i++){
			double rad = sqrt(asq*bsq)/sqrt(bsq*cos(angle*i)*cos(angle*i)+asq*sin(angle*i)*sin(angle*i));
			h1.points.push_back(Point<2>(m_center.x[0]+rad*cos(angle*i), m_center.x[1]+rad*sin(angle*i)));
		}
		return {h1};
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

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Ellipse>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << 0.5*(m_axis1.begin+m_axis1.end) << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Axes>(" << 0.5*Point<2>::dist(m_axis1.end,m_axis1.begin) << ", " << 0.5*Point<2>::dist(m_axis2.end,m_axis2.begin) << ")</Axes>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Rotation>" << m_rotation << "</Rotation>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Ellipse>" << std::endl;
	}
private:

	double 				m_rotation;	// rotation angle in radians
	LineSegment 		m_axis1, m_axis2;
};







class Triangle : public Primitive2D
{
public:

	Triangle(){};

	Triangle(const Point<2> & p1, const Point<2> & p2, const Point<2> & p3)
	: m_p1(p1)
	, m_p2(p2)
	, m_p3(p3) {};

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Triangle>(*this);};

	Box<2> get_bounding_box() const {
		return Box<2>(Point<2>(std::min(std::min(m_p1.x[0],m_p2.x[0]),m_p3.x[0]), 
					  		   std::min(std::min(m_p1.x[1],m_p2.x[1]),m_p3.x[1])),
					  Point<2>(std::max(std::max(m_p1.x[0],m_p2.x[0]),m_p3.x[0]),
					  		   std::max(std::max(m_p1.x[1],m_p2.x[1]),m_p3.x[1])));
	}

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		Hull<2> h1;
		// h1.points.resize(npts);
		double l1 = Point<2>::dist(m_p1, m_p2);
		double l2 = Point<2>::dist(m_p2, m_p3);
		double l3 = Point<2>::dist(m_p3, m_p1);
		double f1 = l1/(l1+l2+l3);
		double f2 = l2/(l1+l2+l3);
		double f3 = l3/(l1+l2+l3);
		unsigned int np1 = f1*npts;
		unsigned int np2 = f2*npts;
		unsigned int np3 = f3*npts;
		Point<2> p1top2 = m_p2-m_p1;
		for (auto i=0; i<np1; i++) h1.points.push_back(m_p1 + double(i)/double(np1)*p1top2);
		Point<2> p2top3 = m_p3-m_p2;
		for (auto i=0; i<np2; i++) h1.points.push_back(m_p2 + double(i)/double(np2)*p2top3);
		Point<2> p3top1 = m_p1-m_p3;
		for (auto i=0; i<np3; i++) h1.points.push_back(m_p3 + double(i)/double(np3)*p3top1);

		return {h1};
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

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		// os << "Triangle: " << m_p1 << "-->" << m_p2 << "-->" << m_p3 ;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Triangle>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Point>" << m_p1 << "</Point>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Point>" << m_p2 << "</Point>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Point>" << m_p3 << "</Point>" << std::endl ;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Triangle>" << std::endl;
	}
private:

	Point<2> 			m_p1, m_p2, m_p3;
};







// a collection of Segments that connect
// this is a generalization of a polygon to permit curved segments
class Polycurve : public Primitive2D
{
public:

	Polycurve(){};

	Polycurve(const std::vector<std::shared_ptr<Segment<2>>> & segs)
	: m_segments(segs) {};

	virtual std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Polycurve>(*this);};

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

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		Hull<2> h1;
		h1.points.resize(npts);
		// should divide up each segment to its respective size
		std::cout << "Polycurve: get_outline not working! FIX ME" << std::endl;
		return {h1};
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
		// std::cout << "checking polycurve" << std::endl;

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

	virtual void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Polycurve>" << std::endl;
		for (auto n=0; n<m_segments.size(); n++){
			for (auto i=0; i<ntabs+1; i++) os << "\t" ;
			m_segments[n]->print_summary(os);
			os << std::endl;
		}
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Polycurve>" << std::endl;
	}

protected:

	std::vector<std::shared_ptr<Segment<2>>> 		m_segments;
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

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<Polygon>(*this);};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Polygon>" << std::endl;
		for (auto n=0; n<m_segments.size(); n++){
			for (auto i=0; i<ntabs+1; i++) os << "\t" ;
			m_segments[n]->print_summary(os);
			os << std::endl;
		}
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Polygon>" << std::endl;
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

	std::shared_ptr<Primitive2D> copy() const {return std::make_shared<RegularPolygon>(*this);};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<RegularPolygon>" << std::endl;
		for (auto n=0; n<m_segments.size(); n++){
			for (auto i=0; i<ntabs+1; i++) os << "\t" ;
			m_segments[n]->print_summary(os);
			os << std::endl;
		}
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</RegularPolygon>" << std::endl;

	}

};

}
#endif