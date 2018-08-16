#ifndef _SYMMETRYTRANSFORMATION_H
#define _SYMMETRYTRANSFORMATION_H

#include "GeomUtils.hpp"
#include <memory>

namespace csg{




// Discrete Translation symmetry map 
struct DiscreteTranslationSymmetryMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mCen, mSvec;

	DiscreteTranslationSymmetryMap2D(const PointT & t, const PointT & c) : mSvec(t), mCen(c) {};

	template <typename PrimitiveT>
	BoxT get_bounding_box(const std::shared_ptr<PrimitiveT> prim) const {
		return bounding_box(BoxT::translate(prim->get_bounding_box(), -1.0e+6*mSvec),
							BoxT::translate(prim->get_bounding_box(), +1.0e+6*mSvec));
		// return prim->get_bounding_box();
	}

	PointT inverse_map(const PointT & p) const{
		PointT v = p-mCen;
		double amagn = mSvec.norm();
		double proj = PointT::dot(v, mSvec);
		// double vmagn = v.norm();
		while (fabs(proj/(amagn*amagn)) > 0.5){
			// if (fabs(proj/amagn) > 1.0) std::cout << "proj/a: " << proj/amagn << std::endl;
			v = v - static_cast<double>(sgn(proj))*mSvec;
			proj = PointT::dot(v, mSvec);
			// vmagn = v.norm();
		}
		// std::cout << "end" << std::endl;
		return v+mCen;
	};

	PointT forward_map(const PointT & p) const{
		return p;
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<DiscreteTranslationSymmetryMapping>" << mSvec << ", " << mCen << "</DiscreteTranslationSymmetryMapping>" << std::endl;
	}


	template <typename PrimitiveT>
	std::vector<Hull<2>> get_outline(unsigned int npts, const std::shared_ptr<PrimitiveT> prim) const {
		std::vector<Hull<2>> o = prim->get_outline(npts);
		std::vector<Hull<2>> out;
		for (auto it=o.begin(); it!=o.end(); it++){
			for (int i=-10; i<=10; i++){
				out.push_back(*it);
				auto mit = --out.end();
				for (auto p=mit->points.begin(); p!=mit->points.end(); p++){
					*p = *p + static_cast<double>(i)*mSvec;
				}
			}
		}
		return out;
	}
};


// Continuous Translation symmetry map 
struct ContinuousTranslationSymmetryMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mCen, mSvec;

	ContinuousTranslationSymmetryMap2D(const PointT & t, const PointT & c) : mSvec(t), mCen(c) {};

	template <typename PrimitiveT>
	BoxT get_bounding_box(const std::shared_ptr<PrimitiveT> prim) const {
		return bounding_box(BoxT::translate(prim->get_bounding_box(), -1.0e+6*mSvec),
							BoxT::translate(prim->get_bounding_box(), +1.0e+6*mSvec));
		// return prim->get_bounding_box();
	}

	PointT inverse_map(const PointT & p) const{
		PointT v = p-mCen;
		double amagn = mSvec.norm();
		double proj = PointT::dot(v, mSvec);
		v = v - mSvec/amagn*proj/amagn;
		return v+mCen;
	};

	PointT forward_map(const PointT & p) const{
		return p;
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<ContinuousTranslationSymmetryMapping>" << mSvec << ", " << mCen << "</ContinuousTranslationSymmetryMapping>" << std::endl;
	}

	// this is only for 2D
	template <typename PrimitiveT>
	std::vector<Hull<2>> get_outline(unsigned int npts, const std::shared_ptr<PrimitiveT> prim) const {
		std::vector<Hull<2>> o = prim->get_outline(npts);
		for (auto it=o.begin(); it!=o.end(); it++){
			for (auto p=it->points.begin(); p!=it->points.end(); p++){
				*p = forward_map(*p);
			}
		}
		return o;
	}
};



