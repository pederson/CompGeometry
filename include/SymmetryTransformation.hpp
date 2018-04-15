#ifndef _SYMMETRYTRANSFORMATION_H
#define _SYMMETRYTRANSFORMATION_H

#include "GeomUtils.hpp"
#include <memory>

namespace csg{



// // shear map implemented as M = I + S, where I is diagonal and S is off-diagonals
// struct ShearMap2D{
// public:
// 	typedef Point<2> 					PointT;
// 	typedef Box<2>						BoxT;
// 	PointT 		mL;

// 	ShearMap2D(const PointT & p) : mL(p) {};

// 	PointT inverse_map(const PointT & p) const{
// 		double det = (1-mL.x[0]*mL.x[1]);
// 		return 1.0/det*PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1]);
// 	};

// 	PointT forward_map(const PointT & p) const{
// 		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0]);
// 	};

// 	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
// 		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
// 		os << "<ShearMapping>" << mL << "</ShearMapping>" << std::endl;
// 	}
// };



// struct ShearMap3D{
// public:
// 	typedef Point<3> 					PointT;
// 	typedef Box<3>						BoxT;
// 	PointT 		mL;

// 	ShearMap3D(const PointT & p) : mL(p) {
// 		std::cout << "3D Shear Map not working... fix me!" << std::endl;
// 		throw -1;
// 	};

// 	PointT inverse_map(const PointT & p) const{
// 		double det = (1-mL.x[0]*mL.x[1]*mL.x[2]);
// 		return 1.0/det*PointT(p.x[0]-p.x[1]*mL.x[0], p.x[1]-p.x[0]*mL.x[1], 0);
// 	};

// 	PointT forward_map(const PointT & p) const{
// 		return PointT(p.x[0]+mL.x[0]*p.x[1], p.x[1] + mL.x[1]*p.x[0], 0);
// 	};

// 	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
// 		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
// 		os << "<ShearMapping>" << mL << "</ShearMapping>" << std::endl;
// 	}
// };







// // Dilatation map implemented as M = I + D, where I is identity and D is diagonal
// struct DilatationMap2D{
// public:
// 	typedef Point<2> 					PointT;
// 	typedef Box<2>						BoxT;
// 	PointT 								mL;

// 	DilatationMap2D(const PointT & p) : mL(p) {};

// 	PointT inverse_map(const PointT & p) const{
// 		double det = (1.0+mL.x[0])*(1.0+mL.x[1]);
// 		return 1.0/det*PointT(p.x[0]*(1.0+mL.x[1]), p.x[1]*(1.0+mL.x[0]));
// 	};

// 	PointT forward_map(const PointT & p) const{
// 		return PointT(p.x[0]*(1.0+mL.x[0]), p.x[1]*(1.0+mL.x[1]));
// 	};

// 	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
// 		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
// 		os << "<DilatationMapping>" << mL << "</DilatationMapping>" << std::endl;
// 	}
// };



// // rotation map implemented as M = D+A, where A is antisymmetric and D is diagonal matrix
// struct RotationMap2D{
// public:
// 	typedef Point<2> 					PointT;
// 	typedef Box<2>						BoxT;
// 	double 		mTheta;

// 	RotationMap2D(double theta) : mTheta(theta) {};

// 	PointT inverse_map(const PointT & p) const{
// 		// double det = (1-mL.x[0]*mL.x[1]);
// 		return PointT(cos(mTheta)*p.x[0]+sin(mTheta)*p.x[1], cos(mTheta)*p.x[1]-sin(mTheta)*p.x[0]);
// 	};

// 	PointT forward_map(const PointT & p) const{
// 		return PointT(cos(mTheta)*p.x[0]-sin(mTheta)*p.x[1], cos(mTheta)*p.x[1]+sin(mTheta)*p.x[0]);
// 	};

// 	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
// 		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
// 		os << "<RoataionMapping>" << mTheta << "</RoataionMapping>" << std::endl;
// 	}
// };


// Translation symmetry map implemented as M = I  and t = T, where I is identity and T is a vector
struct TranslationSymmetryMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mCen, mSvec;

	TranslationSymmetryMap2D(const PointT & t, const PointT & c) : mSvec(t), mCen(c) {};

