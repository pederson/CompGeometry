#ifndef _CSGEOMETRY2D_H
#define _CSGEOMETRY2D_H

#include <memory>
#include "GeomUtils.hpp"
#include "Primitive2D.hpp"
#include "Delaunay.hpp"

namespace csg{

class CSGeometry2D 
{
public:

	CSGeometry2D() {};

	CSGeometry2D(const Primitive2D & leaf)
	: m_isleaf(true), m_leaf(leaf.copy()), m_flavor(-1) {};

	CSGeometry2D(std::shared_ptr<Primitive2D> leaf)
	: m_isleaf(true), m_leaf(leaf), m_flavor(-1) {};

	CSGeometry2D(const CSGeometry2D & left, const CSGeometry2D & right, Operation op)
	: m_isleaf(false), m_leaf(nullptr), m_flavor(-1)
	, m_ldaughter(left.copy()), m_rdaughter(right.copy()), m_op(op) {};

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

	std::shared_ptr<CSGeometry2D> copy() const {return std::make_shared<CSGeometry2D>(*this);};

	void set_flavor(unsigned int flavor) {m_flavor = flavor;};

	unsigned int get_flavor() const {return m_flavor;};

	Box<2> get_bounding_box() const{
		if (m_isleaf) return m_leaf->get_bounding_box();

		return Box<2>::bounding_box(m_ldaughter->get_bounding_box(), m_rdaughter->get_bounding_box());
	}

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


	// get only the points that define an outline of the geometry
	std::vector<Point<2>> get_outline_points(unsigned int npts) const {
		std::vector<Hull<2>> hv = get_outline(npts);
		std::vector<csg::Point<2>> pts;
		for (auto i=0; i<hv.size(); i++) pts.insert(pts.end(), hv[i].points.begin(), hv[i].points.end());
		return pts;
	}


	// get a triangulation of the object
	Triangulation<2> get_triangulation(unsigned int npts) const {
		// first get set of points that define the outline
		Triangulation<2> tout;
		tout.points = get_outline_points(npts);

		// do a Delaunay triangulation of these points
		Delaunay del(tout.points, 1);

		// go through the triangulation and remove triangles with
		// 2 or more edges whose centerpoint is not contained 
		// within the geometry
		unsigned int ct;
		for (auto it=del.triangles.begin(); it != del.triangles.end(); it++){
			if (it->state <= 0) continue;

			ct=0;
			if (!contains_point(0.5*(del.points[it->vertices[0]]+del.points[it->vertices[1]]))) ct++;
			if (!contains_point(0.5*(del.points[it->vertices[1]]+del.points[it->vertices[2]]))) ct++;
			if (!contains_point(0.5*(del.points[it->vertices[2]]+del.points[it->vertices[0]]))) ct++;
			
			if (ct > 1){
				del.erase_triangle(it->vertices[0], it->vertices[1], it->vertices[2],
								   -1, -1, -1);
			}
		}

		unsigned int pos = 0;
		tout.triangles.resize(del.ntri);
		for (auto it=del.triangles.begin(); it != del.triangles.end(); it++){
			if (it->state <= 0) continue;

			tout.triangles[pos] = {static_cast<double>(it->vertices[0]), static_cast<double>(it->vertices[1]), static_cast<double>(it->vertices[2])};
			pos++;
		}

		return tout;
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
				return lc && !rcoll;
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
		os << "<CSGeometry2D> " << std::endl;
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
		os << "</CSGeometry2D> " << std::endl;
	}

private:
	bool 							m_isleaf;
	std::shared_ptr<Primitive2D> 	m_leaf;
	unsigned int 					m_flavor;


	std::shared_ptr<CSGeometry2D> 	m_ldaughter, m_rdaughter;
	Operation 						m_op;
};

}
#endif