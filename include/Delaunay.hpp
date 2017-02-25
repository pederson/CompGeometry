#ifndef _DELAUNAY_H
#define _DELAUNAY_H

#include "GeomUtils.hpp"
#include "Primitive2D.hpp"

#include <unordered_map>
#include <stack>
#include <iostream>

// add some typedefs for consistency with the Numerical Recipes book
typedef unsigned long long int Ullong;
typedef double Doub;
typedef unsigned int Uint;
typedef int Int;

namespace csg{

struct RandomHash{
	inline Ullong int64(Ullong u){
		Ullong v = u * 3935559000370003845LL + 2691343689449507681LL;
		v ^= v >> 21; v ^= v << 37; v ^= v >> 4;
		v *= 4768777513237032717LL;
		v ^= v << 20;
		return v;
	}

	// Returns hash of u as a 32-bit integer.
	inline Uint int32(Ullong u)	{ return (Uint)(int64(u) & 0xffffffff) ; };

	//Returns hash of u as a double-precision floating value between 0. and 1.
	inline Doub doub(Ullong u) { return 5.42101086242752217E-20 * int64(u); };

};






// Return positive, zero, or negative value if point d is respectively inside, 
// on, or outside the circle through points a, b, and c.
inline double in_circle(const Point<2> & d, const Point<2> & a, const Point<2> & b, const Point<2> & c) {
	
	Circle cc = Circle::circumcircle(a,b,c);
	Point<2> ccen = cc.center();
	double crad = cc.radius();
	double radd = (d.x[0]-ccen.x[0])*(d.x[0]-ccen.x[0]) + (d.x[1]-ccen.x[1])*(d.x[1]-ccen.x[1]);
	return ((crad)*(crad) - radd);
}




struct TriElem{
	const Point<2> * 			points;	// the vector of points that this references
	Int  						vertices[3];	// the 3 vertices in this element
	Int 						daughters[3];	// locations of up to 3 daughters
	Int 						state;		// nonzero if the element is live

	void set(Int a, Int b, Int c, const Point<2> * pts)
	{
		points = pts;
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		daughters[0] = daughters[1] = daughters[2] = -1;
		state = 1;
	}

	Int contains_point(const Point<2> & pt){
		Doub d;
		Int ztest = 0;
		Int i,j;
		for (auto i=0; i<3; i++){
			j = (i+1)%3;
			d = (points[vertices[j]].x[0] - points[vertices[i]].x[0])
				*(pt.x[1]-points[vertices[i]].x[1])
				- (points[vertices[j]].x[1] - points[vertices[i]].x[1])
				*(pt.x[0]-points[vertices[i]].x[0]);
			if (d<0.0) return -1;
			if (d == 0.0) ztest = true;
		}
		return (ztest? 0 : 1);
	}
};







// const std::function<std::size_t(unsigned long int)> nullhash{return *(unsigned long int *)key;};

struct Delaunay {
	Int 						npts, ntri, ntree, ntreemax, opt;
	Doub 								delx, dely;
	std::vector<Point<2>> 				points;
	std::vector<TriElem> 				triangles;
	std::unordered_map<Ullong, Int> 	linehash;
	std::unordered_map<Ullong, Int> 	trihash;
	std::vector<Int> 					perm;	//Permutation for randomizing point order.
	RandomHash 							hashfn;

	static Uint jran;
	static const Doub fuzz, bigscale;


	//Construct Delaunay triangulation from a vector of points pvec. If bit 0 in options is nonzero,
	//hash memories used in the construction are deleted. (Some applications may want to use them
	//and will set options to 1.)
	Delaunay(std::vector<Point<2> > &pvec, Int options) 
	: npts(pvec.size()), ntri(0), ntree(0), ntreemax(10*npts+1000)
	, opt(options), points(pvec){
		
		Doub xl,xh,yl,yh;
		linehash.reserve(6*npts+12);
		trihash.reserve(2*npts+6);
		perm.resize(npts);
		triangles.resize(ntreemax);
		
		
		//Copy points to local store and calculate their
		//bounding box.
		xl = xh = pvec[0].x[0];
		yl = yh = pvec[0].x[1];
		for (auto j=0; j<npts; j++) {
			perm[j] = j;
			if (pvec[j].x[0] < xl) xl = pvec[j].x[0];
			if (pvec[j].x[0] > xh) xh = pvec[j].x[0];
			if (pvec[j].x[1] < yl) yl = pvec[j].x[1];
			if (pvec[j].x[1] > yh) yh = pvec[j].x[1];
		}
		delx = xh - xl;
		dely = yh - yl;

		//Store bounding box dimensions, then construct
		//the three fictitious points and store them.
		points.push_back(Point<2>(0.5*(xl + xh), yh + bigscale*dely));
		points.push_back(Point<2>(xl - 0.5*bigscale*delx,yl - 0.5*bigscale*dely));
		points.push_back(Point<2>(xh + 0.5*bigscale*delx,yl - 0.5*bigscale*dely));
		store_triangle(npts,npts+1,npts+2);

		// mix up the order of insertion
		for (auto j=npts; j>0; j--) std::swap(perm[j-1],perm[hashfn.int64(jran++) % j]);
		
		// insert points one-by-one
		for (auto j=0; j<npts; j++) insert_point(perm[j]);
		
		// Deactivate the huge root triangle and all of its connecting edges.
		for (auto j=0; j<ntree; j++) {
			if (triangles[j].state > 0) {
				if (triangles[j].vertices[0] >= npts || 
					triangles[j].vertices[1] >= npts ||
					triangles[j].vertices[2] >= npts) {
					triangles[j].state = -1;
					ntri--;
				}
			}
		}

		// Clean up, unless option bit says not to.
		if (!(opt & 1)) {
			perm.clear();
			trihash.clear();
			linehash.clear();
		}
	}

