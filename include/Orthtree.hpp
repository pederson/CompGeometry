#ifndef _ORTHTREE_H
#define _ORTHTREE_H

#include <map>
#include <unordered_map>
#include <memory>

#include "GeomUtils.hpp"


namespace csg{




// evaluate Val^P at compile time (template metaprogram)
template <std::size_t Val, std::size_t P>
struct Power {
    static constexpr const std::size_t value {Val*Power<Val,P-1>::value};
};

template <std::size_t Val>
struct Power<Val,0> {
    static constexpr const std::size_t value {1};
};




template <	std::size_t dim,
			std::size_t rfactor,
			class ValueT>
class Orthtree{
public:

	struct Node{
		std::shared_ptr<ValueT>	mVal;			// hold the value or pointer to value
		bool					mIsLeaf;		// if this node is a leaf. If not, it has children
	};


	Orthtree(){
		std::size_t k = 17;
		std::cout << Power<3, 3>::value << std::endl;
		std::cout << "Ref: " << rfactor << " Dim: " << dim << std::endl;
		std::cout << "k: " << k << std::endl;
		std::cout << "Parent(k): " << getParentKey(k) << std::endl;
		std::cout << "LeftChild(k): " << getLeftChildKey(k) << std::endl;
		std::cout << "Level(k): " << getLevel(k) << std::endl;
		std::cout << "PosWithinParent(k): " << getPositionWithinParent(k) << std::endl;
		std::cout << "OffsetWithinParent(k): " << getOffsetWithinParent(k) << std::endl;
		std::cout << "LevelOffset(k): " << getLevelOffset(k) << std::endl;
		std::cout << "LevelStartingKey(2): " << getLevelStartingKey(2) << std::endl;
		std::cout << "KeyFromLevelOffset(2, (1,2)): " << getKeyFromLevelOffset(2, IntPoint2(1,2)) << std::endl;
		std::cout << "NeighborKeyMin(k, 0): " << getNeighborKeyMin(k, 0) << std::endl;
		std::cout << "NeighborKeyMin(k, 1): " << getNeighborKeyMin(k, 1) << std::endl;
		std::cout << "NeighborKeyMax(k, 0): " << getNeighborKeyMax(k, 0) << std::endl;
		std::cout << "NeighborKeyMax(k, 1): " << getNeighborKeyMax(k, 1) << std::endl;
		std::cout << std::endl; 
	};

	// define some construction functions

	// build tree using an indicator function that outputs an int for a given point,
	// then use a map to a prototype Value to fill the tree
	template <class PrototypeMap, class RefineOracle>
	void buildTree(std::size_t lvlmax, Box<dim> boundbox, 
				  const PrototypeMap & pm, const RefineOracle & ro,
				  std::size_t key=0){

		// std::cout << "key: " << key << "  box: " << boundbox << std::endl;
		// boundbox.print_summary(); 

		// create node for key
		std::size_t lvl = getLevel(key);
		Node n; n.mIsLeaf = true; 
		n.mVal = std::make_shared<ValueT>(pm.getValue(boundbox));
		mLevelMaps[lvl][key] = n;
		// std::cout << "checking refinement conditions" << std::endl;

		// decide if refinement is necessary
		if (lvl == lvlmax) return;
		if (ro.isUniform(boundbox)) return;
		mLevelMaps[lvl][key].mIsLeaf = false;

		// std::cout << "got here" << std::endl;
		// if refining, 
		Point<dim> boxsize = 1.0/static_cast<double>(rfactor)*(boundbox.hi-boundbox.lo);
		std::size_t kl = getLeftChildKey(key);
		for (auto kc=kl; kc<kl+sSize; kc++){
			IntPoint<dim> off = getOffsetWithinParent(kc);
			// std::cout << "kc: " << kc << " offsetwithinparent: " << off << std::endl;
			Point<dim> newlo = boundbox.lo+boxsize*off;
			Box<dim> rbox(newlo, newlo+boxsize);
			buildTree(lvlmax, rbox, pm, ro, kc);
		}
	}

	// random access

	// set a value to a pre-existing key
	void setCellValue(std::size_t key, const ValueT & v){
		// find level
		std::size_t lvl = getLevel(key);
		// insert node value
		mLevelMaps[lvl].at(key).mVal = std::shared_ptr<ValueT>(v);
	}

