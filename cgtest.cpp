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
	c0.print_summary();
	cout << endl;

	// rectangle
	cout << "******* Rectangle *******" << endl;
	Rectangle r0(Point<2>(0.5, 0.5), Point<2>(0.3, 0.4));
	r0.print_summary();
	cout << endl;

	// ellipse
	cout << "******* Ellipse *******" << endl;
	Ellipse e0(Point<2>(0.5, 0.5), Point<2>(1, 1.5));
	e0.print_summary();
	cout << endl;

	// triangle
	cout << "******* Triangle *******" << endl;
	Triangle t0(Point<2>(0,0), Point<2>(1,0), Point<2>(0,1));
	t0.print_summary();
	cout << endl;

	// polycurve
	cout << "******* Polycurve *******" << endl;
	vector<Segment<2>> seg;
	seg.push_back(LineSegment(Point<2>(0,0),Point<2>(-1,1)));
	seg.push_back(LineSegment(Point<2>(-1,1),Point<2>(0,2)));
	seg.push_back(CircleSegment(Point<2>(0,2),Point<2>(0,0),5,true));
	Polycurve pc0(seg);
	pc0.print_summary();
	cout << endl;


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


	// regular polygon
	cout << "******* Regular Polygon *******" << endl;
	RegularPolygon rpg0(8, Point<2>(0,-2), 1);
	rpg0.print_summary();
	cout << endl;


	// build a geometry model
	shared_ptr<CSGeometry2D> obj1(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,2))), shared_ptr<Primitive2D>(new Circle(Point<2>(0,0), 0.6)), DIFFERENCE));
	obj1->print_summary();
	shared_ptr<CSGeometry2D> obj2(new CSGeometry2D(shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(0.3,2))), shared_ptr<Primitive2D>(new Rectangle(Point<2>(0,0),Point<2>(2,0.3))), UNION));
	obj2->print_summary();

	shared_ptr<CSGeometry2D> window(new CSGeometry2D(obj1, obj2, UNION));
	window->print_summary();
	// CSGeometry2D()


	return 0;
}