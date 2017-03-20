#include <iostream>

#include <csg.h>

using namespace std;
using namespace csg;


// compile this with command:
// 			clang++ -std=c++11 -I./ cgtest.cpp -o cgtest		

int main(int argc, char * argv[])
{
	
	// test 2D primitives
	// circle
	cout << "\n******* Circle *******" << endl;
	Circle c0 (Point<2>(0.5, 0.5), 1.0);
	c0.print_summary(); cout << endl;
	cout << "BoundingBox: " << c0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.25, 0.25) << "? " << int(c0.contains_point(Point<2>(0.25,0.25))) << endl;
	cout << "Contains Point " << Point<2>(-0.5, 0.5) << "? " << int(c0.contains_point(Point<2>(-0.5,0.5))) << endl;
	cout << "Contains Point " << Point<2>(1.5, 1.5) << "? " << int(c0.contains_point(Point<2>(1.5,1.5))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.2, -0.2),Point<2>(0.2,0.2)) << "? " << int(c0.contains_box(Box<2>(Point<2>(-0.2, -0.2),Point<2>(0.2,0.2)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.2,0.2)) << "? " << int(c0.contains_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.2,0.2)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.2, -0.2),Point<2>(0.2,0.2)) << "? " << int(c0.collides_box(Box<2>(Point<2>(-0.2, -0.2),Point<2>(0.2,0.2)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.2,0.2)) << "? " << int(c0.collides_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.2,0.2)))) << endl;


	// rectangle
	cout << "\n******* Rectangle *******" << endl;
	Rectangle r0(Point<2>(0.5, 0.5), Point<2>(0.3, 0.4));
	r0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << r0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.6) << "? " << int(r0.contains_point(Point<2>(0.5, 0.6))) << endl;
	cout << "Contains Point " << Point<2>(0.65, 0.7) << "? " << int(r0.contains_point(Point<2>(0.65, 0.7))) << endl;
	cout << "Contains Point " << Point<2>(0.65, 0.71) << "? " << int(r0.contains_point(Point<2>(0.65, 0.71))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.5,0.5),Point<2>(0.6,0.6)) << "? " << int(r0.contains_box(Box<2>(Point<2>(0.5, 0.5),Point<2>(0.6,0.6)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.3,-0.3),Point<2>(0.6,0.6)) << "? " << int(r0.contains_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.6,0.6)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.5,0.5),Point<2>(0.6,0.6)) << "? " << int(r0.collides_box(Box<2>(Point<2>(0.5, 0.5),Point<2>(0.6,0.6)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.3,-0.3),Point<2>(0.6,0.6)) << "? " << int(r0.collides_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.6,0.6)))) << endl;


	// ellipse
	cout << "\n******* Ellipse *******" << endl;
	Ellipse e0(Point<2>(0.5, 0.5), Point<2>(1, 1.5));
	e0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << e0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.6) << "? " << int(e0.contains_point(Point<2>(0.5, 0.6))) << endl;
	cout << "Contains Point " << Point<2>(1.0, 0.5) << "? " << int(e0.contains_point(Point<2>(1.0, 0.5))) << endl;
	cout << "Contains Point " << Point<2>(1.1, 0.5) << "? " << int(e0.contains_point(Point<2>(1.1, 0.5))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.5,0.5),Point<2>(0.6,0.6)) << "? " << int(e0.contains_box(Box<2>(Point<2>(0.5, 0.5),Point<2>(0.6,0.6)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.3,-0.3),Point<2>(0.6,0.6)) << "? " << int(e0.contains_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.6,0.6)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.5,0.5),Point<2>(0.6,0.6)) << "? " << int(e0.collides_box(Box<2>(Point<2>(0.5, 0.5),Point<2>(0.6,0.6)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.3,-0.3),Point<2>(0.6,0.6)) << "? " << int(e0.collides_box(Box<2>(Point<2>(-0.3, -0.3),Point<2>(0.6,0.6)))) << endl;


	// triangle
	cout << "\n******* Triangle *******" << endl;
	Triangle t0(Point<2>(0,0), Point<2>(1,0), Point<2>(0,1));
	t0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << t0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.4, 0.4) << "? " << int(t0.contains_point(Point<2>(0.4, 0.4))) << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.5) << "? " << int(t0.contains_point(Point<2>(0.5, 0.5))) << endl;
	cout << "Contains Point " << Point<2>(0.51, 0.51) << "? " << int(t0.contains_point(Point<2>(0.51, 0.51))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(t0.contains_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(t0.contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(t0.collides_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(t0.collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;

	// polycurve
	cout << "\n******* Polycurve *******" << endl;
	vector<shared_ptr<Segment<2>>> seg;
	seg.push_back(shared_ptr<Segment<2>>(new LineSegment(Point<2>(0,0),Point<2>(-1,1))));
	seg.push_back(shared_ptr<Segment<2>>(new LineSegment(Point<2>(-1,1),Point<2>(0,2))));
	seg.push_back(shared_ptr<Segment<2>>(new CircleSegment(Point<2>(0,2),Point<2>(0,0),5,true,true)));
	Polycurve pc0(seg);
	pc0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << pc0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(-0.9, 1.0) << "? " << int(pc0.contains_point(Point<2>(-0.9,1.0))) << endl;
	cout << "Contains Point " << Point<2>(0.1, 1.0) << "? " << int(pc0.contains_point(Point<2>(0.1, 1.0))) << endl;
	cout << "Contains Point " << Point<2>(0.5, 1.0) << "? " << int(pc0.contains_point(Point<2>(0.5, 1.0))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(pc0.contains_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(pc0.contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(pc0.collides_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(pc0.collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;


	// polygon
	cout << "\n******* Polygon *******" << endl;
	vector<LineSegment> seg2;
	seg2.push_back(LineSegment(Point<2>(0,0),Point<2>(-1,1)));
	seg2.push_back(LineSegment(Point<2>(-1,1),Point<2>(0,2)));
	seg2.push_back(LineSegment(Point<2>(0,2),Point<2>(2,0)));
	seg2.push_back(LineSegment(Point<2>(2,0),Point<2>(0,0)));
	Polygon pg0(seg2);
	pg0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << pg0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(-0.9, 1.0) << "? " << int(pg0.contains_point(Point<2>(-0.9,1.0))) << endl;
	cout << "Contains Point " << Point<2>(0.1, 1.0) << "? " << int(pg0.contains_point(Point<2>(0.1, 1.0))) << endl;
	cout << "Contains Point " << Point<2>(1.1, 1.0) << "? " << int(pg0.contains_point(Point<2>(1.1, 1.0))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(pg0.contains_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(pg0.contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(pg0.collides_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(pg0.collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;



	// regular polygon
	cout << "\n******* Regular Polygon *******" << endl;
	RegularPolygon rpg0(5, Point<2>(0,-2), 1);
	rpg0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << rpg0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0,-2) << "? " << int(rpg0.contains_point(Point<2>(0,-2))) << endl;
	cout << "Contains Point " << Point<2>(0,-1) << "? " << int(rpg0.contains_point(Point<2>(0,-1))) << endl;
	cout << "Contains Point " << Point<2>(0,0) << "? " << int(rpg0.contains_point(Point<2>(0,0))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(rpg0.contains_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(rpg0.contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(0.1,0.1),Point<2>(0.4,0.4)) << "? " << int(rpg0.collides_box(Box<2>(Point<2>(0.1, 0.1),Point<2>(0.4,0.4)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.1,0.1)) << "? " << int(rpg0.collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.1,0.1)))) << endl;



	// build a 2D geometry model
	cout << "\n******* CSGeometry2D *******" << endl;
	shared_ptr<CSGeometry2D> obj1(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,2))), shared_ptr<Primitive2D>(new Circle(Point<2>(0,0), 0.6)), DIFFERENCE));
	obj1->print_summary();
	shared_ptr<CSGeometry2D> obj2(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(0.3,2))), shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,0.3))), UNION));
	obj2->print_summary();

	shared_ptr<CSGeometry2D> window(new CSGeometry2D(obj1, obj2, UNION));
	window->print_summary();
	cout << "BoundingBox: " << window->get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.4,0) << "? " << int(window->contains_point(Point<2>(0.4,0))) << endl;
	cout << "Contains Point " << Point<2>(0.4,0.15) << "? " << int(window->contains_point(Point<2>(0.4,0.15))) << endl;
	cout << "Contains Point " << Point<2>(0.4,0.16) << "? " << int(window->contains_point(Point<2>(0.4,0.16))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.14,-0.14),Point<2>(0.14,0.14)) << "? " << int(window->contains_box(Box<2>(Point<2>(-0.14, -0.14),Point<2>(0.14,0.14)))) << endl;
	cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.16,0.16)) << "? " << int(window->contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.16,0.16)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.14,-0.14),Point<2>(0.14,0.14)) << "? " << int(window->collides_box(Box<2>(Point<2>(-0.14, -0.14),Point<2>(0.14,0.14)))) << endl;
	cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.16,0.16)) << "? " << int(window->collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.16,0.16)))) << endl;



	// tests 3D primitives
	// sphere
	cout << "\n******* Sphere *******" << endl;
	Sphere s0 (Point<3>(0.5, 0.5, 0.5), 1.0);
	s0.print_summary(); cout << endl;
	cout << "BoundingBox: " << s0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<3>(0.25, 0.25, 0.25) << "? " << int(s0.contains_point(Point<3>(0.25,0.25,0.25))) << endl;
	cout << "Contains Point " << Point<3>(-0.5, 0.5, 0.5) << "? " << int(s0.contains_point(Point<3>(-0.5,0.5,0.5))) << endl;
	cout << "Contains Point " << Point<3>(1.5, 1.5, 1.5) << "? " << int(s0.contains_point(Point<3>(1.5,1.5,1.5))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;

	// cylinder
	cout << "\n******* Cylinder *******" << endl;
	Cylinder cy0 (Point<3>(0,0,0), Point<3>(0,0,1), Point<3>(1,0,0), 1.0, 1.0);
	cy0.print_summary(); cout << endl;
	cout << "BoundingBox: " << cy0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<3>(0.25, 0.25, 0.25) << "? " << int(cy0.contains_point(Point<3>(0.25,0.25,0.25))) << endl;
	cout << "Contains Point " << Point<3>(-1.0, 0.0, 0.5) << "? " << int(cy0.contains_point(Point<3>(-1.0,0.0,0.5))) << endl;
	cout << "Contains Point " << Point<3>(1.5, 1.5, 1.5) << "? " << int(cy0.contains_point(Point<3>(1.5,1.5,1.5))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;


	cout << "\n******* Pyramid *******" << endl;
	Pyramid py0 (pg0, Point<3>(0,0,0), Point<3>(0,0,1), Point<3>(1,0,0), 1.0);
	py0.print_summary(); cout << endl;
	cout << "BoundingBox: " << py0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<3>(0.25, 0.25, 0.25) << "? " << int(py0.contains_point(Point<3>(0.25,0.25,0.25))) << endl;
	cout << "Contains Point " << Point<3>(0.9, 0.9, 0.1) << "? " << int(py0.contains_point(Point<3>(0.9, 0.9, 0.1))) << endl;
	cout << "Contains Point " << Point<3>(1.5, 1.5, 1.5) << "? " << int(py0.contains_point(Point<3>(1.5,1.5,1.5))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;


	cout << "\n******* Extrusion *******" << endl;
	Extrusion ex0 (pg0, Point<3>(0,0,0), Point<3>(0,0,1), Point<3>(1,0,0), 1.0);
	ex0.print_summary(); cout << endl;
	cout << "BoundingBox: " << ex0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<3>(0.25, 0.25, 0.25) << "? " << int(ex0.contains_point(Point<3>(0.25,0.25,0.25))) << endl;
	cout << "Contains Point " << Point<3>(0.9, 0.9, 0.5) << "? " << int(ex0.contains_point(Point<3>(0.9,0.9,0.5))) << endl;
	cout << "Contains Point " << Point<3>(1.5, 1.5, 1.5) << "? " << int(ex0.contains_point(Point<3>(1.5,1.5,1.5))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Contains Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.contains_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.2, -0.2),Point<3>(0.2,0.2)))) << endl;
	// cout << "Collides Box " << Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)) << "? " << int(s0.collides_box(Box<3>(Point<3>(-0.3, -0.3),Point<3>(0.2,0.2)))) << endl;




	// build a 3D geometry model
	cout << "\n******* CSGeometry3D *******" << endl;
	CSGeometry3D obj31(Sphere(Point<3>(0,0,0),1), Cylinder(Point<3>(-1,0,0),Point<3>(1,0,0),Point<3>(0,0,1), 0.3, 2.0), DIFFERENCE);
	obj31.print_summary();
	CSGeometry3D obj32(Cylinder(Point<3>(0,0,1),Point<3>(0,0,1),Point<3>(1,0,0), 0.3, 1.0), Cylinder(Point<3>(-0.5,0,2),Point<3>(1,0,0),Point<3>(0,0,1), 0.3, 1.0), UNION);
	obj32.print_summary();

	CSGeometry3D window3(obj31, obj32, UNION);
	window3.print_summary();
	cout << "BoundingBox: " << window3.get_bounding_box() << endl;
	cout << "Contains Point " << Point<3>(0,0,0.5) << "? " << int(window3.contains_point(Point<3>(0,0,0.5))) << endl;
	cout << "Contains Point " << Point<3>(0,0,2.3) << "? " << int(window3.contains_point(Point<3>(0,0,2.3))) << endl;
	cout << "Contains Point " << Point<3>(0,0,0) << "? " << int(window3.contains_point(Point<3>(0,0,0))) << endl;
	// cout << "Contains Box " << Box<2>(Point<2>(-0.14,-0.14),Point<2>(0.14,0.14)) << "? " << int(window->contains_box(Box<2>(Point<2>(-0.14, -0.14),Point<2>(0.14,0.14)))) << endl;
	// cout << "Contains Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.16,0.16)) << "? " << int(window->contains_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.16,0.16)))) << endl;
	// cout << "Collides Box " << Box<2>(Point<2>(-0.14,-0.14),Point<2>(0.14,0.14)) << "? " << int(window->collides_box(Box<2>(Point<2>(-0.14, -0.14),Point<2>(0.14,0.14)))) << endl;
	// cout << "Collides Box " << Box<2>(Point<2>(-0.1,0.1),Point<2>(0.16,0.16)) << "? " << int(window->collides_box(Box<2>(Point<2>(-0.1, 0.1),Point<2>(0.16,0.16)))) << endl;






	// trees 
	Orthtree<2,3,int> tree3;
	Quadtree<double> qtree;

	// build a quadtree of doubles corresponding to a cirlce centered at 0,0 with radius 1
	struct CircleThing{
		double rad = 1.0;
		Point2 cen = Point2(0,0);

		double getValue(Box<2> bx) const{
			Point2 p = 0.5*(bx.lo + bx.hi);
			return getValue(p);
		}

		double getValue(const Point2 & p) const{
			if (Point2::distsq(p,cen) > rad*rad) return 0.0;
			return 1.0;
		}

		bool isUniform(Box<2> bx) const{
			Point2 tl = bx.lo; tl.x[1] = bx.hi.x[1];
			Point2 br = bx.hi; br.x[1] = bx.lo.x[1];
			Point2 c = 0.5*(bx.hi+bx.lo);

			// cout << c << tl << br << endl;
			// cout << getValue(tl) << "  " << getValue(c) << endl;
			if (getValue(tl) == getValue(br) &&
				getValue(br) == getValue(bx.hi) &&
				getValue(bx.hi) == getValue(bx.lo) &&
				getValue(bx.lo) == getValue(c)) return true;
			return false;
		}
	};

	CircleThing c1;
	Box<2> bounds(Point2(-1,-1), Point2(1,1));
	qtree.buildTree(5, bounds, c1, c1);
	qtree.print_summary();


	// iterate over leaves
	int ctr=0;
	for (auto it=qtree.leaf_begin(); it!=qtree.leaf_end(); it++){
		ctr++;
	}
	cout << "leaf count: " << ctr << endl;

	return 0;
}
