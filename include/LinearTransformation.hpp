#ifndef _LINEARTRANSFORMATION_H
#define _LINEARTRANSFORMATION_H

#include "GeomUtils.hpp"
#include <memory>

namespace csg{




struct ShearMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 		mL;

	ShearMap2D(const PointT & p) : mL(p) {};

	PointT inverse_map(const PointT & p) const{
		double det = (1-mL.x[0]*mL.x[1]);
		return PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1]);
	};

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0]);
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<ShearMapping>" << mL << "</ShearMapping>" << std::endl;
	}
};



struct ShearMap3D{
public:
	typedef Point<3> 					PointT;
	typedef Box<3>						BoxT;
	PointT 		mL;

	ShearMap3D(const PointT & p) : mL(p) {
		std::cout << "3D Shear Map not working... fix me!" << std::endl;
		throw -1;
	};

	PointT inverse_map(const PointT & p) const{
		double det = (1-mL.x[0]*mL.x[1]*mL.x[2]);
		return PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1], 0);
	};

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0], 0);
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<ShearMapping>" << mL << "</ShearMapping>" << std::endl;
	}
};




template <class PrimitiveType, class MapPolicy>
class LinearTransformation : public PrimitiveType
{
public:
	typedef LinearTransformation 		SelfT;
	typedef PrimitiveType				PrimitiveT;
	typedef typename MapPolicy::PointT 	PointT;
	typedef typename MapPolicy::BoxT	BoxT;


	MapPolicy 							mMap; // holds the components of the transformation
	std::shared_ptr<PrimitiveT>			mPrim;	// holds the primitive object


	LinearTransformation(std::shared_ptr<PrimitiveT> prim, const MapPolicy & m)
	: mPrim(prim), mMap(m) {};//{mPrim = std::make_shared<PrimitiveT>(prim

	LinearTransformation(const LinearTransformation & t)
	: mPrim(t.mPrim), mMap(t.mMap) {};


	///////////// functions that implement the Base Interface
	std::shared_ptr<PrimitiveT> copy() const {
		return std::make_shared<SelfT>(*this);
	}

	BoxT get_bounding_box() const {
		BoxT bb = mPrim->get_bounding_box();
		return BoxT(mMap.forward_map(bb.lo), mMap.forward_map(bb.hi));
	}

	// this is only for 2D
	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		std::vector<Hull<2>> o = mPrim->get_outline(npts);
		for (auto it=o.begin(); it!=o.end(); it++){
			for (auto p=it->points.begin(); p!=it->points.end(); p++){
				*p = mMap.forward_map(*p);
			}
		}
		return o;
	}

	bool contains_point(const PointT & pt) const {
		return mPrim->contains_point(mMap.inverse_map(pt));
	}

	// bool contains_box(const BoxT & bx) const {
	// 	return this->contains_point(bx.lo) && this->contains_point(bx.hi) &&
	// 		   this->contains_point(PointT(bx.lo.x[0], bx.hi.x[1])) &&
	// 		   this->contains_point(PointT(bx.hi.x[0], bx.lo.x[1]));
	// }

	// bool collides_box(const BoxT & bx) const {
	// 	return this->contains_point(bx.lo) || this->contains_point(bx.hi) ||
	// 		   this->contains_point(PointT(bx.lo.x[0], bx.hi.x[1])) ||
	// 		   this->contains_point(PointT(bx.hi.x[0], bx.lo.x[1]));
	// }

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<LinearTransformation>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Mapping>" << std::endl;
		mMap.print_summary(os, ntabs+1);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Mapping>" << std::endl;
		mPrim->print_summary(os, ntabs+1);
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</LinearTransformation>" << std::endl;
	}
	///////////////////////////////////////
};



template <typename DerivedType>
LinearTransformation<Primitive2D, ShearMap2D> shear_transformation(const DerivedType & c, const Point<2> & p){
	return LinearTransformation<Primitive2D, ShearMap2D>(c.copy(), ShearMap2D(p));
}

template <typename DerivedType>
LinearTransformation<Primitive3D, ShearMap3D> shear_transformation(const DerivedType & c, const Point<3> & p){
	return LinearTransformation<Primitive3D, ShearMap3D>(c.copy(), ShearMap3D(p));
}



}
#endif