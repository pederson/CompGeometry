#ifndef _CSGTREE_H
#define _CSGTREE_H

#include <memory>
#include "GeomUtils.hpp"
#include "Delaunay.hpp"

namespace csg{

template <class PrimitiveType>
class CSGTree : public PrimitiveType
{
public:
	typedef PrimitiveType				LeafT;


	CSGTree() : m_isleaf(true), m_leaf(nullptr) {};

	CSGTree(std::shared_ptr<LeafT> leaf)
	: m_isleaf(true), m_leaf(leaf) {};

	CSGTree(std::shared_ptr<CSGTree> left, std::shared_ptr<CSGTree> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr)
	, m_ldaughter(left), m_rdaughter(right), m_op(op) {};

	CSGTree(std::shared_ptr<LeafT> left, std::shared_ptr<CSGTree> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr)
	, m_ldaughter(std::shared_ptr<CSGTree>(new CSGTree(left))), m_rdaughter(right), m_op(op) {};

	CSGTree(std::shared_ptr<CSGTree> left, std::shared_ptr<LeafT> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr)
	, m_ldaughter(left), m_rdaughter(std::shared_ptr<CSGTree>(new CSGTree(right))), m_op(op) {};

	CSGTree(std::shared_ptr<LeafT> left, std::shared_ptr<LeafT> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr)
	, m_ldaughter(std::shared_ptr<CSGTree>(new CSGTree(left))), m_rdaughter(std::shared_ptr<CSGTree>(new CSGTree(right))), m_op(op) {};

	// std::shared_ptr<CSGTree> copy() const {return std::make_shared<CSGTree>(*this);};

	void push_back(std::shared_ptr<LeafT> obj, Operation op){

		if (m_isleaf && m_leaf == nullptr){
			m_isleaf 	= true;
			m_leaf 		= obj;
		}
		else {
			// auto cp = this->copy();
			m_ldaughter = std::make_shared<CSGTree>(*this);//this->copy();//std::shared_ptr<CSGTree>(new CSGTree(*this));
			m_rdaughter = std::make_shared<CSGTree>(obj);
			m_op 		= op;
			m_isleaf 	= false;
			m_leaf 		= nullptr;
		}
		
	}


public:
//////////// THESE IMPLEMENT THE PRIMITIVEGEOMETRY INTERFACE

	// convenience constructors
	CSGTree(const LeafT & leaf)
	: m_isleaf(true), m_leaf(leaf.copy()) {};

	// convenience construction function
	void push_back(const LeafT & leaf, Operation op){push_back(leaf.copy(),op);};

	std::shared_ptr<PrimitiveType> copy() const {return std::make_shared<CSGTree>(*this);};

	// returns a bounding box
	// - only compiles if the LeafT has a function "get_bounding_box()"
	typename BoxTypedef<PrimitiveType>::type get_bounding_box() const{
		if (m_isleaf) return m_leaf->get_bounding_box();

		typedef typename BoxTypedef<PrimitiveType>::type BoxT;
		BoxT bxl = m_ldaughter->get_bounding_box();
		BoxT bxr = m_rdaughter->get_bounding_box();

		BoxT bx = bxl;
			
		switch (m_op){
			case UNION:
				bx = bounding_box(m_ldaughter->get_bounding_box(), m_rdaughter->get_bounding_box());
				break;
			case INTERSECT:
				for (auto i=0; i<bx.lo.size(); i++){
					bx.lo.x[i] = std::max(bxl.lo.x[i], bxr.lo.x[i]);
					bx.hi.x[i] = std::min(bxl.hi.x[i], bxr.hi.x[i]);
				}
				break;
			case DIFFERENCE:
				bx = m_ldaughter->get_bounding_box();
				break;
			case XOR:
				bx = bounding_box(m_ldaughter->get_bounding_box(), m_rdaughter->get_bounding_box());
				break;
		}
		return bx;
	}


	// returns a Hull that outlines the object (2D Feature ONLY)
	// - only compiles if the LeafT has a function "get_outline(int numpoints)"
	std::vector<Hull<2>> get_outline(unsigned int npts) const {
		if (m_isleaf) return m_leaf->get_outline(npts);

		std::vector<Hull<2>> oleft = m_ldaughter->get_outline(npts);
		std::vector<Hull<2>> oright = m_rdaughter->get_outline(npts);
		

		switch (m_op){
			case UNION:
				for (auto j=0; j<oleft.size(); j++){
					bool state0 = m_rdaughter->contains_point(*(--oleft[j].points.end()));
					for (auto it=oleft[j].points.begin(); it != oleft[j].points.end();){
						if (m_rdaughter->contains_point(*it)) it = oleft[j].points.erase(it);
						else it++;
					}
				}

				for (auto j=0; j<oright.size(); j++){
					for (auto it=oright[j].points.begin(); it != oright[j].points.end();){
						if (m_ldaughter->contains_point(*it)) it = oright[j].points.erase(it);
						else it++;
					}
				}
				break;
			case INTERSECT:
				for (auto j=0; j<oleft.size(); j++){
					for (auto it=oleft[j].points.begin(); it != oleft[j].points.end();){
						if (!m_rdaughter->contains_point(*it)) it = oleft[j].points.erase(it);
						else it++;
					}
				}

				for (auto j=0; j<oright.size(); j++){
					for (auto it=oright[j].points.begin(); it != oright[j].points.end();){
						if (!m_ldaughter->contains_point(*it)) it = oright[j].points.erase(it);
						else it++;
					}
				}
				break;
			case DIFFERENCE:
				for (auto j=0; j<oleft.size(); j++){
					for (auto it=oleft[j].points.begin(); it != oleft[j].points.end();){
						if (m_rdaughter->contains_point(*it)) it = oleft[j].points.erase(it);
						else it++;
					}
				}

				for (auto j=0; j<oright.size(); j++){
					for (auto it=oright[j].points.begin(); it != oright[j].points.end();){
						if (!m_ldaughter->contains_point(*it)) it = oright[j].points.erase(it);
						else it++;
					}
				}
				break;
			case XOR:
				// no culling necessary for XOR
				// for (auto j=0; j<oleft.size(); j++){
				// 	for (auto it=oleft[j].points.begin(); it != oleft[j].points.end();){
				// 		if (m_rdaughter->contains_point(*it)) it = oleft[j].points.erase(it);
				// 		else it++;
				// 	}
				// }

				// for (auto j=0; j<oright.size(); j++){
				// 	for (auto it=oright[j].points.begin(); it != oright[j].points.end();){
				// 		if (!m_ldaughter->contains_point(*it)) it = oright[j].points.erase(it);
				// 		else it++;
				// 	}
				// }
				break;
		}
		oleft.insert(oleft.end(), oright.begin(), oright.end());
		return oleft;
	}


