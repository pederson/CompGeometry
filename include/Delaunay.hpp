#ifndef _DELAUNAY_H
#define _DELAUNAY_H

#include "GeomUtils.hpp"

namespace csg{

struct RandomHash{
	inline unsigned long int int64(unsigned long int u){
		unsigned long int v = u * 3935559000370003845LL + 2691343689449507681LL;
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





template<class keyT, class hfnT> struct Hashtable {
	//Instantiate a hash table, with methods for maintaining a one-to-one correspondence between
	//arbitrary keys and unique integers in a specified range.
	Int nhash, nmax, nn, ng;
	VecInt htable, next, garbg;
	VecUllong thehash;
	hfnT hash;
	//An instance of a hash function object.
	Hashtable(Int nh, Int nv);
	//Constructor. Arguments are size of hash table and max number of stored elements (keys).
	Int	iget(const keyT &key);
	Int iset(const keyT &key);
	Int ierase(const keyT &key);
	Int ireserve();
	Int irelinquish(Int k);
	//Return integer for a previously set key.
	//Return unique integer for a new key.
	//Erase a key.
	//Reserve an integer (with no key).
	//Un-reserve an integer.
};



template<class keyT, class elT, class hfnT>
struct Hash : Hashtable<keyT, hfnT> {
	//Extend the Hashtable class with storage for elements of type elT, and provide methods for
	//storing, retrieving. and erasing elements. key is passed by address in all methods.
	using Hashtable<keyT,hfnT>::iget;
	using Hashtable<keyT,hfnT>::iset;
	using Hashtable<keyT,hfnT>::ierase;
	vector<elT> els;


	Hash(Int nh, Int nm) : Hashtable<keyT, hfnT>(nh, nm), els(nm) {}
	//Same constructor syntax as Hashtable.

	void set(const keyT &key, const elT &el)
	//Store an element el.
	{els[iset(key)] = el;}

	Int get(const keyT &key, elT &el) {
	///Retrieve an element into el. Returns 0 if no element is stored under key, or 1 for success.
	Int ll = iget(key);
	if (ll < 0) return 0;
	el = els[ll];
	return 1;
	}

	elT& operator[] (const keyT &key) {
	//Store or retrieve an element using subscript notation for its key. Returns a reference that
	//can be used as an l-value.
	Int ll = iget(key);
	if (ll < 0) {
	ll = iset(key);
	els[ll] = elT();
	}
	return els[ll];
	}

	Int count(const keyT &key) {
	//Return the number of elements stored under key, that is, either 0 or 1.
	Int ll = iget(key);
	return (ll < 0 ? 0 : 1);
	}

	Int erase(const keyT &key) {
	//Erase an element. Returns 1 for success, or 0 if no element is stored under key.
	return (ierase(key) < 0 ? 0 : 1);
	}
};




struct TriElem{
	std::vector<Point<2>> & 	points;	// the vector of points that this references
	iPoint<3> 					vertices;	// the 3 vertices in this element
	iPoint<3> 					daughters;	// locations of up to 3 daughters
	bool 						state;		// true if the element is a leaf

	TriElem() {};
	TriElem(iPoint<3> p, std::vector<Point<2>> & pts)
	: points(pts), vertices(p), daughters({0,0,0}), state(true) {};

	int contains_points(const Point<2> & pt){
		double d;
		bool ztest = false;
		unsigned int i,j;
		for (auto i=0; i<3; i++){
			j = (i+1)%3;
			d = (points[vertices.x[j]].x[0] - points[vertices.x[i]].x[0])
				*(pt.x[1]-points[vertices.x[i]].x[1])
				- (points[vertices.x[j]].x[1] - points[vertices.x[i]].x[1])
				*(pt.x[0]-points[vertices.x[i]].x[0]);
			if (d<0.0) return -1;
			if (d == 0.0) ztest = true;
		}
		return (ztest? 0 : 1);
	}
};

struct Delaunay {
	unsigned int npts, ntri, ntree, ntreemax, opt;
	double delx, dely;
	std::vector<Point<2>> points;
	vector<TriElem> triangles;
};



}
#endif