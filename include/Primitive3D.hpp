#ifndef _PRIMITIVE3D_H
#define _PRIMITIVE3D_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"
#include "CSGeometry2D.hpp"
#include "PrimitiveTypes.hpp"

namespace csg {


class Sphere : public Primitive3D
{
public:

	Sphere(){};

	Sphere(const Point<3> & center, double radius)
	: m_center(center), m_radius(radius) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Sphere>(*this);};

	Box<3> get_bounding_box() const {
		return Box<3>(Point<3>(m_center.x[0]-m_radius, m_center.x[1]-m_radius, m_center.x[2]-m_radius),
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

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Sphere>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_center << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Radius>" << m_radius << "</Radius>" << std::endl ;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Sphere>" << std::endl;
	}
private:

	double 				m_radius;
	Point<3> 			m_center;
};






class Cylinder : public Primitive3D
{
public:

	Cylinder(){};

	Cylinder(const Point<3> & center, const Point<3> & normal, const Point<3> & px, double radius, double height)
	: m_plane(Plane(center, normal, px))
	, m_circle(Circle(Point<2>(0,0), radius))
	, m_height(height) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Cylinder>(*this);};

	Box<3> get_bounding_box() const {
		Point<3> yhat = (cross(m_plane.normal, m_plane.posx)).normalize();
		Point<3> xhat = (m_plane.posx).normalize();
		Point<3> nhat = (m_plane.normal).normalize();

		Box<2> basebox = m_circle.get_bounding_box();

		// get the 8 box points in 3D space
		std::vector<Point<3>> bpts(8);
		bpts[0] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[1] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[2] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;
		bpts[3] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;

		bpts[4] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[5] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[6] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;
		bpts[7] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;

		Point<3> lo = bpts[0];
		Point<3> hi = bpts[0];
		for (auto i=0; i<8; i++){
			lo.x[0] = std::min(lo.x[0], bpts[i].x[0]);
			lo.x[1] = std::min(lo.x[1], bpts[i].x[1]);
			lo.x[2] = std::min(lo.x[2], bpts[i].x[2]);
			hi.x[0] = std::max(hi.x[0], bpts[i].x[0]);
			hi.x[1] = std::max(hi.x[1], bpts[i].x[1]);
			hi.x[2] = std::max(hi.x[2], bpts[i].x[2]);
		}
		return Box<3>(lo,hi);
	}

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {
	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point<3>::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		return m_circle.contains_point(pp);
	}

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Cylinder>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_plane.origin << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Normal>" << m_plane.normal << "</Normal>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<XDir>" << m_plane.posx << "</XDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Radius>" << m_circle.radius() << "</Radius>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Height>" << m_height << "</Height>" << std::endl ;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Cylinder>" << std::endl;
	}
private:

	double 			m_height;
	Plane 			m_plane;
	Circle 			m_circle;
};











class RectangularPrism : public Primitive3D
{
public:

	RectangularPrism(){};

	RectangularPrism(const Point<3> & center, const Point<3> & normal, const Point<3> & px, const Point<2> & rdims, double height)
	: m_plane(Plane(center, normal, px))
	, m_rect(Rectangle(Point<2>(0,0), rdims))
	, m_height(height) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<RectangularPrism>(*this);};

	Box<3> get_bounding_box() const {
		Point<3> yhat = (cross(m_plane.normal, m_plane.posx)).normalize();
		Point<3> xhat = (m_plane.posx).normalize();
		Point<3> nhat = (m_plane.normal).normalize();

		Box<2> basebox = m_rect.get_bounding_box();

		// get the 8 box points in 3D space
		std::vector<Point<3>> bpts(8);
		bpts[0] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[1] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[2] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;
		bpts[3] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;

		bpts[4] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[5] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[6] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;
		bpts[7] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;

		Point<3> lo = bpts[0];
		Point<3> hi = bpts[0];
		for (auto i=0; i<8; i++){
			lo.x[0] = std::min(lo.x[0], bpts[i].x[0]);
			lo.x[1] = std::min(lo.x[1], bpts[i].x[1]);
			lo.x[2] = std::min(lo.x[2], bpts[i].x[2]);
			hi.x[0] = std::max(hi.x[0], bpts[i].x[0]);
			hi.x[1] = std::max(hi.x[1], bpts[i].x[1]);
			hi.x[2] = std::max(hi.x[2], bpts[i].x[2]);
		}
		return Box<3>(lo,hi);
	}

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {
	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point<3>::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		return m_rect.contains_point(pp);
	}

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<RectangularPrism>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_plane.origin << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Normal>" << m_plane.normal << "</Normal>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<XDir>" << m_plane.posx << "</XDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Dims>" << m_rect.dims() << "</Dims>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Height>" << m_height << "</Height>" << std::endl;

		// m_rect.print_summary(os, ntabs+1);

		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</RectangularPrism>" << std::endl;
	}
