#ifndef _PRIMITIVEIO_H
#define _PRIMITIVEIO_H

#include "PrimitiveTypes.hpp"
#include "Primitive2D.hpp"
#include "Primitive3D.hpp"
#include "CSGTree.hpp"
#include "LinearTransformation.hpp"
#include "SymmetryTransformation.hpp"

#include <fstream>
#include <memory>

namespace csg{


template <typename PrimitiveT>
void write(std::string filename, const PrimitiveT & obj){
	std::fstream fobj(filename, std::fstream::out);
	obj.print_summary(fobj);
	return;
}










#ifdef TINYXML2_INCLUDED
	using namespace tinyxml2;

	

	void printXML(const XMLNode * n, unsigned int ntab = 0){
		if (n == nullptr) return;

		for (unsigned int i=0; i<ntab; i++) std::cout << "\t" ;
		std::cout << n->Value() ;

		// handle leaves
		if (n->FirstChildElement() == nullptr && n->FirstChild() != nullptr) 	std::cout << " ---> " ;
		else 									std::cout << std::endl;
		// print key, value, attributes if there are no children

		printXML(n->FirstChild(), ntab+1);
		printXML(n->NextSibling(), ntab);

		return;
	}

	// forward declare this
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D(const XMLNode * n);

	// declare the templatized version
	template <typename T>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D(const XMLNode * n){
	}

	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<Circle>(const XMLNode * n){
		std::cout << "\t output is a Circle" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasRadius = false;
		double rad;
		Point<2> cen;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Radius")){
				hasRadius = true;
				ss << c->FirstChild()->Value();
				ss >> rad;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasRadius){
			std::cerr << "must have both <Radius> and <Center> properties" << std::endl;
			throw -1;
		}

