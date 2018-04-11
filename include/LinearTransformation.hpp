#ifndef _LINEARTRANSFORMATION_H
#define _LINEARTRANSFORMATION_H

#include "GeomUtils.hpp"
#include <memory>

namespace csg{




struct ShearMap{
public:
	typedef Point<2> 					PointT;

	PointT 		mL;

	PointT inverse_map(const PointT & p) const{
		double det = (1-mL.x[0]*mL.x[1]);
		return PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1]);
	};

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0]);
	};
};


template <class BaseInterface, class PrimitiveType>
class LinearTransformation : public BaseInterface
{
public:
	typedef LinearTransformation 		SelfT;
	typedef PrimitiveType				PrimitiveT;
	typedef Point<2> 					PointT;
	typedef Box<2> 						BoxT;

	PointT 								mL; // holds the components of the transformation
	PrimitiveT 							mPrim;	// holds the primitive object

	// implement a shearing without volume change
	LinearTransformation(const PrimitiveT & prim, const PointT & p)
	: mPrim(prim), mL(p) {};


	///// MAPPING FUNCTIONS
	PointT inverse_map(const PointT & p) const{
		double det = (1-mL.x[0]*mL.x[1]);
		return PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1]);
	}

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0]);
	}
	////////////////////////


	///////////// functions that implement the Base Interface
	std::shared_ptr<BaseInterface> copy() const {
		return std::make_shared<SelfT>(*this);
	}

	BoxT get_bounding_box() const {
		BoxT bb = mPrim.get_bounding_box();
		return BoxT(forward_map(bb.lo), forward_map(bb.hi));
	}

	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		std::vector<Hull<2>> o = mPrim.get_outline(npts);
		for (auto it=o.begin(); it!=o.end(); it++){
			for (auto p=it->points.begin(); p!=it->points.end(); p++){
				*p = forward_map(*p);
			}
		}
		return o;
	}

	bool contains_point(const PointT & pt) const {
		return mPrim.contains_point(inverse_map(pt));
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
		os << "<Matrix>" << mL << "</Matrix>" << std::endl;
		mPrim.print_summary(os, ntabs+1);
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</LinearTransformation>" << std::endl;
	}
	///////////////////////////////////////
};



template <typename PrimitiveType, typename PointType>
LinearTransformation<typename PrimitiveType::BaseType, PrimitiveType> shear_transformation(const PrimitiveType & c, const PointType & p){
	return LinearTransformation<typename PrimitiveType::BaseType, PrimitiveType>(c, p);
}

}
#endif