private:

	double 			m_height;
	Plane 			m_plane;
	Rectangle 		m_rect;
};









class Pyramid : public Primitive3D
{
public:

	Pyramid(){};

	Pyramid(const Primitive2D & base, const Point<3> & center, const Point<3> & normal, const Point<3> & px, double height)
	: m_plane(Plane(center, normal, px))
	, m_base(base.copy())
	, m_height(height) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Pyramid>(*this);};

	Box<3> get_bounding_box() const {
		Point<3> yhat = (cross(m_plane.normal, m_plane.posx)).normalize();
		Point<3> xhat = (m_plane.posx).normalize();
		Point<3> nhat = (m_plane.normal).normalize();
		Box<2> basebox = m_base->get_bounding_box();

		// get the 8 box points in 3D space
		std::vector<Point<3>> bpts(8);
		bpts[0] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[1] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[2] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;
		bpts[3] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;

		bpts[4] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[5] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[6] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;
		bpts[7] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;

		Point<3> lo = bpts[0];
		Point<3> hi = bpts[0];
		for (auto i=0; i<8; i++){
			lo.x[0] = std::min(lo.x[0], bpts[i].x[0]);
			lo.x[1] = std::min(lo.x[1], bpts[i].x[1]);
			lo.x[2] = std::min(lo.x[2], bpts[i].x[2]);
			hi.x[0] = std::max(hi.x[0], bpts[i].x[0]);
			hi.x[1] = std::max(hi.x[1], bpts[i].x[1]);
			hi.x[2] = std::max(hi.x[2], bpts[i].x[2]);
		}
		return Box<3>(lo,hi);
	}

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point<3>::dot(ptvec, m_plane.normal);
		if (proj >= m_height || proj < 0) return false;

		// now project the point onto the plane
		double M = 1.0/(1.0-proj/m_height); // need to scale the plane projection b/c pyramid shape
		Point<2> pp = m_plane.project(pt);
		pp = M*pp;
		return m_base->contains_point(pp);
	}

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Pyramid>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_plane.origin << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Normal>" << m_plane.normal << "</Normal>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<XDir>" << m_plane.posx << "</XDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Base>" << std::endl;
		m_base->print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Base>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Height>" << m_height << "</Height>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Pyramid>" << std::endl;
	}
private:

	std::shared_ptr<Primitive2D> 	m_base;
	double 							m_height;
	Plane 							m_plane;

};







class Extrusion : public Primitive3D
{
public:

	Extrusion(){};

	Extrusion(const Primitive2D & base, const Point<3> & center, const Point<3> & normal, const Point<3> & px, double height)
	: m_plane(Plane(center, normal, px))
	, m_base(base.copy())
	, m_height(height) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Extrusion>(*this);};

	Box<3> get_bounding_box() const {
		Point<3> yhat = (cross(m_plane.normal, m_plane.posx)).normalize();
		Point<3> xhat = (m_plane.posx).normalize();
		Point<3> nhat = (m_plane.normal).normalize();
		Box<2> basebox = m_base->get_bounding_box();

		// get the 8 box points in 3D space
		std::vector<Point<3>> bpts(8);
		bpts[0] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[1] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin;
		bpts[2] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;
		bpts[3] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin;

		bpts[4] = xhat*basebox.lo.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[5] = xhat*basebox.hi.x[0] + yhat*basebox.lo.x[1] + m_plane.origin + m_height*nhat;
		bpts[6] = xhat*basebox.hi.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;
		bpts[7] = xhat*basebox.lo.x[0] + yhat*basebox.hi.x[1] + m_plane.origin + m_height*nhat;

		Point<3> lo = bpts[0];
		Point<3> hi = bpts[0];
		for (auto i=0; i<8; i++){
			lo.x[0] = std::min(lo.x[0], bpts[i].x[0]);
			lo.x[1] = std::min(lo.x[1], bpts[i].x[1]);
			lo.x[2] = std::min(lo.x[2], bpts[i].x[2]);
			hi.x[0] = std::max(hi.x[0], bpts[i].x[0]);
			hi.x[1] = std::max(hi.x[1], bpts[i].x[1]);
			hi.x[2] = std::max(hi.x[2], bpts[i].x[2]);
		}
		return Box<3>(lo,hi);	}

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// check projection of point onto normal
		Point<3> ptvec(pt.x[0]-m_plane.origin.x[0], pt.x[1]-m_plane.origin.x[1], pt.x[2]-m_plane.origin.x[2]);
		double proj = Point<3>::dot(ptvec, m_plane.normal);
		if (proj > m_height || proj < 0) return false;

		// now project the point onto the plane
		Point<2> pp = m_plane.project(pt);
		return m_base->contains_point(pp);
	}

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Extrusion>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Base>" << std::endl;
		m_base->print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Base>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_plane.origin << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Normal>" << m_plane.normal << "</Normal>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<XDir>" << m_plane.posx << "</XDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Height>" << m_height << "</Height>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Extrusion>" << std::endl;
	}