	// split parent key, and endow all children
	// with a copy of the parent value
	void refineCell(std::size_t key) {
		std::size_t lvl = getLevel(key);
		if (!mLevelMaps[lvl][key].mIsLeaf){
			std::cerr << "Orthtree: Cell is already refined!" << std::endl;
			throw -1;
		}
		std::size_t kl = getLeftChildKey(key);
		Node n = mLevelMaps[lvl].at(key);
		for (auto kc=kl; kc<kl+dim*rfactor; kc++){
			mLevelMaps[lvl+1][kc] = n;
		}
		mLevelMaps[lvl][key].mIsLeaf = false;
	}

	// delete all child cells of a given parent key
	void pruneChildren(std::size_t key) {
		std::size_t lvl = getLevel(key);
		if (mLevelMaps[lvl][key].mIsLeaf){
			std::cerr << "Orthtree: Cell is already a leaf!" << std::endl;
			throw -1;
		}
		std::size_t kl = getLeftChildKey(key);
		for (auto kc=kl; kc<kl+dim*rfactor; kc++){
			if (!mLevelMaps[lvl+1][kc].mIsLeaf) pruneChildren(kc);
			mLevelMaps[lvl+1].remove(kc);
		}
		mLevelMaps[lvl][key].mIsLeaf = true;
	}

	void print_summary(std::ostream & os = std::cout) const {
		for (auto lit=mLevelMaps.begin(); lit!=mLevelMaps.end(); lit++){
			os << "Level (" << lit->first << ") --> " << lit->second.size() << std::endl;
		}
	}

	// expose iterators
	friend class leaf_iterator;
	// iterate over key/node pairs for leaf nodes only 
	class leaf_iterator{
	public:
		typedef leaf_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const std::size_t, Node> value_type;
	    typedef std::pair<const std::size_t, Node> & reference;
	    typedef std::pair<const std::size_t, Node> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		leaf_iterator(Orthtree & t)
		: tree(t)
		, level(0)
		, lit(t.mLevelMaps.begin())
		, it((t.mLevelMaps.begin())->second.begin()){
			it = tree.mLevelMaps.begin()->second.begin();
			if (! it->second.mIsLeaf){
				this->operator++();
			}
		}

		leaf_iterator(Orthtree & t, typename std::unordered_map<std::size_t, Node>::iterator iter)
		: tree(t)
		, level(0)
		, lit(t.mLevelMaps.begin())
		, it(iter) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			while (lit != tree.mLevelMaps.end()){
				while (it != lit->second.end()){
					if (it->second.mIsLeaf) return *this;
					it++;				
				}
				// reached end of level
				lit++;
				it = lit->second.begin();
			}

			// have reached the end of all the cells
			return *this;
		}

		// preincrement 
		self_type operator++(int blah){
			it++;
			while (lit != tree.mLevelMaps.end()){
				while (it != lit->second.end()){
					if (it->second.mIsLeaf) return *this;
					it++;				
				}
				// reached end of level
				lit++;
				it = lit->second.begin();
			}

			// have reached the end of all the cells
			return *this;
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		std::size_t level;
		typename std::map<std::size_t, std::unordered_map<std::size_t, Node>>::iterator lit;
		typename std::unordered_map<std::size_t, Node>::iterator it;
		Orthtree & tree;
	};

	
	leaf_iterator leaf_begin(){return leaf_iterator(*this);};
	leaf_iterator leaf_end(){auto p=mLevelMaps.end(); p--; return leaf_iterator(*this, p->second.end());};



	friend class level_iterator;
	// iterate over key/node pairs for leaf nodes only 
	class level_iterator{
	public:
		typedef level_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const std::size_t, Node> value_type;
	    typedef std::pair<const std::size_t, Node> & reference;
	    typedef std::pair<const std::size_t, Node> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		level_iterator(Orthtree & t, std::size_t lvl)
		: tree(t)
		, it(t.mLevelMaps[lvl].begin()){};

		level_iterator(Orthtree & t, std::size_t lvl, typename std::unordered_map<std::size_t, Node>::iterator iter)
		: tree(t)
		, it(iter){};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			// have reached the end of level
			return *this;
		}

		// preincrement 
		self_type operator++(int blah){
			it++;
			// have reached the end of level
			return *this;
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename std::unordered_map<std::size_t, Node>::iterator it;
		Orthtree & tree;
	};

	
	level_iterator level_begin(std::size_t lvl){return level_iterator(*this,lvl);};
	level_iterator level_end(std::size_t lvl){return level_iterator(*this, lvl, mLevelMaps[lvl].end());};


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
	
