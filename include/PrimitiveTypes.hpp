#ifndef _PRIMITIVETYPES_H
#define _PRIMITIVETYPES_H

#include "GeomUtils.hpp"
#include "Delaunay.hpp"
#include <memory>

namespace csg{


template <class T, std::size_t dim>
struct BoxCollision {};

template <class T, std::size_t dim>
struct BoxContainment {};





template <std::size_t dim>
class PrimitiveGeometry{
public:
	typedef PrimitiveGeometry 			BaseT;
	typedef Box<dim>					BoxT;
	typedef Point<dim>					PointT;

	// copy to shared_ptr of base type
	virtual std::shared_ptr<BaseT> copy() const = 0;

	// get bounding box
	virtual BoxT get_bounding_box() const = 0;

	// contains point
	virtual bool contains_point(const PointT & pt) const = 0;

	// contains box
	virtual bool contains_box(const BoxT & bx) const = 0;

	// collides with box
	virtual bool collides_box(const BoxT & bx) const = 0;

	virtual void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const = 0;
};




template<>
class PrimitiveGeometry<2>{
public:
	typedef PrimitiveGeometry<2>		BaseT;
	typedef Box<2>						BoxT;
	typedef Point<2>					PointT;

	// copy to shared_ptr of base type
	virtual std::shared_ptr<BaseT> copy() const = 0;

	// get bounding box
	virtual BoxT get_bounding_box() const = 0;

	// get outline
	virtual std::vector<Hull<2>> get_outline(unsigned int npts) const = 0;

	// get outline points
	std::vector<Point<2>> get_outline_points(unsigned int npts) const {
		std::vector<Hull<2>> hv = get_outline(npts);
		std::vector<csg::Point<2>> pts;
		for (auto i=0; i<hv.size(); i++) pts.insert(pts.end(), hv[i].points.begin(), hv[i].points.end());
		return pts;
	}

	// get a triangulation of the object
	Triangulation<2> get_triangulation(unsigned int npts) const {
		// first get set of points that define the outline
		Triangulation<2> tout;
		tout.points = get_outline_points(npts);

		// do a Delaunay triangulation of these points
		Delaunay del(tout.points, 1);

		// go through the triangulation and remove triangles with
		// 2 or more edges whose centerpoint is not contained 
		// within the geometry
		unsigned int ct;
		for (auto it=del.triangles.begin(); it != del.triangles.end(); it++){
			if (it->state <= 0) continue;

			ct=0;
			if (!contains_point(0.5*(del.points[it->vertices[0]]+del.points[it->vertices[1]]))) ct++;
			if (!contains_point(0.5*(del.points[it->vertices[1]]+del.points[it->vertices[2]]))) ct++;
			if (!contains_point(0.5*(del.points[it->vertices[2]]+del.points[it->vertices[0]]))) ct++;
			if (!contains_point(1.0/3.0*(del.points[it->vertices[0]]+del.points[it->vertices[1]]+del.points[it->vertices[2]]))) ct++;

			if (ct > 1){
				del.erase_triangle(it->vertices[0], it->vertices[1], it->vertices[2],
								   -1, -1, -1);
			}
		}

		unsigned int pos = 0;
		tout.triangles.resize(del.ntri);
		for (auto it=del.triangles.begin(); it != del.triangles.end(); it++){
			if (it->state <= 0) continue;

			tout.triangles[pos] = {static_cast<int>(it->vertices[0]), static_cast<int>(it->vertices[1]), static_cast<int>(it->vertices[2])};
			pos++;
		}

		return tout;
	}

	// contains point
	virtual bool contains_point(const PointT & pt) const = 0;

	// contains box
	bool contains_box(const BoxT & bx) const{
		return contains_point(bx.lo) && contains_point(bx.hi) &&
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1])) &&
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1]));
	};

	// collides box
	bool collides_box(const BoxT & bx) const{
		return contains_point(bx.lo) || contains_point(bx.hi) ||
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1])) ||
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1]));
	};

	virtual void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const = 0;
};



template<>
class PrimitiveGeometry<3>{
public:
	typedef PrimitiveGeometry<3>		BaseT;
	typedef Box<3>						BoxT;
	typedef Point<3>					PointT;

	// copy to shared_ptr of base type
	virtual std::shared_ptr<BaseT> copy() const = 0;

	// get bounding box
	virtual BoxT get_bounding_box() const = 0;

	// contains point
	virtual bool contains_point(const PointT & pt) const = 0;

	// contains box
	bool contains_box(const BoxT & bx) const{
		return contains_point(PointT(bx.lo.x[0], bx.lo.x[1], bx.hi.x[2])) &&
			   contains_point(PointT(bx.hi.x[0], bx.hi.x[1], bx.hi.x[2])) &&
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1], bx.hi.x[2])) &&
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1], bx.hi.x[2])) && 

			   contains_point(PointT(bx.lo.x[0], bx.lo.x[1], bx.lo.x[2])) &&
			   contains_point(PointT(bx.hi.x[0], bx.hi.x[1], bx.lo.x[2])) &&
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1], bx.lo.x[2])) &&
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1], bx.lo.x[2])) ;
	}

	// collides box
	bool collides_box(const BoxT & bx) const{
		return contains_point(PointT(bx.lo.x[0], bx.lo.x[1], bx.hi.x[2])) ||
			   contains_point(PointT(bx.hi.x[0], bx.hi.x[1], bx.hi.x[2])) ||
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1], bx.hi.x[2])) ||
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1], bx.hi.x[2])) || 

			   contains_point(PointT(bx.lo.x[0], bx.lo.x[1], bx.lo.x[2])) ||
			   contains_point(PointT(bx.hi.x[0], bx.hi.x[1], bx.lo.x[2])) ||
			   contains_point(PointT(bx.lo.x[0], bx.hi.x[1], bx.lo.x[2])) ||
			   contains_point(PointT(bx.hi.x[0], bx.lo.x[1], bx.lo.x[2])) ;
	}

	virtual void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const = 0;
};


typedef PrimitiveGeometry<2>	Primitive2D;
typedef PrimitiveGeometry<3> 	Primitive3D;



template <typename Primitive>
struct BoxTypedef{};
template<> struct BoxTypedef<Primitive2D> {typedef Box<2> type;};
template<> struct BoxTypedef<Primitive3D> {typedef Box<3> type;};

template <typename Primitive>
struct PointTypedef{};
template<> struct PointTypedef<Primitive2D> {typedef Point<2> type;};
template<> struct PointTypedef<Primitive3D> {typedef Point<3> type;};


} // end namespace csg
#endif