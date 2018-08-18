#ifndef _LINEARTRANSFORMATION_H
#define _LINEARTRANSFORMATION_H

#include "GeomUtils.hpp"
#include <memory>

namespace csg{



// shear map implemented as M = I + S, where I is diagonal and S is off-diagonals
struct ShearMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 		mL;

	ShearMap2D(const PointT & p) : mL(p) {};

	PointT inverse_map(const PointT & p) const{
		double det = (1-mL.x[0]*mL.x[1]);
		return 1.0/det*PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1]);
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
	PointT 		mL, mU;

	ShearMap3D(const PointT & U, const PointT & L) : mU(U), mL(L) {
		// std::cout << "3D Shear Map not working... fix me!" << std::endl;
		// throw -1;
	};

	PointT inverse_map(const PointT & p) const{
		double a=mU.x[0], b=mU.x[1], c=mU.x[2];
		double d=mL.x[0], e=mL.x[1], f=mL.x[2];
		double det = (1-c*f)-a*(d-e*c)+b*(d*f-e);//(1-mL.x[0]*mL.x[1]*mL.x[2]);
		return 1.0/det*PointT(p.x[0]*(1*1-f*c)-p.x[1]*(1*a-f*b)+p.x[2]*(c*a-1*b),
							 -p.x[0]*(1*d-e*c)+p.x[1]*(1*1-e*b)-p.x[2]*(c*1-d*b),
							  p.x[0]*(f*d-e*1)-p.x[1]*(f*1-e*a)+p.x[2]*(1*1-d*a));
	};

	PointT forward_map(const PointT & p) const{
		double a=mU.x[0], b=mU.x[1], c=mU.x[2];
		double d=mL.x[0], e=mL.x[1], f=mL.x[2];
		return PointT(p.x[0]+a*p.x[1]+b*p.x[2], d*p.x[0]+p.x[1]+c*p.x[2], e*p.x[0]+f*p.x[1]+p.x[2]);
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<ShearMapping>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Upper>" << mU << "</Upper>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Lower>" << mL << "</Lower>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</ShearMapping>" << std::endl;
	}
};







// Dilatation map implemented as M = I + D, where I is identity and D is diagonal
struct DilatationMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mL;

	DilatationMap2D(const PointT & p) : mL(p) {};

	PointT inverse_map(const PointT & p) const{
		double det = (1.0+mL.x[0])*(1.0+mL.x[1]);
		return 1.0/det*PointT(p.x[0]*(1.0+mL.x[1]), p.x[1]*(1.0+mL.x[0]));
	};

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0]*(1.0+mL.x[0]), p.x[1]*(1.0+mL.x[1]));
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<DilatationMapping>" << mL << "</DilatationMapping>" << std::endl;
	}
};



// rotation map implemented as M = D+A, where A is antisymmetric and D is diagonal matrix
struct RotationMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	double 		mTheta;

	RotationMap2D(double theta) : mTheta(theta) {};

	PointT inverse_map(const PointT & p) const{
		// double det = (1-mL.x[0]*mL.x[1]);
		return PointT(cos(mTheta)*p.x[0]+sin(mTheta)*p.x[1], cos(mTheta)*p.x[1]-sin(mTheta)*p.x[0]);
	};

	PointT forward_map(const PointT & p) const{
		return PointT(cos(mTheta)*p.x[0]-sin(mTheta)*p.x[1], cos(mTheta)*p.x[1]+sin(mTheta)*p.x[0]);
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<RotationMapping>" << mTheta << "</RotationMapping>" << std::endl;
	}
};


// Translation map implemented as M = I  and t = T, where I is identity and T is a vector
struct TranslationMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mL;

	TranslationMap2D(const PointT & p) : mL(p) {};

	PointT inverse_map(const PointT & p) const{
		return PointT(p.x[0] - mL.x[0], p.x[1] - mL.x[1]);
	};

	PointT forward_map(const PointT & p) const{
		return PointT(p.x[0] + mL.x[0], p.x[1] + mL.x[1]);
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<TranslationMapping>" << mL << "</TranslationMapping>" << std::endl;
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
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Primitive>" << std::endl;
		mPrim->print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Primitive>" << std::endl;
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
LinearTransformation<Primitive3D, ShearMap3D> shear_transformation(const DerivedType & c, const Point<3> & upper, const Point<3> & lower){
	return LinearTransformation<Primitive3D, ShearMap3D>(c.copy(), ShearMap3D(upper, lower));
}





template <typename DerivedType>
LinearTransformation<Primitive2D, DilatationMap2D> dilatation_transformation(const DerivedType & c, const Point<2> & p){
	return LinearTransformation<Primitive2D, DilatationMap2D>(c.copy(), DilatationMap2D(p));
}





template <typename DerivedType>
LinearTransformation<Primitive2D, RotationMap2D> rotation_transformation(const DerivedType & c, double theta){
	return LinearTransformation<Primitive2D, RotationMap2D>(c.copy(), RotationMap2D(theta));
}



template <typename DerivedType>
LinearTransformation<Primitive2D, TranslationMap2D> translation_transformation(const DerivedType & c, const Point<2> & p){
	return LinearTransformation<Primitive2D, TranslationMap2D>(c.copy(), TranslationMap2D(p));
}
}
#endif