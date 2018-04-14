#ifndef _PRIMITIVETYPES_H
#define _PRIMITIVETYPES_H

#include "GeomUtils.hpp"
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

	// // get outline
	virtual std::vector<Hull<2>> get_outline(unsigned int npts) const = 0;

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