	// detects if the CSGTree contains the given point
	// - only compiles if the LeafT has a function "contains_point(Point)"
	template <typename PointType>
	bool contains_point_impl(const PointType & pt) const{
		if (m_isleaf) return m_leaf->contains_point(pt);

		bool lc = m_ldaughter->contains_point(pt);
		bool rc = m_rdaughter->contains_point(pt);

		switch (m_op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				return lc && !rc;
				break;
			case XOR:
				return (lc || rc) && !(lc && rc);
				break;
		}
	}

	bool contains_point(const Point<2> & pt) const {return contains_point_impl(pt);};
	bool contains_point(const Point<3> & pt) const {return contains_point_impl(pt);};

	// detects if the CSGTree contains the given box
	// - only compiles if the LeafT has a function "contains_box(Box)"
	template <typename BoxType>
	bool contains_box_impl(const BoxType & bx) const{
		if (m_isleaf) return m_leaf->contains_box(bx);

		bool lc = m_ldaughter->contains_box(bx);
		bool rc = m_rdaughter->contains_box(bx);
		bool rcoll, lcoll;
		

		switch (m_op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				rcoll = m_rdaughter->collides_box(bx);
				return lc && !rcoll;
				break;
			case XOR:
				lcoll = m_ldaughter->collides_box(bx);
				rcoll = m_rdaughter->collides_box(bx);
				return (lc || rc) && !(lc && rc) && !lcoll && !rcoll;
				break;
		}
	}

	bool contains_box(const Box<2> & pt) const {return contains_box_impl(pt);};
	bool contains_box(const Box<3> & pt) const {return contains_box_impl(pt);};

	// detects collisions between bounding boxes
	// - only compiles if the LeafT has a function "collides_box(Box)"
	template <typename BoxType>
	bool collides_box_impl(const BoxType & bx) const{
		if (m_isleaf) return m_leaf->collides_box(bx);

		bool lc = m_ldaughter->collides_box(bx);
		bool rc = m_rdaughter->collides_box(bx);
		bool lcont, rcont;

		switch (m_op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				rcont = m_rdaughter->contains_box(bx);
				return lc && !rcont;
				break;
			case XOR:
				lcont = m_ldaughter->contains_box(bx);
				rcont = m_rdaughter->contains_box(bx);
				return (lc || rc) && !(lcont && rcont);
				break;
		}
	}


	bool collides_box(const Box<2> & pt) const {return collides_box_impl(pt);};
	bool collides_box(const Box<3> & pt) const {return collides_box_impl(pt);};


	// prints an XML-style summary of the tree to an output stream
	// - only compiles if the LeafT has a function "print_summary(ostream, num_tabs)"
	void print_summary(std::ostream & os = std::cout, unsigned int level=0) const{
		if (m_isleaf){
			m_leaf->print_summary(os,level);
			return;
		}

		for (auto i=0; i<level; i++) os << "\t" ;
		os << "<CSGTree> " << std::endl;

		for (auto i=0; i<level+1; i++) os << "\t" ;
		os << "<Operation>" ;
		switch (m_op){
			case UNION:
				os << "UNION" ;
				break;
			case INTERSECT:
				os << "INTERSECTION" ;
				break;
			case DIFFERENCE:
				os << "DIFFERENCE" ;
				break;
			case XOR:
				os << "XOR" ;
				break;
		}
		os << "</Operation>" ;
		os << std::endl;
		for (auto i=0; i<level+1; i++) os << "\t" ;
		os << "<Left> " << std::endl ; m_ldaughter->print_summary(os, level+1+1); 
		for (auto i=0; i<level+1; i++) os << "\t" ;
		os << "</Left>" << std::endl;
		for (auto i=0; i<level+1; i++) os << "\t" ;
		os << "<Right> " << std::endl ; m_rdaughter->print_summary(os, level+1+1); 
		for (auto i=0; i<level+1; i++) os << "\t" ;
		os <<"</Right>" << std::endl;
		for (auto i=0; i<level; i++) os << "\t" ;
		os << "</CSGTree> " << std::endl;
	}

protected:
	bool 							m_isleaf;
	std::shared_ptr<LeafT> 			m_leaf;


	std::shared_ptr<CSGTree> 		m_ldaughter, m_rdaughter;
	Operation 						m_op;
};

}
#endif