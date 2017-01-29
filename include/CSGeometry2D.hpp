#ifndef _CSGEOMETRY2D_H
#define _CSGEOMETRY2D_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

// 2D Constructive Solid Geometry model
enum Operation {UNION, INTERSECT, DIFFERENCE, XOR};

class CSGeometry2D 
{
public:

	CSGeometry2D() {};

	CSGeometry2D(const Primitive2D * leaf)
	: m_isleaf(true), m_leaf(leaf), m_flavor(-1) {};

	CSGeometry2D(CSGeometry2D left, CSGeometry2D right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(right), m_op(op) {};

	CSGeometry2D(const Primitive2D * left, CSGeometry2D right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(CSGeometry2D(left)), m_rdaughter(right), m_op(op) {};

	CSGeometry2D(CSGeometry2D left, const Primitive2D * right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left), m_rdaughter(CSGeometry2D(right)), m_op(op) {};

	void set_flavor(unsigned int flavor) {m_flavor = flavor;};

	unsigned int get_flavor() const {return m_flavor;};

	Box<2> get_bounding_box() const{
		if (m_isleaf) return m_leaf->get_bounding_box();

		return Box::bounding_box(m_ldaughter.get_bounding_box(), m_rdaughter.get_bounding_box());
	}

	void translate(const Point<2> & pt){
		if (m_isleaf) return m_leaf->translate(pt);

		bool lc = m_ldaughter.translate(pt);
		bool rc = m_rdaughter.translate(pt);
	}

	// void rotate(const Point<2> & anchor, double degrees) = 0;
	// virtual void rescale(const Point<2> & scalefactor) = 0;

	bool contains_point(const Point<2> & pt) const{
		if (m_isleaf) return m_leaf->contains_point(pt);

		bool lc = m_ldaughter.contains_point(pt);
		bool rc = m_rdaughter.contains_point(pt);

		switch (op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				return lc && ~rc;
				break;
			case XOR:
				return (lc || rc) && ~(lc && rc);
				break;
		}
	}

	bool contains_box(const Box<2> & bx) const{
		if (m_isleaf) return m_leaf->contains_box(bx);

		bool lc = m_ldaughter.contains_box(bx);
		bool rc = m_rdaughter.contains_box(bx);
		

		switch (op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				bool rcoll = m_rdaughter.collides_box(bx);
				return lc && ~rc && ~rcoll;
				break;
			case XOR:
				bool lcoll = m_ldaughter.collides_box(bx);
				bool rcoll = m_rdaughter.collides_box(bx);
				return (lc || rc) && ~(lc && rc) && ~lcoll && ~rcoll;
				break;
		}
	}

	bool collides_box(const Box<2> & bx) const{
		if (m_isleaf) return m_leaf->collides_box(bx);

		bool lc = m_ldaughter.collides_box(bx);
		bool rc = m_rdaughter.collides_box(bx);
		

		switch (op){
			case UNION:
				return lc || rc;
				break;
			case INTERSECT:
				return lc && rc;
				break;
			case DIFFERENCE:
				bool rcont = m_rdaughter.contains_box(bx);
				return lc && ~rc && ~rcont;
				break;
			case XOR:
				bool lcont = m_ldaughter.contains_box(bx);
				bool rcont = m_rdaughter.contains_box(bx);
				return (lc || rc) && ~(lcont && rcont);
				break;
		}
	}

	void print_summary(std::ostream & os) const{
		if (m_isleaf){
			m_leaf->print_summary(os);
			return;
		}

		os << "\tCSGeometry2D: " << std::endl;
		os << "L: " << m_ldaughter.print_summary(os);
		os << "R: " << m_rdaughter.print_summary(os);
	}

private:
	bool m_isleaf;
	const Primitive2D * m_leaf;
	unsigned int m_flavor;

	CSGeometry2D m_ldaughter, m_rdaughter;
	Operation m_op;
};

#endif