#ifndef _CSGEOMETRY3D_H
#define _CSGEOMETRY3D_H

#include <memory>
#include "GeomUtils.hpp"
#include "Primitive3D.hpp"

namespace csg{

class CSGeometry3D 
{
public:

	CSGeometry3D() {};

	CSGeometry3D(const Primitive3D & leaf)
	: m_isleaf(true), m_leaf(leaf.copy()), m_flavor(-1) {};

	CSGeometry3D(std::shared_ptr<Primitive3D> leaf)
	: m_isleaf(true), m_leaf(leaf), m_flavor(-1) {};

	CSGeometry3D(const CSGeometry3D & left, const CSGeometry3D & right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left.copy()), m_rdaughter(right.copy()), m_op(op) {};

	CSGeometry3D(std::shared_ptr<CSGeometry3D> left, std::shared_ptr<CSGeometry3D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(right), m_op(op) {};

	CSGeometry3D(std::shared_ptr<Primitive3D> left, std::shared_ptr<CSGeometry3D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(std::shared_ptr<CSGeometry3D>(new CSGeometry3D(left))), m_rdaughter(right), m_op(op) {};

	CSGeometry3D(std::shared_ptr<CSGeometry3D> left, std::shared_ptr<Primitive3D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(std::shared_ptr<CSGeometry3D>(new CSGeometry3D(right))), m_op(op) {};

	CSGeometry3D(std::shared_ptr<Primitive3D> left, std::shared_ptr<Primitive3D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(std::shared_ptr<CSGeometry3D>(new CSGeometry3D(left))), m_rdaughter(std::shared_ptr<CSGeometry3D>(new CSGeometry3D(right))), m_op(op) {};

	std::shared_ptr<CSGeometry3D> copy() const {return std::make_shared<CSGeometry3D>(*this);};

	void set_flavor(unsigned int flavor) {m_flavor = flavor;};

	unsigned int get_flavor() const {return m_flavor;};

	Box<3> get_bounding_box() const{
		if (m_isleaf) return m_leaf->get_bounding_box();

		return Box<3>::bounding_box(m_ldaughter->get_bounding_box(), m_rdaughter->get_bounding_box());
	}

	void translate(const Point<3> & pt){
		if (m_isleaf) return m_leaf->translate(pt);

		m_ldaughter->translate(pt);
		m_rdaughter->translate(pt);
	}

	// void rotate(const Point<3> & anchor, double degrees) = 0;
	// virtual void rescale(const Point<3> & scalefactor) = 0;

	bool contains_point(const Point<3> & pt) const{
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

	bool contains_box(const Box<3> & bx) const{
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

	bool collides_box(const Box<3> & bx) const{
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

	void print_summary(std::ostream & os = std::cout, unsigned int level=0) const{
		if (m_isleaf){
			m_leaf->print_summary(os,level);
			return;
		}

		for (auto i=0; i<level; i++) os << "\t" ;
		os << "<CSGeometry3D> " << std::endl;
		for (auto i=0; i<level; i++) os << "\t" ;

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
		for (auto i=0; i<level; i++) os << "\t" ;
		os << "<Left> " << std::endl ; m_ldaughter->print_summary(os, level+1); os << "</Left>" << std::endl;
		for (auto i=0; i<level; i++) os << "\t" ;
		os << "<Right> " << std::endl ; m_rdaughter->print_summary(os, level+1); os <<"</Right>" << std::endl;

		for (auto i=0; i<level; i++) os << "\t" ;
		os << "</CSGeometry3D> " << std::endl;
	}

private:
	bool 							m_isleaf;
	std::shared_ptr<Primitive3D> 	m_leaf;
	unsigned int 					m_flavor;

	std::shared_ptr<CSGeometry3D> 	m_ldaughter, m_rdaughter;
	Operation 						m_op;
};

}
#endif