// Discrete Rotation symmetry map 
struct DiscreteRotationSymmetryMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mCen, mRpt;
	std::size_t 						mN; // number of copies per 360 degrees

	DiscreteRotationSymmetryMap2D(const PointT & R, const PointT & c, std::size_t N) : mN(N), mCen(c), mRpt(R) {};

	template <typename PrimitiveT>
	BoxT get_bounding_box(const std::shared_ptr<PrimitiveT> prim) const {
		double rmax = std::max(PointT::dist(mRpt, prim->get_bounding_box().lo), PointT::dist(mRpt, prim->get_bounding_box().hi));
		return BoxT(mCen+PointT(rmax, rmax), mCen-PointT(rmax,rmax));
	}

	PointT inverse_map(const PointT & p) const{
		double angle = 2.0*pi/static_cast<double>(mN);
		PointT pp = p - mRpt;
		PointT v = mCen - mRpt;
		double proj = cross(pp, v);
		while (PointT::dot(pp, v) < 0 || fabs(asin(proj/(v.norm()*pp.norm()))) > angle/2.0){
			// rotate
			pp = Point<2>(cos(angle)*pp.x[0] + sin(angle)*pp.x[1], -sin(angle)*pp.x[0] + cos(angle)*pp.x[1]);
			proj = cross(pp, v);
		}
		return pp + mRpt;
	};

	PointT forward_map(const PointT & p) const{
		return p;
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<DiscreteRotationSymmetryMapping>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<N>" << mN << "</N>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Center>" << mRpt << "</Center>" << std::endl;
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</DiscreteRotationSymmetryMapping>" << std::endl;
	}

	// this is only for 2D
	template <typename PrimitiveT>
	std::vector<Hull<2>> get_outline(unsigned int npts, const std::shared_ptr<PrimitiveT> prim) const {
		std::vector<Hull<2>> o = prim->get_outline(npts);
		std::vector<Hull<2>> out;
		double angle = 2.0*pi/static_cast<double>(mN);
		for (auto it=o.begin(); it!=o.end(); it++){
			for (std::size_t i=0; i<mN; i++){
				double theta = static_cast<double>(i)*angle;
				out.push_back(*it);
				auto mit = --out.end();
				for (auto p=mit->points.begin(); p!=mit->points.end(); p++){
					// (*p) = inverse_map(*p);
					PointT pp = *p - mRpt;
					pp = Point<2>(cos(theta)*pp.x[0] + sin(theta)*pp.x[1], -sin(theta)*pp.x[0] + cos(theta)*pp.x[1]);
					*p = pp + mRpt;
				}
			}
		}

		std::cout << "size of in: " << o.size() << std::endl;
		std::cout << "size of out: " << out.size() << std::endl;
		return out;
	}
};


// Continuous Rotation symmetry map 
struct ContinuousRotationSymmetryMap2D{
public:
	typedef Point<2> 					PointT;
	typedef Box<2>						BoxT;
	PointT 								mCen, mRpt;
	// std::size_t 						mN; // number of copies per 360 degrees

	ContinuousRotationSymmetryMap2D(const PointT & R, const PointT & c) : mCen(c), mRpt(R) {};

	template <typename PrimitiveT>
	BoxT get_bounding_box(const std::shared_ptr<PrimitiveT> prim) const {
		double rmax = std::max(PointT::dist(mRpt, prim->get_bounding_box().lo), PointT::dist(mRpt, prim->get_bounding_box().hi));
		return BoxT(mCen+PointT(rmax, rmax), mCen-PointT(rmax,rmax));
	}

	PointT inverse_map(const PointT & p) const{
		PointT pp = p - mRpt;
		PointT v = mCen - mRpt;
		if (PointT::dot(pp, v) < 0) pp = Point<2>(cos(pi)*pp.x[0] + sin(pi)*pp.x[1], -sin(pi)*pp.x[0] + cos(pi)*pp.x[1]);
		double proj = asin(cross(pp, v)/(v.norm()*pp.norm()));
		pp = Point<2>(cos(proj)*pp.x[0] - sin(proj)*pp.x[1], sin(proj)*pp.x[0] + cos(proj)*pp.x[1]);
		return pp + mRpt;
	};

	PointT forward_map(const PointT & p) const{
		return p;
	};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<ContinuousRotationSymmetryMapping>" << mRpt << ", " << mCen << "</ContinuousRotationSymmetryMapping>" << std::endl;
	}

	// this is only for 2D
	template <typename PrimitiveT>
	std::vector<Hull<2>> get_outline(unsigned int npts, const std::shared_ptr<PrimitiveT> prim) const {
		std::vector<Hull<2>> o = prim->get_outline(npts);
		for (auto it=o.begin(); it!=o.end(); it++){
			for (auto p=it->points.begin(); p!=it->points.end(); p++){
				*p = forward_map(*p);
			}
		}
		return o;
	}
};




// // Helical symmetry map 
// struct HelicalSymmetryMap2D{
// public:
// 	typedef Point<2> 					PointT;
// 	typedef Box<2>						BoxT;
// 	PointT 								mCen, mVec;
// 	double 								mR;
// 	// std::size_t 						mN; // number of copies per 360 degrees