	PointT inverse_map(const PointT & p) const{
		PointT v = p-mCen;
		double amagn = mSvec.norm();
		double proj = PointT::dot(v, mSvec);
		double vmagn = v.norm();
		while (fabs(proj/(amagn*amagn)) > 0.5){
			// if (fabs(proj/amagn) > 1.0) std::cout << "proj/a: " << proj/amagn << std::endl;
			v = v - static_cast<double>(sgn(proj))*mSvec;
			proj = PointT::dot(v, mSvec);
			vmagn = v.norm();
		}
		// std::cout << "end" << std::endl;
		return v;
	};

	PointT forward_map(const PointT & p) const{
		return p;
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<TranslationSymmetryMapping>" << mSvec << ", " << mCen << "</TranslationSymmetryMapping>" << std::endl;
	}
};



template <class PrimitiveType, class MapPolicy>
class SymmetryTransformation : public PrimitiveType
{
public:
	typedef SymmetryTransformation 		SelfT;
	typedef PrimitiveType				PrimitiveT;
	typedef typename MapPolicy::PointT 	PointT;
	typedef typename MapPolicy::BoxT	BoxT;


	MapPolicy 							mMap; // holds the components of the transformation
	std::shared_ptr<PrimitiveT>			mPrim;	// holds the primitive object


	SymmetryTransformation(std::shared_ptr<PrimitiveT> prim, const MapPolicy & m)
	: mPrim(prim), mMap(m) {};//{mPrim = std::make_shared<PrimitiveT>(prim

	SymmetryTransformation(const SymmetryTransformation & t)
	: mPrim(t.mPrim), mMap(t.mMap) {};


	///////////// functions that implement the Base Interface
	std::shared_ptr<PrimitiveT> copy() const {
		return std::make_shared<SelfT>(*this);
	}

	BoxT get_bounding_box() const {
		// BoxT bb = mPrim->get_bounding_box();
		// return BoxT(mMap.forward_map(bb.lo), mMap.forward_map(bb.hi));
		return BoxT(PointT(-std::numeric_limits<double>::infinity(),
						   -std::numeric_limits<double>::infinity(), 
						   -std::numeric_limits<double>::infinity()), 
					PointT(std::numeric_limits<double>::infinity(), 
						   std::numeric_limits<double>::infinity(), 
						   std::numeric_limits<double>::infinity()));
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


	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<SymmetryTransformation>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Mapping>" << std::endl;
		mMap.print_summary(os, ntabs+1);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Mapping>" << std::endl;
		mPrim->print_summary(os, ntabs+1);
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</SymmetryTransformation>" << std::endl;
	}
	///////////////////////////////////////
};



template <typename DerivedType>
SymmetryTransformation<Primitive2D, TranslationSymmetryMap2D> translation_symmetry(const DerivedType & c, const Point<2> & svec){
	Box<2> bx = c.get_bounding_box();
	Point<2> center = 0.5*(bx.hi+bx.lo);
	return SymmetryTransformation<Primitive2D, TranslationSymmetryMap2D>(c.copy(), TranslationSymmetryMap2D(svec, center));
}

// template <typename DerivedType>
// SymmetryTransformation<Primitive3D, ShearMap3D> shear_transformation(const DerivedType & c, const Point<3> & p){
// 	return SymmetryTransformation<Primitive3D, ShearMap3D>(c.copy(), ShearMap3D(p));
// }





// template <typename DerivedType>
// SymmetryTransformation<Primitive2D, DilatationMap2D> dilatation_transformation(const DerivedType & c, const Point<2> & p){
// 	return SymmetryTransformation<Primitive2D, DilatationMap2D>(c.copy(), DilatationMap2D(p));
// }





// template <typename DerivedType>
// SymmetryTransformation<Primitive2D, RotationMap2D> rotation_transformation(const DerivedType & c, double theta){
// 	return SymmetryTransformation<Primitive2D, RotationMap2D>(c.copy(), RotationMap2D(theta));
// }



// template <typename DerivedType>
// SymmetryTransformation<Primitive2D, TranslationMap2D> translation_transformation(const DerivedType & c, const Point<2> & p){
// 	return SymmetryTransformation<Primitive2D, TranslationMap2D>(c.copy(), TranslationMap2D(p));
// }

} // end namespace csg
#endif