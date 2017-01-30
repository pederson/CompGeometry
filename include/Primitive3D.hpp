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

	  /*
	  for (unsigned int i=0; i<20; i++){
	    cout << " TRIANGLES PREVIEW" << endl;
	    cout << "vertex: " << outmesh
	  }
	  */

	  if (munmap(stlmap, 84 + sizeof(stl_tri)*tricount) < 0){
	    cout << "ruh roh! problem unmapping STL file" << endl;
	    throw -1;
	  }
	  close(fd);

	  delete[] triangles;

		return outmesh;
	}
	}


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

#endif