private:

	std::shared_ptr<Primitive2D> 	m_base;
	double 							m_height;
	Plane 							m_plane;

};







class Sweep : public Primitive3D
{
public:

	Sweep(){};

	Sweep(const Primitive2D & base, const Point<3> & center, const Point<3> & normal, const Point<3> & px, Line<3> ln, double angle)
	: m_plane(Plane(center, normal, px))
	, m_base(base.copy())
	, m_line(ln)
	, m_angle(angle) {};

	std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Sweep>(*this);};

	Box<3> get_bounding_box() const {
		Point<3> o = m_plane.origin - m_line.pt;
		double distsq = Point<3>::dot(o,o);
		Point<2> dims = m_base->get_bounding_box().hi - m_base->get_bounding_box().lo;
		double maxdim = std::max(dims.x[0], dims.x[1]);
		return Box<3>(m_line.pt+(2*sqrt(distsq)+maxdim/4)*Point<3>(1,1,1),
					  m_line.pt-(2*sqrt(distsq)+maxdim/4)*Point<3>(1,1,1));
	}

	void translate(const Point<3> & pt) {
		m_plane.origin = m_plane.origin + pt;
		m_line.pt = m_line.pt + pt;
	}

	// void rotate(const Point<3> & axis, double degrees) {

	// }

	bool contains_point(const Point<3> & pt) const{
		// center everything around the line point
		Point<3> rho = pt - m_line.pt;
		Point<3> o = m_plane.origin - m_line.pt;
		Point<3> odir;

		Point<3> yhat;
		if (o.norm() < 1e-16){
			yhat = m_plane.normal;
			odir = cross(yhat, m_line.dir).normalize();
		}
		else {
			yhat = (cross(m_line.dir, o)).normalize();
			odir = o.normalize();
		}

		// project onto line
		Box<2> bb = m_base->get_bounding_box();
		double pproj_l = Point<3>::dot(rho, m_line.dir);
		// subtract out the l component of rho
		Point<3> p_yo = rho - pproj_l*m_line.dir;

		// project rho onto the y-o plane
		double theta = atan2(Point<3>::dot(p_yo, yhat), Point<3>::dot(p_yo, odir));
		double ang = theta*180.0/3.14159265;
		if (ang < 0) ang += 360.0;
		if (ang > m_angle) return false;

		// rotate rho to the l-o plane
		theta = -theta;
		Point<3> u = m_line.dir;
		Point<3> p_lo(rho.x[0]*(cos(theta) + u.x[0]*u.x[0]*(1.0-cos(theta))) 		+ rho.x[1]*(u.x[0]*u.x[1]*(1-cos(theta)) - u.x[2]*sin(theta)) 	+ rho.x[2]*(u.x[0]*u.x[2]*(1-cos(theta))+u.x[1]*sin(theta)),
					   rho.x[0]*(u.x[1]*u.x[0]*(1-cos(theta))+u.x[2]*sin(theta)) 	+ rho.x[1]*(cos(theta) + u.x[1]*u.x[1]*(1.0-cos(theta))) 		+ rho.x[2]*(u.x[1]*u.x[2]*(1-cos(theta))+u.x[0]*sin(theta)),
					   rho.x[0]*(u.x[2]*u.x[0]*(1-cos(theta))+u.x[1]*sin(theta)) 	+ rho.x[1]*(u.x[2]*u.x[1]*(1-cos(theta))+u.x[0]*sin(theta)) 	+ rho.x[2]*(cos(theta) + u.x[2]*u.x[2]*(1.0-cos(theta)))
					   );
		Point<3> p_lo_mo = p_lo - o;
		Point<2> dprime(Point<3>::dot(p_lo_mo, odir), Point<3>::dot(p_lo_mo, u));
		return m_base->contains_point(dprime);
	}

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		// os << "Sweep: center = " ;
		// os << m_plane.origin ;
		// os << " base = " << std::endl;
		// m_base->print_summary(os,ntabs+1);

		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Sweep>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Base>" << std::endl;
		m_base->print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Base>" << std::endl ;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << m_plane.origin << "</Center>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Normal>" << m_plane.normal << "</Normal>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<XDir>" << m_plane.posx << "</XDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<LinePoint>" << m_line.pt << "</LinePoint>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<LineDir>" << m_line.dir << "</LineDir>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Angle>" << m_angle << "</Angle>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Sweep>" << std::endl;
	}
private:

	std::shared_ptr<Primitive2D> 		m_base;
	double 								m_angle;	// sweep angle in degrees
	Plane 								m_plane;
	Line<3> 							m_line;

};




} // end namespace csg

#endif