// 	HelicalSymmetryMap2D(const PointT & vec, const PointT & c, double r) : mCen(c), mVec(vec), mR(r) {};

// 	PointT inverse_map(const PointT & p) const{
// 		PointT pp = p - mRpt;
// 		PointT v = mCen - mRpt;
// 		if (PointT::dot(pp, v) < 0) pp = Point<2>(cos(pi)*pp.x[0] + sin(pi)*pp.x[1], -sin(pi)*pp.x[0] + cos(pi)*pp.x[1]);
// 		double proj = asin(cross(pp, v)/(v.norm()*pp.norm()));
// 		pp = Point<2>(cos(proj)*pp.x[0] - sin(proj)*pp.x[1], sin(proj)*pp.x[0] + cos(proj)*pp.x[1]);
// 		return pp + mRpt;
// 	};

// 	PointT forward_map(const PointT & p) const{
// 		return p;
// 	};

// 	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
// 		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
// 		os << "<HelicalSymmetryMapping>" << mRpt << ", " << mCen << "</HelicalSymmetryMapping>" << std::endl;
// 	}
// };




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
		return mMap.get_bounding_box(mPrim);
		// return mPrim->get_bounding_box();

		// return BoxT(PointT(-std::numeric_limits<double>::infinity(),
		// 				   -std::numeric_limits<double>::infinity(), 
		// 				   -std::numeric_limits<double>::infinity()), 
		// 			PointT(std::numeric_limits<double>::infinity(), 
		// 				   std::numeric_limits<double>::infinity(), 
		// 				   std::numeric_limits<double>::infinity()));
	}

	// this is only for 2D
	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		// std::vector<Hull<2>> o = mPrim->get_outline(npts);
		// for (auto it=o.begin(); it!=o.end(); it++){
		// 	for (auto p=it->points.begin(); p!=it->points.end(); p++){
		// 		*p = mMap.forward_map(*p);
		// 	}
		// }
		// return o;
		return mMap.get_outline(npts, mPrim);
	}

	bool contains_point(const PointT & pt) const {
		return mPrim->contains_point(mMap.inverse_map(pt));
	}


	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<SymmetryTransformation>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Mapping>" << std::endl;
		mMap.print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Mapping>" << std::endl;

		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Primitive>" << std::endl;
		mPrim->print_summary(os, ntabs+2);
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "</Primitive>" << std::endl;

		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</SymmetryTransformation>" << std::endl;
	}
	///////////////////////////////////////
};



template <typename DerivedType>
SymmetryTransformation<Primitive2D, DiscreteTranslationSymmetryMap2D> discrete_translation_symmetry(const DerivedType & c, const Point<2> & svec){
	Box<2> bx = c.get_bounding_box();
	Point<2> center = 0.5*(bx.hi+bx.lo);
	return SymmetryTransformation<Primitive2D, DiscreteTranslationSymmetryMap2D>(c.copy(), DiscreteTranslationSymmetryMap2D(svec, center));
}

template <typename DerivedType>
SymmetryTransformation<Primitive2D, ContinuousTranslationSymmetryMap2D> continuous_translation_symmetry(const DerivedType & c, const Point<2> & svec){
	Box<2> bx = c.get_bounding_box();
	Point<2> center = 0.5*(bx.hi+bx.lo);
	return SymmetryTransformation<Primitive2D, ContinuousTranslationSymmetryMap2D>(c.copy(), ContinuousTranslationSymmetryMap2D(svec, center));
}





template <typename DerivedType>
SymmetryTransformation<Primitive2D, DiscreteRotationSymmetryMap2D> discrete_rotation_symmetry(const DerivedType & c, const Point<2> & svec, std::size_t N){
	Box<2> bx = c.get_bounding_box();
	Point<2> center = 0.5*(bx.hi+bx.lo);
	return SymmetryTransformation<Primitive2D, DiscreteRotationSymmetryMap2D>(c.copy(), DiscreteRotationSymmetryMap2D(svec, center, N));
}


template <typename DerivedType>
SymmetryTransformation<Primitive2D, ContinuousRotationSymmetryMap2D> continuous_rotation_symmetry(const DerivedType & c, const Point<2> & svec){
	Box<2> bx = c.get_bounding_box();
	Point<2> center = 0.5*(bx.hi+bx.lo);
	return SymmetryTransformation<Primitive2D, ContinuousRotationSymmetryMap2D>(c.copy(), ContinuousRotationSymmetryMap2D(svec, center));
}


} // end namespace csg
#endif