	IntPoint<dim> getOffsetWithinParent(std::size_t key) const {
		IntPoint<dim> off;
		key = getPositionWithinParent(key);
		for (auto i=0; i<dim; i++) off.x[i] = 0;
		for (auto i=0; i<dim; i++) {
			off.x[i] = key%rfactor; 
			key -= off.x[i];
			key /= rfactor;
		}
		return off;
	}
	
	IntPoint<dim> getLevelOffset(std::size_t key) const {

		IntPoint<dim> off;
		for (auto i=0; i<dim; i++) off.x[i] = 0;
		std::size_t mult = 1;
		std::size_t pkey;
		while (key>0) {
			// get the offset within parent
			off = off + getOffsetWithinParent(key)*mult;
			// begin anew with parent key
			key = getParentKey(key);
			mult *= rfactor;
		}
		return off;
	}

	// get the key that starts a given level
	std::size_t getLevelStartingKey(std::size_t lvl) const {
		std::size_t keystart = 1;	// starting at level 1
		std::size_t lct = 1;
		for (auto l=1; l<lvl; l++){
			lct *= sSize;
			keystart += lct;
		}
		return keystart;
	}

	// get a key from a offset on a given level
	std::size_t getKeyFromLevelOffset(std::size_t lvl, IntPoint<dim> off) const {
		// get starting key for the level
		std::size_t keystart = getLevelStartingKey(lvl);

		std::size_t ct = 1;
		std::size_t tot =0;
		IntPoint<dim> mult;
		IntPoint<dim> dotter;
		std::size_t rval = 1;
		for (auto i=0; i<dim; i++){
			mult.x[i] = rval;
			rval *= rfactor;
		}
		
		dotter = off%rfactor;
		tot += ct*IntPoint<dim>::dot(mult, dotter);

		// std::cout << "mult: " << mult << std::endl;
		// std::cout << "off: " << off << std::endl;
		// std::cout << "tot: " << tot << std::endl;

		for (auto l=lvl-1; l>0; l--){
			// increase ct
			ct *= sSize;
			// divide by rfactor
			off = off/static_cast<int>(rfactor);
			// take modulus
			dotter = off%rfactor;
			// accumulate dot products
			tot += ct*IntPoint<dim>::dot(mult, dotter);

			// std::cout << "l: " << l << std::endl;
			// std::cout << "off: " << off << std::endl;
			// std::cout << "dotter: " << dotter << std::endl;
			// std::cout << "tot: " << tot << std::endl;
		}

		// add the start key and return;
		return keystart + tot;
	}

	// get the neighboring key to 'key' on the minimum side
	// along a dimension specified by 'd'
	std::size_t getNeighborKeyMin(std::size_t key, std::size_t d) const {
		// get the level of this key
		std::size_t lvl = getLevel(key);
		// get the level offset of this key
		IntPoint<dim> loff = getLevelOffset(key);
		// subtract 1 in the chosen dimension
		loff.x[d]--;
		// get the key from level offset
		std::size_t neighbor = getKeyFromLevelOffset(lvl, loff);
		return neighbor;
	}

	// get the neighboring key to 'key' on the maximum side
	// along a dimension specified by 'd'
	std::size_t getNeighborKeyMax(std::size_t key, std::size_t d) const {
		// get the level of this key
		std::size_t lvl = getLevel(key);
		// get the level offset of this key
		IntPoint<dim> loff = getLevelOffset(key);
		// add 1 in the chosen dimension
		loff.x[d]++;
		// get the key from level offset
		std::size_t neighbor = getKeyFromLevelOffset(lvl, loff);
		return neighbor;
	}

	// set an existing key in the level map as a boundary.
	// this will add the key to the boundary key map
	void setKeyAsBoundary(std::size_t key) {
		std::size_t lvl = getLevel(key);
		mBdryMaps[lvl][key] = &mLevelMaps[lvl][key];
	}

	// build out a level boundary by going through all the points
	// and adding boundary cells to the left/right if the neighbors
	// on a given side don't exist
	void buildLevelBoundary(std::size_t lvl) {
		auto lit = level_iterator(lvl);
	}

	

protected:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	// these map an integer LEVEL (starting from 0) to another map that maps a KEY to a NODE
	std::map<std::size_t, std::unordered_map<std::size_t, Node>> 		mLevelMaps; 		
	std::map<std::size_t, std::unordered_map<std::size_t, Node *>>		mBdryMaps;
};




}

#endif