		return std::make_shared<Circle>(Circle(cen, rad));
	}





	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<Rectangle>(const XMLNode * n){
		std::cout << "\t output is a Rectangle" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasDims = false;
		Point<2> cen, dims;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Dims")){
				hasDims = true;
				ss << c->FirstChild()->Value();
				ss >> dims;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasDims){
			std::cerr << "Rectangle format prototype is: " << std::endl;
			std::cerr << "\t<Rectangle>" << std::endl;
			std::cerr << "\t\t<Center>(x0, y0)</Center>" << std::endl;
			std::cerr << "\t\t<Dims>(width, height)</Dims>" << std::endl;
			std::cerr << "\t</Rectangle>" << std::endl;

			throw -1;
		}

		return std::make_shared<Rectangle>(Rectangle(cen, dims));
	}



	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<Ellipse>(const XMLNode * n){
		std::cout << "\t output is a Ellipse" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasDims = false;
		Point<2> cen, dims;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Axes")){
				hasDims = true;
				ss << c->FirstChild()->Value();
				ss >> dims;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasDims){
			std::cerr << "Ellipse format prototype is: " << std::endl;
			std::cerr << "\t<Ellipse>" << std::endl;
			std::cerr << "\t\t<Center>(x0, y0)</Center>" << std::endl;
			std::cerr << "\t\t<Axes>(width, height)</Axes>" << std::endl;
			std::cerr << "\t</Ellipse>" << std::endl;

			throw -1;
		}

		return std::make_shared<Ellipse>(Ellipse(cen, dims));
	}




	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<Triangle>(const XMLNode * n){
		std::cout << "\t output is a Triangle" << std::endl;
		auto c = n->FirstChild();

		std::vector<Point<2>> pts;
		Point<2> p;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Point")){
				ss << c->FirstChild()->Value();
				ss >> p;
				pts.push_back(p);
			}

			c = c->NextSibling();
		}

		if (pts.size() != 3){
			std::cerr << "Triangle format prototype is: " << std::endl;
			std::cerr << "\t<Triangle>" << std::endl;
			std::cerr << "\t\t<Point>(x0, y0)</Point>" << std::endl;
			std::cerr << "\t\t<Point>(x1, y1)</Point>" << std::endl;
			std::cerr << "\t\t<Point>(x2, y2)</Point>" << std::endl;
			std::cerr << "\t</Triangle>" << std::endl;

			throw -1;
		}

		return std::make_shared<Triangle>(Triangle(pts[0], pts[1], pts[2]));
	}




	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<RegularPolygon>(const XMLNode * n){
		std::cout << "\t output is a RegularPolygon" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasN = false, hasVertexLength = false;
		Point<2> cen;
		unsigned int N;
		double vlen;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			if (!strcmp(c->Value(), "N")){
				hasN = true;
				ss << c->FirstChild()->Value();
				ss >> N;
			}
			if (!strcmp(c->Value(), "VertexLength")){
				hasVertexLength = true;
				ss << c->FirstChild()->Value();
				ss >> vlen;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasN || !hasVertexLength){
			std::cerr << "RegularPolygon format prototype is: " << std::endl;
			std::cerr << "\t<RegularPolygon>" << std::endl;
			std::cerr << "\t\t<Center>(x0, y0)</Center>" << std::endl;
			std::cerr << "\t\t<N>Npts</N>" << std::endl;
			std::cerr << "\t\t<VertexLength>len</VertexLength>" << std::endl;
			std::cerr << "\t</RegularPolygon>" << std::endl;

			throw -1;
		}

		return std::make_shared<RegularPolygon>(RegularPolygon(N, cen, vlen));
	}




	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<CSGTree<Primitive2D>>(const XMLNode * n){
		std::cout << "\t output is a CSGTree" << std::endl;
		auto c = n->FirstChild();

		bool hasLeft = false, hasRight = false, hasOperation = false;
		std::shared_ptr<Primitive2D> left, right;
		Operation op = DIFFERENCE;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Left")){
				hasLeft = true;
				left = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Right")){
				hasRight = true;
				right = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Operation")){
				hasOperation = true;
				if(!strcmp(c->FirstChild()->Value(), "DIFFERENCE")) op = DIFFERENCE;
				else if(!strcmp(c->FirstChild()->Value(), "INTERSECT")) op = INTERSECT;
				else if(!strcmp(c->FirstChild()->Value(), "UNION"))	op = UNION;
				else if(!strcmp(c->FirstChild()->Value(), "XOR"))	op = XOR;
				else{
					std::cerr << "Operation value must be one of [DIFFERENCE, INTERSECT, UNION, XOR]" << std::endl;
					throw -1;
				}
			}

			c = c->NextSibling();
		}

		if (!hasLeft || !hasRight || !hasOperation){
			std::cerr << "CSGTree format prototype is: " << std::endl;
			std::cerr << "\t<CSGTree>" << std::endl;
			std::cerr << "\t\t<Right>[a primitive type here]</Right>" << std::endl;
			std::cerr << "\t\t<Left>[a primitive type here]</Left>" << std::endl;
			std::cerr << "\t\t<Operation>[a binary operation here]</Operation>" << std::endl;
			std::cerr << "\t</CSGTree>" << std::endl;

			std::cerr << "The resulting tree is interpreted as (Left **Op** Right)" << std::endl;

			throw -1;
		}

		return std::make_shared<CSGTree<Primitive2D>>(CSGTree<Primitive2D>(left, right, op));
	}




	std::shared_ptr<Primitive2D> buildGeometryFromXML2D_LinearTransformation(const XMLNode * n){
		std::cout << "\t output is a LinearTransformation" << std::endl;
		auto c = n->FirstChild();

		bool hasMapping = false, hasPrimitive = false;
		std::shared_ptr<Primitive2D> prim;
		std::string mapstring;
		const XMLNode * mapnode;

		while (c != nullptr){

			if (!strcmp(c->Value(), "Primitive")){
				hasPrimitive = true;
				prim = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Mapping")){
				hasMapping = true;
				mapnode = c->FirstChild();
				if(!strcmp(c->FirstChild()->Value(), "ShearMapping")) mapstring = "ShearMapping";
				else if(!strcmp(c->FirstChild()->Value(), "DilatationMapping")) mapstring = "DilatationMapping";
				else if(!strcmp(c->FirstChild()->Value(), "RotationMapping"))	mapstring = "RotationMapping";
				else if(!strcmp(c->FirstChild()->Value(), "TranslationMapping"))	mapstring = "TranslationMapping";
				else{
					std::cerr << "Operation value must be one of [ShearMapping, DilatationMapping, RotationMapping, TranslationMapping]" << std::endl;
					throw -1;
				}
			}

			c = c->NextSibling();
		}

		if (!hasMapping || !hasPrimitive){
			std::cerr << "LinearTransformation format prototype is: " << std::endl;
			std::cerr << "\t<LinearTransformation>" << std::endl;
			std::cerr << "\t\t<Mapping>[a mapping type here]</Mapping>" << std::endl;
			std::cerr << "\t\t<Primitive>[a primitive type here]</Primitive>" << std::endl;
			std::cerr << "\t</LinearTransformation>" << std::endl;
			throw -1;
		}


		std::stringstream ss;
		if(!strcmp(mapstring.c_str(), "ShearMapping")){
			Point<2> p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, ShearMap2D>>(shear_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "DilatationMapping")){
			Point<2> p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, DilatationMap2D>>(dilatation_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "RotationMapping")){
			double p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, RotationMap2D>>(rotation_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "TranslationMapping")){
			Point<2> p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, TranslationMap2D>>(translation_transformation(*prim, p));
		}
		else{
			std::cerr << "Operation value must be one of [ShearMapping, DilatationMapping, RotationMapping, TranslationMapping]" << std::endl;
			throw -1;
		}

		// return std::make_shared<LinearTransformation<Primitive2D>>(LinearTransformation<Primitive2D>(left, right, op));
	}





	std::shared_ptr<Primitive2D> buildGeometryFromXML2D_SymmetryTransformation(const XMLNode * n){
		std::cout << "\t output is a SymmetryTransformation" << std::endl;
		auto c = n->FirstChild();

		bool hasMapping = false, hasPrimitive = false;
		std::shared_ptr<Primitive2D> prim;
		std::string mapstring;
		const XMLNode * mapnode;

		while (c != nullptr){

			if (!strcmp(c->Value(), "Primitive")){
				hasPrimitive = true;
				prim = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Mapping")){
				hasMapping = true;
				mapnode = c->FirstChild();
				if(!strcmp(c->FirstChild()->Value(), "DiscreteRotationMapping")) mapstring = "DiscreteRotationMapping";
				else if(!strcmp(c->FirstChild()->Value(), "ContinuousRotationMapping")) mapstring = "ContinuousRotationMapping";
				else if(!strcmp(c->FirstChild()->Value(), "ContinuousTranslationMapping"))	mapstring = "ContinuousTranslationMapping";
				else if(!strcmp(c->FirstChild()->Value(), "DiscreteTranslationMapping"))	mapstring = "DiscreteTranslationMapping";
				else{
					std::cerr << "Operation value must be one of [DiscreteRotationMapping, ContinuousRotationMapping, DiscreteTranslationMapping, ContinuousTranslationMapping]" << std::endl;
					throw -1;
				}
			}

			c = c->NextSibling();
		}

		if (!hasMapping || !hasPrimitive){
			std::cerr << "SymmetryTransformation format prototype is: " << std::endl;
			std::cerr << "\t<SymmetryTransformation>" << std::endl;
			std::cerr << "\t\t<Mapping>[a mapping type here]</Mapping>" << std::endl;
			std::cerr << "\t\t<Primitive>[a primitive type here]</Primitive>" << std::endl;
			std::cerr << "\t</SymmetryTransformation>" << std::endl;
			throw -1;
		}


		std::stringstream ss;
		if(!strcmp(mapstring.c_str(), "DiscreteRotationMapping")){
			Point<2> p;
			std::size_t N;
			const XMLNode * mn = mapnode->FirstChild();
			ss << mn->FirstChild()->Value();
			ss >> N;

			std::cout << "N: " << mn->FirstChild()->Value() << std::endl;

			ss.clear();
			mn = mn->NextSibling();
			ss << mn->FirstChild()->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, DiscreteRotationSymmetryMap2D>>(discrete_rotation_symmetry(*prim, p, N));
		}
		else if(!strcmp(mapstring.c_str(), "ContinuousRotationMapping")){
			Point<2> p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, ContinuousRotationSymmetryMap2D>>(continuous_rotation_symmetry(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "ContinuousTranslationMapping")){
			double p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, ContinuousTranslationSymmetryMap2D>>(continuous_translation_symmetry(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "DiscreteTranslationMapping")){
			Point<2> p;
			ss << mapnode->FirstChild()->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, DiscreteTranslationSymmetryMap2D>>(discrete_translation_symmetry(*prim, p));
		}
		else{
			std::cerr << "Operation value must be one of [DiscreteRotationMapping, ContinuousRotationMapping, DiscreteTranslationMapping, ContinuousTranslationMapping]" << std::endl;
			throw -1;
		}

		// return std::make_shared<LinearTransformation<Primitive2D>>(LinearTransformation<Primitive2D>(left, right, op));
	}




	std::shared_ptr<Primitive2D> buildGeometryFromXML2D(const XMLNode * n){
		if (n == nullptr) return std::make_shared<Circle>(Circle({0,0}, 0));

		// for (unsigned int i=0; i<ntab; i++) std::cout << "\t" ;
		std::cout << n->Value() << ":   " << std::endl ;

		if (!strcmp(n->Value(),"CSGTree")) return buildGeometryFromXML2D<CSGTree<Primitive2D>>(n);
		else if (!strcmp(n->Value(),"LinearTransformation")) return buildGeometryFromXML2D_LinearTransformation(n);
		else if (!strcmp(n->Value(),"SymmetryTransformation")) return buildGeometryFromXML2D_SymmetryTransformation(n);
		else if (!strcmp(n->Value(),"Circle")) return buildGeometryFromXML2D<Circle>(n);
		else if (!strcmp(n->Value(),"Rectangle")) return buildGeometryFromXML2D<Rectangle>(n);
		else if (!strcmp(n->Value(),"Ellipse")) return buildGeometryFromXML2D<Ellipse>(n);
		else if (!strcmp(n->Value(),"Triangle")) return buildGeometryFromXML2D<Triangle>(n);
		else if (!strcmp(n->Value(),"RegularPolygon")) return buildGeometryFromXML2D<RegularPolygon>(n);
		

		std::cerr << "Unrecognized geometry option: " << n->Value() << std::endl;
		throw -1;
		return std::make_shared<Circle>(Circle({0,0}, 0));
	}




	std::shared_ptr<Primitive2D> read2D(std::string filename){
		XMLDocument doc;
		doc.LoadFile(filename.c_str());
		return buildGeometryFromXML2D(doc.FirstChild());
	}

	
#endif





} // end namespace csg
#endif