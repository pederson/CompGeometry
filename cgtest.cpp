#include <iostream>

#include "include/GeomUtils.hpp"
#include "include/CSGeometry2D.hpp"

using namespace std;

int main(int argc, char * argv[])
{
	
	// test 2D primitives
	// circle
	cout << "******* Circle *******" << endl;
	Circle c0 (Point<2>(0.5, 0.5), 1.0);
	c0.print_summary(); cout << endl;
	cout << "BoundingBox: " << c0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.25, 0.25) << "? " << int(c0.contains_point(Point<2>(0.25,0.25))) << endl;
	cout << "Contains Point " << Point<2>(-0.5, 0.5) << "? " << int(c0.contains_point(Point<2>(-0.5,0.5))) << endl;
	cout << "Contains Point " << Point<2>(1.5, 1.5) << "? " << int(c0.contains_point(Point<2>(1.5,1.5))) << endl;

	// rectangle
	cout << "******* Rectangle *******" << endl;
	Rectangle r0(Point<2>(0.5, 0.5), Point<2>(0.3, 0.4));
	r0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << r0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.6) << "? " << int(r0.contains_point(Point<2>(0.5, 0.6))) << endl;
	cout << "Contains Point " << Point<2>(0.65, 0.7) << "? " << int(r0.contains_point(Point<2>(0.65, 0.7))) << endl;
	cout << "Contains Point " << Point<2>(0.65, 0.71) << "? " << int(r0.contains_point(Point<2>(0.65, 0.71))) << endl;


	// ellipse
	cout << "******* Ellipse *******" << endl;
	Ellipse e0(Point<2>(0.5, 0.5), Point<2>(1, 1.5));
	e0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << e0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.6) << "? " << int(e0.contains_point(Point<2>(0.5, 0.6))) << endl;
	cout << "Contains Point " << Point<2>(1.0, 0.5) << "? " << int(e0.contains_point(Point<2>(1.0, 0.5))) << endl;
	cout << "Contains Point " << Point<2>(1.1, 0.5) << "? " << int(e0.contains_point(Point<2>(1.1, 0.5))) << endl;


	// triangle
	cout << "******* Triangle *******" << endl;
	Triangle t0(Point<2>(0,0), Point<2>(1,0), Point<2>(0,1));
	t0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << t0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0.4, 0.4) << "? " << int(t0.contains_point(Point<2>(0.4, 0.4))) << endl;
	cout << "Contains Point " << Point<2>(0.5, 0.5) << "? " << int(t0.contains_point(Point<2>(0.5, 0.5))) << endl;
	cout << "Contains Point " << Point<2>(0.51, 0.51) << "? " << int(t0.contains_point(Point<2>(0.51, 0.51))) << endl;


	// polycurve
	cout << "******* Polycurve *******" << endl;
	vector<shared_ptr<Segment<2>>> seg;
	seg.push_back(shared_ptr<Segment<2>>(new LineSegment(Point<2>(0,0),Point<2>(-1,1))));
	seg.push_back(shared_ptr<Segment<2>>(new LineSegment(Point<2>(-1,1),Point<2>(0,2))));
	seg.push_back(shared_ptr<Segment<2>>(new CircleSegment(Point<2>(0,2),Point<2>(0,0),5,true)));
	Polycurve pc0(seg);
	pc0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << pc0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(-0.9, 1.0) << "? " << int(pc0.contains_point(Point<2>(-0.9,1.0))) << endl;
	cout << "Contains Point " << Point<2>(0.1, 1.0) << "? " << int(pc0.contains_point(Point<2>(0.1, 1.0))) << endl;
	cout << "Contains Point " << Point<2>(0.5, 1.0) << "? " << int(pc0.contains_point(Point<2>(0.5, 1.0))) << endl;


	// polygon
	cout << "******* Polygon *******" << endl;
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



	// regular polygon
	cout << "******* Regular Polygon *******" << endl;
	RegularPolygon rpg0(5, Point<2>(0,-2), 1);
	rpg0.print_summary();
	cout << endl;
	cout << "BoundingBox: " << rpg0.get_bounding_box() << endl;
	cout << "Contains Point " << Point<2>(0,-2) << "? " << int(rpg0.contains_point(Point<2>(0,-2))) << endl;
	cout << "Contains Point " << Point<2>(0,-1) << "? " << int(rpg0.contains_point(Point<2>(0,-1))) << endl;
	cout << "Contains Point " << Point<2>(0,0) << "? " << int(rpg0.contains_point(Point<2>(0,0))) << endl;



	// build a 2D geometry model
	shared_ptr<CSGeometry2D> obj1(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,2))), shared_ptr<Primitive2D>(new Circle(Point<2>(0,0), 0.6)), DIFFERENCE));
	obj1->print_summary();
	shared_ptr<CSGeometry2D> obj2(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(0.3,2))), shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,0.3))), UNION));
	obj2->print_summary();

	shared_ptr<CSGeometry2D> window(new CSGeometry2D(obj1, obj2, UNION));
	window->print_summary();
	cout << "BoundingBox: " << window->get_bounding_box() << endl;
	// CSGeometry2D()



	// tests 3D primitives


	return 0;
}