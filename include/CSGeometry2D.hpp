#ifndef _CSGEOMETRY2D_H
#define _CSGEOMETRY2D_H

#include <memory>
#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

class CSGeometry2D;

// 2D Constructive Solid Geometry model
enum Operation {UNION, INTERSECT, DIFFERENCE, XOR};

class CSGeometry2D 
{
public:

	CSGeometry2D() {};

	CSGeometry2D(std::shared_ptr<Primitive2D> leaf)
	: m_isleaf(true), m_leaf(leaf), m_flavor(-1) {};

	CSGeometry2D(std::shared_ptr<CSGeometry2D> left, std::shared_ptr<CSGeometry2D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(right), m_op(op) {};

	CSGeometry2D(std::shared_ptr<Primitive2D> left, std::shared_ptr<CSGeometry2D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(std::shared_ptr<CSGeometry2D>(new CSGeometry2D(left))), m_rdaughter(right), m_op(op) {};

	CSGeometry2D(std::shared_ptr<CSGeometry2D> left, std::shared_ptr<Primitive2D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(std::shared_ptr<CSGeometry2D>(new CSGeometry2D(right))), m_op(op) {};

	CSGeometry2D(std::shared_ptr<Primitive2D> left, std::shared_ptr<Primitive2D> right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(std::shared_ptr<CSGeometry2D>(new CSGeometry2D(left))), m_rdaughter(std::shared_ptr<CSGeometry2D>(new CSGeometry2D(right))), m_op(op) {};

	void set_flavor(unsigned int flavor) {m_flavor = flavor;};

	unsigned int get_flavor() const {return m_flavor;};

	Box<2> get_bounding_box() const{
		if (m_isleaf) return m_leaf->get_bounding_box();

		return Box<2>::bounding_box(m_ldaughter->get_bounding_box(), m_rdaughter->get_bounding_box());
	}

	void translate(const Point<2> & pt){
		if (m_isleaf) return m_leaf->translate(pt);

		m_ldaughter->translate(pt);
		m_rdaughter->translate(pt);
	}

	// void rotate(const Point<2> & anchor, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	bool contains_point(const Point<2> & pt) const{
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

	bool contains_box(const Box<2> & bx) const{
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
				return lc && !rc && !rcoll;
				break;
			case XOR:
				lcoll = m_ldaughter->collides_box(bx);
				rcoll = m_rdaughter->collides_box(bx);
				return (lc || rc) && !(lc && rc) && !lcoll && !rcoll;
				break;
		}
	}

	bool collides_box(const Box<2> & bx) const{
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
				return lc && !rc && !rcont;
				break;
			case XOR:
				lcont = m_ldaughter->contains_box(bx);
				rcont = m_rdaughter->contains_box(bx);
				return (lc || rc) && !(lcont && rcont);
				break;
		}
	}

	void print_summary(std::ostream & os = std::cout) const{
		if (m_isleaf){
			m_leaf->print_summary(os);
			return;
		}

		os << "CSGeometry2D: " << std::endl;
		switch (m_op){
			case UNION:
				os << "\tUNION" ;
				break;
			case INTERSECT:
				os << "\tINTERSECTION" ;
				break;
			case DIFFERENCE:
				os << "\tDIFFERENCE" ;
				break;
			case XOR:
				os << "\tXOR" ;
				break;
		}
		os << "\n\tL: " ; m_ldaughter->print_summary(os);
		os << "\n\tR: " ; m_rdaughter->print_summary(os);
		os << std::endl;
	}

private:
	bool m_isleaf;
	// const Primitive2D * m_leaf;
	std::shared_ptr<Primitive2D> m_leaf;
	unsigned int m_flavor;

	// CSGeometry2D m_ldaughter, m_rdaughter;
	std::shared_ptr<CSGeometry2D> m_ldaughter, m_rdaughter;
	Operation m_op;
};

#endif