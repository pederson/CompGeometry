#ifndef _PRIMITIVE3D_H
#define _PRIMITIVE3D_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

class Primitive3D
{
public:

	virtual std::shared_ptr<Primitive3D> copy() const = 0;

	virtual Box<3> get_bounding_box() const = 0;
	virtual void translate(const Point<3> & pt) = 0;
	// virtual void rotate(const Point<3> & axis, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	virtual bool contains_point(const Point<3> & pt) const = 0;

	bool contains_box(const Box<3> & bx) const{
		return contains_point(Point<3>(bx.lo.x[0], bx.lo.x[1], bx.hi.x[2])) &&
			   contains_point(Point<3>(bx.hi.x[0], bx.hi.x[1], bx.hi.x[2])) &&
			   contains_point(Point<3>(bx.lo.x[0], bx.hi.x[1], bx.hi.x[2])) &&
			   contains_point(Point<3>(bx.hi.x[0], bx.lo.x[1], bx.hi.x[2])) && 

			   contains_point(Point<3>(bx.lo.x[0], bx.lo.x[1], bx.lo.x[2])) &&
			   contains_point(Point<3>(bx.hi.x[0], bx.hi.x[1], bx.lo.x[2])) &&
			   contains_point(Point<3>(bx.lo.x[0], bx.hi.x[1], bx.lo.x[2])) &&
			   contains_point(Point<3>(bx.hi.x[0], bx.lo.x[1], bx.lo.x[2])) ;
	}

	bool collides_box(const Box<3> & bx) const{
		return contains_point(Point<3>(bx.lo.x[0], bx.lo.x[1], bx.hi.x[2])) ||
			   contains_point(Point<3>(bx.hi.x[0], bx.hi.x[1], bx.hi.x[2])) ||
			   contains_point(Point<3>(bx.lo.x[0], bx.hi.x[1], bx.hi.x[2])) ||
			   contains_point(Point<3>(bx.hi.x[0], bx.lo.x[1], bx.hi.x[2])) || 

			   contains_point(Point<3>(bx.lo.x[0], bx.lo.x[1], bx.lo.x[2])) ||
			   contains_point(Point<3>(bx.hi.x[0], bx.hi.x[1], bx.lo.x[2])) ||
			   contains_point(Point<3>(bx.lo.x[0], bx.hi.x[1], bx.lo.x[2])) ||
			   contains_point(Point<3>(bx.hi.x[0], bx.lo.x[1], bx.lo.x[2])) ;
	}

	virtual void print_summary(std::ostream & os) const = 0;
};


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

	void print_summary(std::ostream & os = std::cout) const{
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

	void print_summary(std::ostream & os = std::cout) const{
		os << "Cylinder: center = " ;
		os << m_plane.origin ;
		os << " radius = " << m_circle.radius() ;
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
		pp = 1.0/M*pp;
		return m_base->contains_point(pp);
	}

	void print_summary(std::ostream & os = std::cout) const{
		os << "Pyramid: center = " ;
		os << m_plane.origin ;
		os << " height = " << m_height ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	std::shared_ptr<Primitive2D> m_base;
	double m_height;
	Plane m_plane;

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

	void print_summary(std::ostream & os = std::cout) const{
		os << "Extrusion: center = " ;
		os << m_plane.origin ;
		os << " height = " << m_height ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	std::shared_ptr<Primitive2D> m_base;
	double m_height;
	Plane m_plane;

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

	// std::shared_ptr<Primitive3D> copy() const {return std::make_shared<Sweep>(*this);};

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
		return false;
	}

	void print_summary(std::ostream & os = std::cout) const{
		os << "Sweep: center = " ;
		os << m_plane.origin ;
		os << " base = " ;
		m_base->print_summary(os);
	}
private:

	std::shared_ptr<Primitive2D> m_base;
	double m_angle;
	Plane m_plane;
	Line<3> m_line;

};




/*
class TriangleMesh : public GeometricObject3D{
public:

	TriangleMesh();
	~TriangleMesh();

	void print_summary() const;
	static TriangleMesh * read_STL(std::string filename, unsigned int byte_offset=0){
		// declare vars
		TriangleMesh * outmesh;
		int fd;
		unsigned int tricount;
		char * stlmap;

		// open file and fast forward
		fd = open(filename.c_str(), O_RDONLY);
		if (fd < 0){
			cout << "Error opening file in read_STL" << endl;
		throw -1;
		}
		lseek(fd, byte_offset, SEEK_SET);

		// find the triangle count
		lseek(fd, 80, SEEK_CUR); // skip the header
		read(fd, &tricount, 4); // read the triangle count


	  lseek(fd, byte_offset, SEEK_CUR); // back to the beginning
		stlmap = (char *)mmap(NULL, 84 + sizeof(stl_tri)*tricount, PROT_READ, MAP_PRIVATE, fd, 0);
		if (stlmap == MAP_FAILED){
			cout << "Failed to map stl file" << endl;
		throw -1;
		}

		// copy the triangle data into structures
		stl_tri * triangles = new stl_tri[tricount];
		memcpy(triangles, &stlmap[84], sizeof(stl_tri)*tricount);

		// copy the structure data into the member data
		outmesh = new TriangleMesh();
		outmesh->triangle_count = tricount;
	  outmesh->vertex_count = 3*tricount;
		outmesh->vertices = new float[tricount*3*3];
		outmesh->normals = new float[tricount*3];
		outmesh->vertex_inds = new unsigned int[tricount*3];
		for (unsigned int i=0; i<tricount; i++){
			//cout << "I: " << i << " \r" << flush;

			outmesh->normals[i*3] = triangles[i].norm_x;
			outmesh->normals[i*3+1] = triangles[i].norm_y;
			outmesh->normals[i*3+2] = triangles[i].norm_z;

			outmesh->vertices[i*9] = triangles[i].v1_x;
			outmesh->vertices[i*9+1] = triangles[i].v1_y;
			outmesh->vertices[i*9+2] = triangles[i].v1_z;

			outmesh->vertices[i*9+3] = triangles[i].v2_x;
			outmesh->vertices[i*9+4] = triangles[i].v2_y;
			outmesh->vertices[i*9+5] = triangles[i].v2_z;

			outmesh->vertices[i*9+6] = triangles[i].v3_x;
			outmesh->vertices[i*9+7] = triangles[i].v3_y;
			outmesh->vertices[i*9+8] = triangles[i].v3_z;

			outmesh->vertex_inds[i*3] = i*3;
			outmesh->vertex_inds[i*3+1] = i*3+1;
			outmesh->vertex_inds[i*3+2] = i*3+2;
		}

	  if (munmap(stlmap, 84 + sizeof(stl_tri)*tricount) < 0){
	    cout << "ruh roh! problem unmapping STL file" << endl;
	    throw -1;
	  }
	  close(fd);

	  delete[] triangles;

		return outmesh;
	}
	};


	unsigned int * vertex_inds; // 3 inds per triangle
	float * vertices; //
	float * normals;
	unsigned int triangle_count, vertex_count;

private:
	//void read_STL_internal(std::string filename, unsigned int byte_offset=0);

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
*/

#endif