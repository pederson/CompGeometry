#ifndef _ORTHTREE_H
#define _ORTHTREE_H

#include <map>
#include <unordered_map>
#include <memory>

#include "GeomUtils.hpp"

// evaluate Val^P at compile time
template <std::size_t Val, std::size_t P>
struct Power {
    static constexpr const std::size_t value {Val*Power<Val,P-1>::value};
};

template <std::size_t Val>
struct Power<Val,0> {
    static constexpr const std::size_t value {1};
};




namespace csg{

template <	std::size_t dim,
			std::size_t rfactor,
			class ValueT>
class Orthtree{
public:

	Orthtree(){};

	// define some construction functions
	// build tree using an indicator function that outputs an int for a given point,
	// then use a map to a prototype Value to fill the tree
	// void buildTree(IndicatorFunction<Point<dim>> ifn, std::map<std::size_t, ValueT> proto, std::size_t nlevels);

	// expose iterators

protected:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	struct Node{
		std::shared_ptr<ValueT>	mVal;			// hold the value or pointer to value
		bool					mIsLeaf;		// if this node is a leaf. If not, it has children
	};

	// these map an integer LEVEL (starting from 0) to another map that maps a KEY to a NODE
	std::map<std::size_t, std::unordered_map<std::size_t, Node>> 		mLevelMaps; 		
	std::map<std::size_t, std::unordered_map<std::size_t, Node>>		mBdryMaps;


	// define some utility functions (can be specialized)
	std::size_t getParentKey(std::size_t key) const {return (key-1)/sSize;};
	std::size_t getLeftChildKey(std::size_t key) const {return key*sSize + 1;};
	std::size_t getLevel(std::size_t key) const {
		std::size_t lvl=0;
		while (key>0){
			lvl++;
			key = (key-1)/sSize;
		}
		return lvl;
	}
	std::size_t getPositionWithinParent(std::size_t key) const {return (key-1)%sSize;};
	iPoint<dim> getOffsetWithinParent(std::size_t key) const {
		iPoint<dim> off;
		for (auto i=0; i<dim; i++) off[i] = 0;
		for (auto i=0; i<dim; i++) {
			off[i] = key%rfactor; 
			key -= off[i];
			key /= rfactor;
		}
		return off;
	}
	iPoint<dim> getLevelOffset(std::size_t key) const {

		iPoint<dim> off;
		for (auto i=0; i<dim; i++) off[i] = 0;
		std::size_t mult = 1;
		std::size_t pkey;
		while (key>0) {
			// get key within parent
			pkey = getPositionWithinParent(key);
			// get the offset within parent
			off = off + mult*getOffsetWithinParent(pkey);
			// begin anew with parent key
			key = getParentKey(key);
			mult *= rfactor;
		}
		return off;
	}

	// random access
	// void insertValue(std::size_t key, ValueT v);
	// void removeValue(std::size_t key);
};	

}

#endif