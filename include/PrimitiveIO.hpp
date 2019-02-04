#ifndef _PRIMITIVEIO_H
#define _PRIMITIVEIO_H

#include "PrimitiveTypes.hpp"
#include "Primitive2D.hpp"
#include "Primitive3D.hpp"
#include "CSGTree.hpp"
#include "LinearTransformation.hpp"
#include "SymmetryTransformation.hpp"

#include "Scene.hpp"

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


	// this function is purely for the purpose of skipping over comments
	const XMLNode * retrieveNode(const XMLNode * n){
		if (n==nullptr) return n;
		if(n->ToComment() != nullptr) return retrieveNode(n->NextSibling());
		return n;
	}


	const char * retrieveXMLString(const XMLNode * n){
		if (n->ToComment() != nullptr) return retrieveXMLString(n->NextSibling());
		return n->Value();
	}



	// const char * retrieveXMLString(const XMLComment * n){
	// 	return retrieveXMLString(n->NextSibling()); // skip line for comments
	// }



//////////////////////// 2D GEOMETRY READ /////////////////////////////
	// forward declare this
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D(const XMLNode * n);

	// declare the templatized version
	template <typename T>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D(const XMLNode * n){
	}

	template <>
	std::shared_ptr<Primitive2D> buildGeometryFromXML2D<Circle>(const XMLNode * n){
		std::cout << "\t output is a Circle" << std::endl;
		auto c = retrieveNode(n->FirstChild());

		bool hasCenter = false, hasRadius = false;
		double rad;
		Point<2> cen;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Center")){
				hasCenter = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> cen;
			}
			else if(!strcmp(retrieveXMLString(c), "Radius")){
				hasRadius = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> rad;
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		bool hasCenter = false, hasDims = false;
		Point<2> cen, dims;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Center")){
				hasCenter = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> cen;
			}
			else if(!strcmp(retrieveXMLString(c), "Dims")){
				hasDims = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> dims;
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		bool hasCenter = false, hasDims = false;
		Point<2> cen, dims;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Center")){
				hasCenter = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> cen;
			}
			else if(!strcmp(retrieveXMLString(c), "Axes")){
				hasDims = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> dims;
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		std::vector<Point<2>> pts;
		Point<2> p;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Point")){
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> p;
				pts.push_back(p);
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		bool hasCenter = false, hasN = false, hasVertexLength = false;
		Point<2> cen;
		unsigned int N;
		double vlen;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Center")){
				hasCenter = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> cen;
			}
			if (!strcmp(retrieveXMLString(c), "N")){
				hasN = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> N;
			}
			if (!strcmp(retrieveXMLString(c), "VertexLength")){
				hasVertexLength = true;
				ss << retrieveNode(c->FirstChild())->Value();
				ss >> vlen;
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		bool hasLeft = false, hasRight = false, hasOperation = false;
		std::shared_ptr<Primitive2D> left, right;
		Operation op = DIFFERENCE;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(retrieveXMLString(c), "Left")){
				hasLeft = true;
				left = buildGeometryFromXML2D(retrieveNode(c->FirstChild()));
			}
			else if(!strcmp(retrieveXMLString(c), "Right")){
				hasRight = true;
				right = buildGeometryFromXML2D(retrieveNode(c->FirstChild()));
			}
			else if(!strcmp(retrieveXMLString(c), "Operation")){
				hasOperation = true;
				if(!strcmp(retrieveXMLString(c->FirstChild()), "DIFFERENCE")) op = DIFFERENCE;
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "INTERSECT")) op = INTERSECT;
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "UNION"))	op = UNION;
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "XOR"))	op = XOR;
				else{
					std::cerr << "Operation value must be one of [DIFFERENCE, INTERSECT, UNION, XOR]" << std::endl;
					throw -1;
				}
			}

			c = retrieveNode(c->NextSibling());
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
		auto c = retrieveNode(n->FirstChild());

		bool hasMapping = false, hasPrimitive = false;
		std::shared_ptr<Primitive2D> prim;
		std::string mapstring;
		const XMLNode * mapnode;

		while (c != nullptr){

			if (!strcmp(retrieveXMLString(c), "Primitive")){
				hasPrimitive = true;
				prim = buildGeometryFromXML2D(retrieveNode(c->FirstChild()));
			}
			else if(!strcmp(retrieveXMLString(c), "Mapping")){
				hasMapping = true;
				mapnode = retrieveNode(c->FirstChild());
				if(!strcmp(retrieveXMLString(c->FirstChild()), "ShearMapping")) mapstring = "ShearMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "DilatationMapping")) mapstring = "DilatationMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "RotationMapping"))	mapstring = "RotationMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "TranslationMapping"))	mapstring = "TranslationMapping";
				else{
					std::cerr << "Operation value must be one of [ShearMapping, DilatationMapping, RotationMapping, TranslationMapping]" << std::endl;
					throw -1;
				}
			}

			c = retrieveNode(c->NextSibling());
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
			ss << retrieveNode(mapnode->FirstChild())->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, ShearMap2D>>(shear_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "DilatationMapping")){
			Point<2> p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, DilatationMap2D>>(dilatation_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "RotationMapping")){
			double p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
			ss >> p;
			return std::make_shared<LinearTransformation<Primitive2D, RotationMap2D>>(rotation_transformation(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "TranslationMapping")){
			Point<2> p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
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
		auto c = retrieveNode(n->FirstChild());

		bool hasMapping = false, hasPrimitive = false;
		std::shared_ptr<Primitive2D> prim;
		std::string mapstring;
		const XMLNode * mapnode;

		while (c != nullptr){

			if (!strcmp(retrieveXMLString(c), "Primitive")){
				hasPrimitive = true;
				prim = buildGeometryFromXML2D(retrieveNode(c->FirstChild()));
			}
			else if(!strcmp(retrieveXMLString(c), "Mapping")){
				hasMapping = true;
				mapnode = retrieveNode(c->FirstChild());
				if(!strcmp(retrieveXMLString(c->FirstChild()), "DiscreteRotationMapping")) mapstring = "DiscreteRotationMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "ContinuousRotationMapping")) mapstring = "ContinuousRotationMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "ContinuousTranslationMapping"))	mapstring = "ContinuousTranslationMapping";
				else if(!strcmp(retrieveXMLString(c->FirstChild()), "DiscreteTranslationMapping"))	mapstring = "DiscreteTranslationMapping";
				else{
					std::cerr << "Operation value must be one of [DiscreteRotationMapping, ContinuousRotationMapping, DiscreteTranslationMapping, ContinuousTranslationMapping]" << std::endl;
					throw -1;
				}
			}

			c = retrieveNode(c->NextSibling());
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
			ss << retrieveNode(mn->FirstChild())->Value();
			ss >> N;

			std::cout << "N: " << retrieveNode(mn->FirstChild())->Value() << std::endl;

			ss.clear();
			mn = retrieveNode(mn->NextSibling());
			ss << retrieveNode(mn->FirstChild())->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, DiscreteRotationSymmetryMap2D>>(discrete_rotation_symmetry(*prim, p, N));
		}
		else if(!strcmp(mapstring.c_str(), "ContinuousRotationMapping")){
			Point<2> p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, ContinuousRotationSymmetryMap2D>>(continuous_rotation_symmetry(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "ContinuousTranslationMapping")){
			Point<2> p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
			ss >> p;
			return std::make_shared<SymmetryTransformation<Primitive2D, ContinuousTranslationSymmetryMap2D>>(continuous_translation_symmetry(*prim, p));
		}
		else if(!strcmp(mapstring.c_str(), "DiscreteTranslationMapping")){
			Point<2> p;
			ss << retrieveNode(mapnode->FirstChild())->Value();
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

		if (!strcmp(retrieveXMLString(n),"CSGTree")) return buildGeometryFromXML2D<CSGTree<Primitive2D>>(n);
		else if (!strcmp(retrieveXMLString(n),"LinearTransformation")) return buildGeometryFromXML2D_LinearTransformation(n);
		else if (!strcmp(retrieveXMLString(n),"SymmetryTransformation")) return buildGeometryFromXML2D_SymmetryTransformation(n);
		else if (!strcmp(retrieveXMLString(n),"Circle")) return buildGeometryFromXML2D<Circle>(n);
		else if (!strcmp(retrieveXMLString(n),"Rectangle")) return buildGeometryFromXML2D<Rectangle>(n);
		else if (!strcmp(retrieveXMLString(n),"Ellipse")) return buildGeometryFromXML2D<Ellipse>(n);
		else if (!strcmp(retrieveXMLString(n),"Triangle")) return buildGeometryFromXML2D<Triangle>(n);
		else if (!strcmp(retrieveXMLString(n),"RegularPolygon")) return buildGeometryFromXML2D<RegularPolygon>(n);
		else if (!strcmp(retrieveXMLString(n),"XMLFile")){
			std::cout << "I am an XML file" << std::endl;
			std::cout << "name: " << retrieveXMLString(retrieveNode(n->FirstChild())) << std::endl;
			XMLDocument doc;
			doc.LoadFile(retrieveXMLString(retrieveNode(n->FirstChild())));
			return buildGeometryFromXML2D(retrieveNode(doc.FirstChild()));
		} 

		std::cerr << "Unrecognized geometry option: " << n->Value() << std::endl;
		throw -1;
		return std::make_shared<Circle>(Circle({0,0}, 0));
	}


	




	std::shared_ptr<Primitive2D> read2D(std::string filename){
		XMLDocument doc;
		doc.LoadFile(filename.c_str());
		return buildGeometryFromXML2D(retrieveNode(doc.FirstChild()));
	}




////////////////////////// 2D SCENE ///////////////////////////
void buildSceneFromXML2D_Background(const XMLNode * n, std::shared_ptr<Scene<std::string, Primitive2D>> s){
	s->background() = n->Value();
	std::cout << n->Value() << std::endl;
	return;
}

void buildSceneFromXML2D_Object(const XMLNode * n, std::shared_ptr<Scene<std::string, Primitive2D>> s){
	typedef Scene<std::string, Primitive2D> SceneType;
	std::string id;
	std::shared_ptr<Primitive2D> prim;

	while (n != nullptr){
		// for (unsigned int i=0; i<ntab; i++) std::cout << "\t" ;
		std::cout << n->Value() << ":   " << std::endl ;

		if (!strcmp(retrieveXMLString(n),"Identifier")){
			id = retrieveNode(n->FirstChild())->Value();
			std::cout << id << std::endl;
		} 
		else if (!strcmp(retrieveXMLString(n),"Primitive")){
			prim = buildGeometryFromXML2D(retrieveNode(n->FirstChild()));
			prim->print_summary();
		}
		else{
			std::cerr << "Unrecognized scene option: " << n->Value() << std::endl;
			throw -1;
		}

		
		n = retrieveNode(n->NextSibling());
	}
	
	s->insert(id, prim);
	return;
}



void buildSceneFromXML2D(const XMLNode * n, std::shared_ptr<Scene<std::string, Primitive2D>> s){
	typedef Scene<std::string, Primitive2D> SceneType;

	while (n != nullptr){
		// for (unsigned int i=0; i<ntab; i++) std::cout << "\t" ;
		std::cout << n->Value() << ":   " << std::endl ;

		if (!strcmp(retrieveXMLString(n),"Object")) buildSceneFromXML2D_Object(retrieveNode(n->FirstChild()), s);
		else if (!strcmp(retrieveXMLString(n),"Background")) buildSceneFromXML2D_Background(retrieveNode(n->FirstChild()), s);
		else{
			std::cerr << "Unrecognized scene option: " << n->Value() << std::endl;
			throw -1;
		}

		
		n = retrieveNode(n->NextSibling());
	}
	return;
}



std::shared_ptr<Scene<std::string, Primitive2D>> read2DScene(std::string filename){
	typedef Scene<std::string, Primitive2D> SceneType;
	XMLDocument doc;
	doc.LoadFile(filename.c_str());
	std::shared_ptr<SceneType> s = std::make_shared<SceneType>(SceneType("background"));
	buildSceneFromXML2D(retrieveNode(doc.FirstChild()), s);
	return s;
}

///////////////////////// END 2D SCENE ///////////////////////



////////////////////////// 2D FRAME ///////////////////////////


///////////////////////// END 2D FRAME ///////////////////////









//////////////////////// 3D GEOMETRY READ /////////////////////////////
	// forward declare this
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D(const XMLNode * n);

	// declare the templatized version
	template <typename T>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D(const XMLNode * n){
	}

	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<Sphere>(const XMLNode * n){
		std::cout << "\t output is a Sphere" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasRadius = false;
		double rad;
		Point<3> cen;

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

		return std::make_shared<Sphere>(Sphere(cen, rad));
	}





	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<Cylinder>(const XMLNode * n){
		std::cout << "\t output is a Cylinder" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasNormal = false, hasXDir = false, hasRadius = false, hasHeight = false;
		Point<3> cen, normal, xdir;
		double radius, height;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Normal")){
				hasNormal = true;
				ss << c->FirstChild()->Value();
				ss >> normal;
			}
			else if(!strcmp(c->Value(), "XDir")){
				hasXDir = true;
				ss << c->FirstChild()->Value();
				ss >> xdir;
			}
			else if(!strcmp(c->Value(), "Radius")){
				hasRadius = true;
				ss << c->FirstChild()->Value();
				ss >> radius;
			}
			else if(!strcmp(c->Value(), "Height")){
				hasHeight = true;
				ss << c->FirstChild()->Value();
				ss >> height;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasNormal || !hasXDir || !hasRadius || !hasHeight){
			std::cerr << "Cylinder format prototype is: " << std::endl;
			std::cerr << "\t<Cylinder>" << std::endl;
			std::cerr << "\t\t<Center>(c0,c1,c2)</Center>" << std::endl;
			std::cerr << "\t\t<Normal>(n0,n1,n2)</Normal>" << std::endl;
			std::cerr << "\t\t<XDir>(x0,x1,x2)</XDir>" << std::endl;
			std::cerr << "\t\t<Radius>rad</Radius>" << std::endl;
			std::cerr << "\t\t<Height>h</Height>" << std::endl;
			std::cerr << "\t</Cylinder>" << std::endl;

			throw -1;
		}

		return std::make_shared<Cylinder>(Cylinder(cen, normal, xdir, radius, height));
	}





	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<RectangularPrism>(const XMLNode * n){
		std::cout << "\t output is a RectangularPrism" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasNormal = false, hasXDir = false, hasDims = false, hasHeight = false;
		Point<3> cen, normal, xdir;
		Point<2> dims;
		double  height;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Normal")){
				hasNormal = true;
				ss << c->FirstChild()->Value();
				ss >> normal;
			}
			else if(!strcmp(c->Value(), "XDir")){
				hasXDir = true;
				ss << c->FirstChild()->Value();
				ss >> xdir;
			}
			else if(!strcmp(c->Value(), "Dims")){
				hasDims = true;
				ss << c->FirstChild()->Value();
				ss >> dims;
			}
			else if(!strcmp(c->Value(), "Height")){
				hasHeight = true;
				ss << c->FirstChild()->Value();
				ss >> height;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasNormal || !hasXDir || !hasDims || !hasHeight){
			std::cerr << "RectangularPrism format prototype is: " << std::endl;
			std::cerr << "\t<RectangularPrism>" << std::endl;
			std::cerr << "\t\t<Center>(c0,c1,c2)</Center>" << std::endl;
			std::cerr << "\t\t<Normal>(n0,n1,n2)</Normal>" << std::endl;
			std::cerr << "\t\t<XDir>(x0,x1,x2)</XDir>" << std::endl;
			std::cerr << "\t\t<Dims>(length, width)</Dims>" << std::endl;
			std::cerr << "\t\t<Height>h</Height>" << std::endl;
			std::cerr << "\t</RectangularPrism>" << std::endl;

			throw -1;
		}

		return std::make_shared<RectangularPrism>(RectangularPrism(cen, normal, xdir, dims, height));
	}





	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<Pyramid>(const XMLNode * n){
		std::cout << "\t output is a Pyramid" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasNormal = false, hasXDir = false, hasBase = false, hasHeight = false;
		Point<3> cen, normal, xdir;
		double height;
		std::shared_ptr<Primitive2D> prim2;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Normal")){
				hasNormal = true;
				ss << c->FirstChild()->Value();
				ss >> normal;
			}
			else if(!strcmp(c->Value(), "XDir")){
				hasXDir = true;
				ss << c->FirstChild()->Value();
				ss >> xdir;
			}
			else if(!strcmp(c->Value(), "Base")){
				hasBase = true;
				prim2 = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Height")){
				hasHeight = true;
				ss << c->FirstChild()->Value();
				ss >> height;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasNormal || !hasXDir || !hasBase || !hasHeight){
			std::cerr << "Pyramid format prototype is: " << std::endl;
			std::cerr << "\t<Pyramid>" << std::endl;
			std::cerr << "\t\t<Base>[primitive 2d type here]</Base>" << std::endl;
			std::cerr << "\t\t<Center>(c0,c1,c2)</Center>" << std::endl;
			std::cerr << "\t\t<Normal>(n0,n1,n2)</Normal>" << std::endl;
			std::cerr << "\t\t<XDir>(x0,x1,x2)</XDir>" << std::endl;
			std::cerr << "\t\t<Height>h</Height>" << std::endl;
			std::cerr << "\t</Pyramid>" << std::endl;

			throw -1;
		}

		return std::make_shared<Pyramid>(Pyramid(*prim2, cen, normal, xdir, height));
	}



	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<Extrusion>(const XMLNode * n){
		std::cout << "\t output is a Extrusion" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasNormal = false, hasXDir = false, hasBase = false, hasHeight = false;
		Point<3> cen, normal, xdir;
		double height;
		std::shared_ptr<Primitive2D> prim2;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Normal")){
				hasNormal = true;
				ss << c->FirstChild()->Value();
				ss >> normal;
			}
			else if(!strcmp(c->Value(), "XDir")){
				hasXDir = true;
				ss << c->FirstChild()->Value();
				ss >> xdir;
			}
			else if(!strcmp(c->Value(), "Base")){
				hasBase = true;
				prim2 = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Height")){
				hasHeight = true;
				ss << c->FirstChild()->Value();
				ss >> height;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasNormal || !hasXDir || !hasBase || !hasHeight){
			std::cerr << "Extrusion format prototype is: " << std::endl;
			std::cerr << "\t<Extrusion>" << std::endl;
			std::cerr << "\t\t<Base>[primitive 2d type here]</Base>" << std::endl;
			std::cerr << "\t\t<Center>(c0,c1,c2)</Center>" << std::endl;
			std::cerr << "\t\t<Normal>(n0,n1,n2)</Normal>" << std::endl;
			std::cerr << "\t\t<XDir>(x0,x1,x2)</XDir>" << std::endl;
			std::cerr << "\t\t<Height>h</Height>" << std::endl;
			std::cerr << "\t</Extrusion>" << std::endl;

			throw -1;
		}

		return std::make_shared<Extrusion>(Extrusion(*prim2, cen, normal, xdir, height));
	}




	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<Sweep>(const XMLNode * n){
		std::cout << "\t output is a Sweep" << std::endl;
		auto c = n->FirstChild();

		bool hasCenter = false, hasNormal = false, hasXDir = false, hasBase = false, hasAngle = false, hasLinePoint = false, hasLineDir = false;
		Point<3> cen, normal, xdir, lpt, ldir;
		double angle;
		std::shared_ptr<Primitive2D> prim2;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Center")){
				hasCenter = true;
				ss << c->FirstChild()->Value();
				ss >> cen;
			}
			else if(!strcmp(c->Value(), "Normal")){
				hasNormal = true;
				ss << c->FirstChild()->Value();
				ss >> normal;
			}
			else if(!strcmp(c->Value(), "XDir")){
				hasXDir = true;
				ss << c->FirstChild()->Value();
				ss >> xdir;
			}
			else if(!strcmp(c->Value(), "LinePoint")){
				hasLinePoint = true;
				ss << c->FirstChild()->Value();
				ss >> lpt;
			}
			else if(!strcmp(c->Value(), "LineDir")){
				hasLineDir = true;
				ss << c->FirstChild()->Value();
				ss >> ldir;
			}
			else if(!strcmp(c->Value(), "Base")){
				hasBase = true;
				prim2 = buildGeometryFromXML2D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Angle")){
				hasAngle = true;
				ss << c->FirstChild()->Value();
				ss >> angle;
			}

			c = c->NextSibling();
		}

		if (!hasCenter || !hasNormal || !hasXDir || 
			!hasBase || !hasAngle || !hasLinePoint || !hasLineDir){
			std::cerr << "Sweep format prototype is: " << std::endl;
			std::cerr << "\t<Sweep>" << std::endl;
			std::cerr << "\t\t<Base>[primitive 2d type here]</Base>" << std::endl;
			std::cerr << "\t\t<Center>(c0,c1,c2)</Center>" << std::endl;
			std::cerr << "\t\t<Normal>(n0,n1,n2)</Normal>" << std::endl;
			std::cerr << "\t\t<XDir>(x0,x1,x2)</XDir>" << std::endl;
			std::cerr << "\t\t<LinePoint>(p0,p1,p2)</LinePoint>" << std::endl;
			std::cerr << "\t\t<LineDir>(l0,l1,l2)</LineDir>" << std::endl;
			std::cerr << "\t\t<Angle>alpha_degrees</Angle>" << std::endl;
			std::cerr << "\t</Sweep>" << std::endl;

			throw -1;
		}

		return std::make_shared<Sweep>(Sweep(*prim2, cen, normal, xdir, Line<3>(lpt, ldir), angle));
	}




	template <>
	std::shared_ptr<Primitive3D> buildGeometryFromXML3D<CSGTree<Primitive3D>>(const XMLNode * n){
		std::cout << "\t output is a CSGTree" << std::endl;
		auto c = n->FirstChild();

		bool hasLeft = false, hasRight = false, hasOperation = false;
		std::shared_ptr<Primitive3D> left, right;
		Operation op = DIFFERENCE;

		while (c != nullptr){
			std::stringstream ss;

			if (!strcmp(c->Value(), "Left")){
				hasLeft = true;
				left = buildGeometryFromXML3D(c->FirstChild());
			}
			else if(!strcmp(c->Value(), "Right")){
				hasRight = true;
				right = buildGeometryFromXML3D(c->FirstChild());
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

		return std::make_shared<CSGTree<Primitive3D>>(CSGTree<Primitive3D>(left, right, op));
	}




	std::shared_ptr<Primitive3D> buildGeometryFromXML3D_LinearTransformation(const XMLNode * n){
		std::cout << "\t output is a LinearTransformation" << std::endl;
		auto c = n->FirstChild();

		bool hasMapping = false, hasPrimitive = false;
		std::shared_ptr<Primitive3D> prim;
		std::string mapstring;
		const XMLNode * mapnode;

		while (c != nullptr){

			if (!strcmp(c->Value(), "Primitive")){
				hasPrimitive = true;
				prim = buildGeometryFromXML3D(c->FirstChild());
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
			Point<3> up, lo;
			ss << mapnode->FirstChild()->FirstChild()->Value();
			ss >> up;
			ss.clear();
			ss << mapnode->FirstChild()->NextSibling()->FirstChild()->Value();
			ss >> lo;
			return std::make_shared<LinearTransformation<Primitive3D, ShearMap3D>>(shear_transformation(*prim, up, lo));
		}
		// else if(!strcmp(mapstring.c_str(), "DilatationMapping")){
		// 	Point<2> p;
		// 	ss << mapnode->FirstChild()->Value();
		// 	ss >> p;
		// 	return std::make_shared<LinearTransformation<Primitive3D, DilatationMap2D>>(dilatation_transformation(*prim, p));
		// }
		// else if(!strcmp(mapstring.c_str(), "RotationMapping")){
		// 	double p;
		// 	ss << mapnode->FirstChild()->Value();
		// 	ss >> p;
		// 	return std::make_shared<LinearTransformation<Primitive3D, RotationMap2D>>(rotation_transformation(*prim, p));
		// }
		// else if(!strcmp(mapstring.c_str(), "TranslationMapping")){
		// 	Point<2> p;
		// 	ss << mapnode->FirstChild()->Value();
		// 	ss >> p;
		// 	return std::make_shared<LinearTransformation<Primitive3D, TranslationMap2D>>(translation_transformation(*prim, p));
		// }
		else{
			std::cerr << "Operation value must be one of [ShearMapping, DilatationMapping, RotationMapping, TranslationMapping]" << std::endl;
			throw -1;
		}

		// return std::make_shared<LinearTransformation<Primitive3D>>(LinearTransformation<Primitive3D>(left, right, op));
	}





	// std::shared_ptr<Primitive3D> buildGeometryFromXML3D_SymmetryTransformation(const XMLNode * n){
	// 	std::cout << "\t output is a SymmetryTransformation" << std::endl;
	// 	auto c = n->FirstChild();

	// 	bool hasMapping = false, hasPrimitive = false;
	// 	std::shared_ptr<Primitive3D> prim;
	// 	std::string mapstring;
	// 	const XMLNode * mapnode;

	// 	while (c != nullptr){

	// 		if (!strcmp(c->Value(), "Primitive")){
	// 			hasPrimitive = true;
	// 			prim = buildGeometryFromXML3D(c->FirstChild());
	// 		}
	// 		else if(!strcmp(c->Value(), "Mapping")){
	// 			hasMapping = true;
	// 			mapnode = c->FirstChild();
	// 			if(!strcmp(c->FirstChild()->Value(), "DiscreteRotationMapping")) mapstring = "DiscreteRotationMapping";
	// 			else if(!strcmp(c->FirstChild()->Value(), "ContinuousRotationMapping")) mapstring = "ContinuousRotationMapping";
	// 			else if(!strcmp(c->FirstChild()->Value(), "ContinuousTranslationMapping"))	mapstring = "ContinuousTranslationMapping";
	// 			else if(!strcmp(c->FirstChild()->Value(), "DiscreteTranslationMapping"))	mapstring = "DiscreteTranslationMapping";
	// 			else{
	// 				std::cerr << "Operation value must be one of [DiscreteRotationMapping, ContinuousRotationMapping, DiscreteTranslationMapping, ContinuousTranslationMapping]" << std::endl;
	// 				throw -1;
	// 			}
	// 		}

	// 		c = c->NextSibling();
	// 	}

	// 	if (!hasMapping || !hasPrimitive){
	// 		std::cerr << "SymmetryTransformation format prototype is: " << std::endl;
	// 		std::cerr << "\t<SymmetryTransformation>" << std::endl;
	// 		std::cerr << "\t\t<Mapping>[a mapping type here]</Mapping>" << std::endl;
	// 		std::cerr << "\t\t<Primitive>[a primitive type here]</Primitive>" << std::endl;
	// 		std::cerr << "\t</SymmetryTransformation>" << std::endl;
	// 		throw -1;
	// 	}


	// 	std::stringstream ss;
	// 	if(!strcmp(mapstring.c_str(), "DiscreteRotationMapping")){
	// 		Point<2> p;
	// 		std::size_t N;
	// 		const XMLNode * mn = mapnode->FirstChild();
	// 		ss << mn->FirstChild()->Value();
	// 		ss >> N;

	// 		std::cout << "N: " << mn->FirstChild()->Value() << std::endl;

	// 		ss.clear();
	// 		mn = mn->NextSibling();
	// 		ss << mn->FirstChild()->Value();
	// 		ss >> p;
	// 		return std::make_shared<SymmetryTransformation<Primitive3D, DiscreteRotationSymmetryMap2D>>(discrete_rotation_symmetry(*prim, p, N));
	// 	}
	// 	else if(!strcmp(mapstring.c_str(), "ContinuousRotationMapping")){
	// 		Point<2> p;
	// 		ss << mapnode->FirstChild()->Value();
	// 		ss >> p;
	// 		return std::make_shared<SymmetryTransformation<Primitive3D, ContinuousRotationSymmetryMap2D>>(continuous_rotation_symmetry(*prim, p));
	// 	}
	// 	else if(!strcmp(mapstring.c_str(), "ContinuousTranslationMapping")){
	// 		double p;
	// 		ss << mapnode->FirstChild()->Value();
	// 		ss >> p;
	// 		return std::make_shared<SymmetryTransformation<Primitive3D, ContinuousTranslationSymmetryMap2D>>(continuous_translation_symmetry(*prim, p));
	// 	}
	// 	else if(!strcmp(mapstring.c_str(), "DiscreteTranslationMapping")){
	// 		Point<2> p;
	// 		ss << mapnode->FirstChild()->Value();
	// 		ss >> p;
	// 		return std::make_shared<SymmetryTransformation<Primitive3D, DiscreteTranslationSymmetryMap2D>>(discrete_translation_symmetry(*prim, p));
	// 	}
	// 	else{
	// 		std::cerr << "Operation value must be one of [DiscreteRotationMapping, ContinuousRotationMapping, DiscreteTranslationMapping, ContinuousTranslationMapping]" << std::endl;
	// 		throw -1;
	// 	}

	// 	// return std::make_shared<LinearTransformation<Primitive3D>>(LinearTransformation<Primitive3D>(left, right, op));
	// }




	std::shared_ptr<Primitive3D> buildGeometryFromXML3D(const XMLNode * n){
		if (n == nullptr){
			std::cerr << "Failed to complete an XML tag... make sure it is closed" << std::endl;
			throw -1;
			return std::make_shared<Sphere>(Sphere({0,0,0}, 0));
		}

		// for (unsigned int i=0; i<ntab; i++) std::cout << "\t" ;
		std::cout << n->Value() << ":   " << std::endl ;

		if (!strcmp(n->Value(),"CSGTree")) return buildGeometryFromXML3D<CSGTree<Primitive3D>>(n);
		else if (!strcmp(n->Value(),"LinearTransformation")) return buildGeometryFromXML3D_LinearTransformation(n);
		// else if (!strcmp(n->Value(),"SymmetryTransformation")) return buildGeometryFromXML3D_SymmetryTransformation(n);
		else if (!strcmp(n->Value(),"Sphere")) return buildGeometryFromXML3D<Sphere>(n);
		else if (!strcmp(n->Value(),"Cylinder")) return buildGeometryFromXML3D<Cylinder>(n);
		else if (!strcmp(n->Value(),"RectangularPrism")) return buildGeometryFromXML3D<RectangularPrism>(n);
		else if (!strcmp(n->Value(),"Pyramid")) return buildGeometryFromXML3D<Pyramid>(n);
		else if (!strcmp(n->Value(),"Extrusion")) return buildGeometryFromXML3D<Extrusion>(n);
		else if (!strcmp(n->Value(),"Sweep")) return buildGeometryFromXML3D<Sweep>(n);
		

		std::cerr << "Unrecognized geometry option: " << n->Value() << std::endl;
		throw -1;
		return std::make_shared<Sphere>(Sphere({0,0,0}, 0));
	}




	std::shared_ptr<Primitive3D> read3D(std::string filename){
		XMLDocument doc;
		doc.LoadFile(filename.c_str());
		return buildGeometryFromXML3D(doc.FirstChild());
	}







	//////////////////////// READ OF OTHER PRIMITIVES /////////////////////////////
















	
#endif





} // end namespace csg
#endif