	//Add the point with index r incrementally to the Delaunay triangulation.
	void insert_point(Int r) {
		
		Int i,j,k,l,s,tno,ntask,d0,d1,d2;
		Ullong key;
		std::stack<Int> tasks, taski, taskj;
		//Stacks (3 vertices) for legalizing edges.

		//Find triangle containing point. Fuzz if it lies on an edge.
		for (j=0; j<3; j++) {
			
			tno = which_contains_point(points[r],1);

			if (tno >= 0) break; //The desired result: Point is OK


			points[r].x[0] += fuzz * delx * (hashfn.doub(jran++)-0.5);
			points[r].x[1] += fuzz * dely * (hashfn.doub(jran++)-0.5);
		}
		if (j == 3){
			std::cerr << "Delaunay: points degenerate even after fuzzing!" << std::endl;
			throw("points degenerate even after fuzzing");
		} 

		ntask = 0;
		i = triangles[tno].vertices[0]; 
		j = triangles[tno].vertices[1]; 
		k = triangles[tno].vertices[2];
		
		//The following line is relevant only when the indicated bit in opt is set. This feature is used
		//by the convex hull application and causes any points already known to be interior to the
		//convex hull to be omitted from the triangulation, saving time (but giving in an incomplete
		//triangulation).
		if (opt & 2 && i < npts && j < npts && k < npts) return;

		//Create three triangles and queue them for legal edge tests.
		d0 = store_triangle(r,i,j);
		tasks.push(r); taski.push(i); taskj.push(j);
		d1 = store_triangle(r,j,k);
		tasks.push(r); taski.push(j); taskj.push(k);
		d2 = store_triangle(r,k,i);
		tasks.push(r); taski.push(k); taskj.push(i);
		
		//Erase the old triangle.
		erase_triangle(i,j,k,d0,d1,d2);

		//Legalize edges recursively.
		while (tasks.size()>0) {
			
			s = tasks.top(); tasks.pop();
			i = taski.top(); taski.pop();
			j = taskj.top(); taskj.pop();

			//Look up fourth point
			key = hashfn.int64(j) - hashfn.int64(i);
			if (linehash.count(key) == 0) continue;
			l = linehash[key];

			if (in_circle(points[l],points[j],points[s],points[i]) > 0.0){ //Needs legalizing
				//Create two new triangles
				d0 = store_triangle(s,l,j);
				d1 = store_triangle(s,i,l);

				//and erase old ones.
				erase_triangle(s,i,j,d0,d1,-1);
				erase_triangle(l,j,i,d0,d1,-1);

				//Erase line in both directions.
				key = hashfn.int64(i)-hashfn.int64(j);
				linehash.erase(key);
				key = 0 - key;	//Unsigned, hence binary minus.
				linehash.erase(key);

				//Two new edges now need checking:
				tasks.push(s); taski.push(l); taskj.push(j);
				tasks.push(s); taski.push(i); taskj.push(l);
			}
		}
	}


	//Given point p, return index in triangles of the triangle in the triangulation that contains it, or
	//return -1 for failure. If strict is nonzero, require strict containment, otherwise allow the point
	//to lie on an edge.
	Int which_contains_point(const Point<2> &p, Int strict) {
		
		Int i,j,k=0;
		//Descend in tree until reach a “live” triangle.
		while (triangles[k].state <= 0) {
			
			//Check up to three daughters.
			for (i=0; i<3; i++) {

				//Daughter doesn’t exist.
				if ((j = triangles[k].daughters[i]) < 0) continue;
				
				if (strict) {
					if (triangles[j].contains_point(p) > 0) break;
				} 
				else {
					//Yes, descend on this branch.
					if (triangles[j].contains_point(p) >= 0) break;
				}
			}

			//No daughters contain the point.
			if (i == 3) return -1;
			
			//Set new mother.
			k = j;
			
		}
		//Normal return.
		return k;
	}


	//Erase triangle abc in trihash and inactivate it in triangles after setting its daughters.
	void erase_triangle(Int a, Int b, Int c, 
						Int d0, Int d1, Int d2) {
		Ullong key;
		Int j;
		key = hashfn.int64(a) ^ hashfn.int64(b) ^ hashfn.int64(c);

		if (trihash.count(key)==0) {
			std::cerr << "Delaunay: Cannot erase triangle... it doesnt exist" << std::endl;
			throw("nonexistent triangle");
		}
		j = trihash[key];

		trihash.erase(key);
		triangles[j].daughters[0] = d0; 
		triangles[j].daughters[1] = d1; 
		triangles[j].daughters[2] = d2;
		triangles[j].state = 0;
		ntri--;
	}


	//Store a triangle with vertices a, b, c in trihash. Store its points in linehash under keys to
	//opposite sides. Add it to triangles, returning its index there.
	Int store_triangle(Int a, Int b, Int c) {
		
		Ullong key;
		triangles[ntree].set(a,b,c,&points.front());
		key = hashfn.int64(a) ^ hashfn.int64(b) ^ hashfn.int64(c);
		trihash[key] = ntree;
		key = hashfn.int64(b)-hashfn.int64(c);
		linehash[key] = a;
		key = hashfn.int64(c)-hashfn.int64(a);
		linehash[key] = b;
		key = hashfn.int64(a)-hashfn.int64(b);
		linehash[key] = c;

		if (++ntree == ntreemax){
			std::cerr << "Delaunay: Cannot store more triangles... out of space!" << std::endl;
			throw("triangles is sized too small");
		} 
		ntri++;
		return (ntree-1);
	}
};

const Doub Delaunay::fuzz = 1.0e-6;
const Doub Delaunay::bigscale = 1000.0;
Uint Delaunay::jran = 14921620;



}


#endif