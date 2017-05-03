#ifndef _ORTHTREE_H
#define _ORTHTREE_H

#include <map>
#include <unordered_map>
#include <memory>
#include <list>
#include <array>
#include <utility>

#include "GeomUtils.hpp"


namespace csg{

// evaluate Val^P at compile time (template metaprogram)
template <std::size_t Val, std::size_t P>
struct Power {
    static constexpr const std::size_t value {Val*Power<Val,P-1>::value};
};

// base case of zero power
template <std::size_t Val>
struct Power<Val,0> {
    static constexpr const std::size_t value {1};
};

// evaluate the number of cells on a given level at compile time
template <std::size_t dim,
		  std::size_t rfactor,
		  std::size_t lvl>
struct NumCellsOnLevel{
	static constexpr const std::size_t value {Power<Power<rfactor,lvl>::value,dim>::value};
};

template <std::size_t dim,
		  std::size_t rfactor>
struct NumCellsOnLevel<dim,rfactor,0>{
	static constexpr const std::size_t value {1};
};

// evaluate the starting key of a given
template <std::size_t dim,
		  std::size_t rfactor,
		  std::size_t lvl>
struct LevelStartingKey{
	static constexpr const std::size_t value {NumCellsOnLevel<dim,rfactor,lvl-1>::value+LevelStartingKey<dim,rfactor,lvl-1>::value};
};

// base case of the zeroth level
template <std::size_t dim,
		  std::size_t rfactor>
struct LevelStartingKey<dim,rfactor,0>{
	static constexpr const std::size_t value {0};
};


template<std::size_t dim, 
		 std::size_t rfactor,
		 std::size_t lvl,
		 std::size_t... I>
constexpr std::array<std::size_t, lvl> buildStartingKeysArray(std::integer_sequence<std::size_t, I...>){
	return std::array<std::size_t, lvl>{{LevelStartingKey<dim,rfactor,I>::value...}};
}

template<std::size_t dim, 
		 std::size_t rfactor,
		 std::size_t lvl>
constexpr std::array<std::size_t, lvl> createStartingKeys(){
	return buildStartingKeysArray<dim,rfactor,lvl>(std::make_index_sequence<lvl>{});
}


template<std::size_t dim, 
		 std::size_t rfactor,
		 std::size_t lvl,
		 std::size_t... I>
constexpr std::array<std::size_t, lvl> buildEndingKeysArray(std::integer_sequence<std::size_t, I...>){
	return std::array<std::size_t, lvl>{{(LevelStartingKey<dim,rfactor,I>::value+NumCellsOnLevel<dim,rfactor,I>::value-1)...}};
}

template<std::size_t dim, 
		 std::size_t rfactor,
		 std::size_t lvl>
constexpr std::array<std::size_t, lvl> createEndingKeys(){
	return buildEndingKeysArray<dim,rfactor,lvl>(std::make_index_sequence<lvl>{});
}


template<std::size_t dim,
		 std::size_t rfactor,
		 class KeyT>
class BruteForceLevelDecoder{
public:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	static std::size_t decode(KeyT key) {
		std::size_t lvl=0;
		while (key>0){
			lvl++;
			key = (key-1)/sSize;
		};
		return lvl;
	};
};


template<std::size_t dim,
		 std::size_t rfactor,
		 class KeyT>
class BruteForceParentDecoder{
public:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	static std::size_t decode(KeyT key) {
		return (key-1)/sSize;
	};
};


template<std::size_t dim,
		 std::size_t rfactor,
		 class KeyT>
class BruteForceLeftChildDecoder{
public:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	static std::size_t decode(KeyT key) {
		return key*sSize + 1;
	};
};


// template<std::size_t dim,
// 		 std::size_t rfactor,
// 		 class KeyT>
// class BruteForceNeighborDecoder{
// public:

// 	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

// 	static std::size_t getNeighborMax(KeyT key, std::size_t d){

// 	};

// 	// get the neighboring key to 'key' on the maximum side
// 	// along a dimension specified by 'd'
// 	std::size_t getNeighborKeyMax(std::size_t key, std::size_t d) const {
// 		// get the level of this key
// 		std::size_t lvl = getLevel(key);
// 		// get the level offset of this key
// 		IntPoint<dim> loff = getLevelOffset(key);
// 		// add 1 in the chosen dimension
// 		loff.x[d]++;
// 		// get the key from level offset
// 		std::size_t neighbor = getKeyFromLevelOffset(lvl, loff);
// 		return neighbor;
// 	}

// 	static std::size_t getNeighborMin(KeyT key, std::size_t d){
		
// 	};
// };


template<class KeyT>
class NullSubdomainDecoder{
public:
	static std::size_t decode(KeyT key) {return 0;};
};





// dim 			- the dimension of the tree (2D, 3D, etc)
// rfactor		- the refinement factor between levels in the tree
// ValueT 		- the value stored in the tree nodes
// KeyT 		- the type associated with keys
// LevelDecoder - a functor, returns the level. std::size_t = operator()(KeyT)
// SubdomainDecoder 	- a functor, returns the subdomain. std::size_t = operator()(KeyT)
template <	std::size_t dim,
			std::size_t rfactor,
			class ValueT,
			class KeyT = std::size_t,
			class LevelDecoder = BruteForceLevelDecoder<dim,rfactor,KeyT>,
			class ParentDecoder = BruteForceParentDecoder<dim,rfactor,KeyT>,
			class LeftChildDecoder = BruteForceLeftChildDecoder<dim,rfactor,KeyT>,
			// class NeighborDecoder = BruteForceNeighborDecoder<dim,rfactor,KeyT>,
			class SubdomainDecoder = NullSubdomainDecoder<KeyT>,
			std::size_t lvlmax = 16>
class Orthtree{
public:

	struct Node{
		ValueT					mVal;			// hold the value
		bool					mIsLeaf;		// if this node is a leaf. If not, it has children
	};


protected:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	// these map an integer LEVEL (starting from 0) to another map
	// that maps an integer SUBDOMAIN to another map
	// that maps a KEY to a NODE
	std::map<std::size_t, std::map<std::size_t, std::unordered_map<KeyT, Node>>> 		mKeyMaps; 		
	
	// some convenience values created at compile time
	std::array<std::size_t, lvlmax+1>			mLvlStartKeys;//={createStartingKeys<dim,rfactor,lvlmax>()};
	std::array<std::size_t, lvlmax+1>			mLvlEndKeys;//={createStartingKeys<dim,rfactor,lvlmax>()};

	// these map an integer LEVEL (starting from 0) to another map that maps a 
	// SUBDOMAIN (any integer) to a list of keys contained in the subdomain
	// std::map<std::size_t, std::unordered_map<std::size_t, std::unordered_map<std::size_t, Node *>>>		mSubDomainMaps;


public:


	Orthtree()
	: mLvlStartKeys(createStartingKeys<dim,rfactor,lvlmax+1>())
	, mLvlEndKeys(createEndingKeys<dim,rfactor,lvlmax+1>())
	{
		KeyT k = 17;
		std::cout << Power<3, 3>::value << std::endl;
		std::cout << "Ref: " << rfactor << " Dim: " << dim << std::endl;
		std::cout << "k: " << k << std::endl;
		std::cout << "Parent(k): " << getParentKey(k) << std::endl;
		std::cout << "LeftChild(k): " << getLeftChildKey(k) << std::endl;
		std::cout << "Level(k): " << getLevel(k) << std::endl;
		// std::cout << "PosWithinParent(k): " << getPositionWithinParent(k) << std::endl;
		// std::cout << "OffsetWithinParent(k): " << getOffsetWithinParent(k) << std::endl;
		// std::cout << "LevelOffset(k): " << getLevelOffset(k) << std::endl;
		// for (auto i=0; i<=lvlmax; i++) std::cout << "LevelStartingKey(" << i << "): " << lkeys[i] << std::endl;
		for (auto i=0; i<=lvlmax; i++) std::cout << "LevelStartingKey(" << i << "): " << mLvlStartKeys[i] << std::endl;
		for (auto i=0; i<=lvlmax; i++) std::cout << "LevelEndingKey(" << i << "): " << mLvlEndKeys[i] << std::endl;
		// std::cout << "KeyFromLevelOffset(2, (1,2)): " << getKeyFromLevelOffset(2, IntPoint2(1,2)) << std::endl;
		// std::cout << "NeighborKeyMin(k, 0): " << getNeighborKeyMin(k, 0) << std::endl;
		// std::cout << "NeighborKeyMin(k, 1): " << getNeighborKeyMin(k, 1) << std::endl;
		// std::cout << "NeighborKeyMax(k, 0): " << getNeighborKeyMax(k, 0) << std::endl;
		// std::cout << "NeighborKeyMax(k, 1): " << getNeighborKeyMax(k, 1) << std::endl;
		std::cout << std::endl; 
	};


	// define some construction functions

	// // build tree using an indicator function that outputs an int for a given point,
	// // then use a map to a prototype Value to fill the tree
	// template <class PrototypeMap, class RefineOracle>
	// void buildTree(std::size_t lvlmax, Box<dim> boundbox, 
	// 			  const PrototypeMap & pm, const RefineOracle & ro,
	// 			  std::size_t key=0){

	// 	// create node for key
	// 	std::size_t lvl = getLevel(key);
	// 	Node n; n.mIsLeaf = true; 
	// 	n.mVal = std::make_shared<ValueT>(pm.getValue(boundbox));
	// 	mLevelMaps[lvl][key] = n;

	// 	// decide if refinement is necessary
	// 	if (lvl == lvlmax) return;
	// 	if (ro.isUniform(boundbox)) return;
	// 	mLevelMaps[lvl][key].mIsLeaf = false;

	// 	// if refining, 
	// 	Point<dim> boxsize = 1.0/static_cast<double>(rfactor)*(boundbox.hi-boundbox.lo);
	// 	std::size_t kl = getLeftChildKey(key);
	// 	for (auto kc=kl; kc<kl+sSize; kc++){
	// 		IntPoint<dim> off = getOffsetWithinParent(kc);
	// 		Point<dim> newlo = boundbox.lo+boxsize*off;
	// 		Box<dim> rbox(newlo, newlo+boxsize);
	// 		buildTree(lvlmax, rbox, pm, ro, kc);
	// 	}
	// }

	// random access

	// // set a value to a pre-existing key
	// void setCellValue(std::size_t key, const ValueT & v){
	// 	// find level
	// 	std::size_t lvl = getLevel(key);
	// 	// insert node value
	// 	mLevelMaps[lvl].at(key).mVal = std::shared_ptr<ValueT>(v);
	// }


	// Node * getCell(std::size_t key, std::size_t lvl){
	// 	return &mLevelMaps[lvl][key];
	// }

	// // split parent key, and endow all children
	// // with a copy of the parent value
	// void refineCell(std::size_t key) {
	// 	std::size_t lvl = getLevel(key);
	// 	if (!mLevelMaps[lvl][key].mIsLeaf){
	// 		std::cerr << "Orthtree: Cell is already refined!" << std::endl;
	// 		throw -1;
	// 	}
	// 	std::size_t kl = getLeftChildKey(key);
	// 	Node n = mLevelMaps[lvl].at(key);
	// 	for (auto kc=kl; kc<kl+dim*rfactor; kc++){
	// 		mLevelMaps[lvl+1][kc] = n;
	// 	}
	// 	mLevelMaps[lvl][key].mIsLeaf = false;
	// }

	// // delete all child cells of a given parent key
	// void pruneChildren(std::size_t key) {
	// 	std::size_t lvl = getLevel(key);
	// 	if (mLevelMaps[lvl][key].mIsLeaf){
	// 		std::cerr << "Orthtree: Cell is already a leaf!" << std::endl;
	// 		throw -1;
	// 	}
	// 	std::size_t kl = getLeftChildKey(key);
	// 	for (auto kc=kl; kc<kl+dim*rfactor; kc++){
	// 		if (!mLevelMaps[lvl+1][kc].mIsLeaf) pruneChildren(kc);
	// 		mLevelMaps[lvl+1].remove(kc);
	// 	}
	// 	mLevelMaps[lvl][key].mIsLeaf = true;
	// }

	// void print_summary(std::ostream & os = std::cout) const {
	// 	using comptype = std::pair<const std::size_t, Node>;
	// 	for (auto lit=mLevelMaps.begin(); lit!=mLevelMaps.end(); lit++){
	// 		auto minmax = std::minmax_element(lit->second.begin(), lit->second.end(), [](comptype p1, comptype p2)->bool{return p1.first < p2.first;});
	// 		os << "Level (" << lit->first << ") --> " << lit->second.size() << " bdry(" << (mBdryMaps.find(lit->first) == mBdryMaps.end()? 0 : mBdryMaps.at(lit->first).size()) << ")" << "\t\tmin: " << minmax.first->first << " max: " << minmax.second->first  << std::endl;
	// 	}
	// }

	// // expose iterators
	// friend class leaf_iterator;
	// // iterate over key/node pairs for leaf nodes only 
	// class leaf_iterator{
	// public:
	// 	typedef leaf_iterator self_type;
	// 	typedef std::ptrdiff_t difference_type;
	//     typedef std::pair<const std::size_t, Node> value_type;
	//     typedef std::pair<const std::size_t, Node> & reference;
	//     typedef std::pair<const std::size_t, Node> * pointer;
	//     typedef std::forward_iterator_tag iterator_category;

	// 	// construction
	// 	leaf_iterator(Orthtree & t)
	// 	: tree(t)
	// 	, level(0)
	// 	, lit(t.mLevelMaps.begin())
	// 	, it((t.mLevelMaps.begin())->second.begin()){
	// 		it = tree.mLevelMaps.begin()->second.begin();
	// 		if (! it->second.mIsLeaf){
	// 			this->operator++();
	// 		}
	// 	}

	// 	leaf_iterator(Orthtree & t, typename std::unordered_map<std::size_t, Node>::iterator iter)
	// 	: tree(t)
	// 	, level(0)
	// 	, lit(t.mLevelMaps.begin())
	// 	, it(iter) {};

	// 	// dereferencing
	// 	reference operator*(){ return *it;};

	// 	// preincrement 
	// 	self_type operator++(){
	// 		it++;
	// 		while (lit != tree.mLevelMaps.end()){
	// 			while (it != lit->second.end()){
	// 				if (it->second.mIsLeaf) return *this;
	// 				it++;				
	// 			}
	// 			// reached end of level
	// 			lit++;

	// 			if (lit == tree.mLevelMaps.end()) break;
	// 			it = lit->second.begin();
	// 		}
	// 		// have reached the end of all the cells
	// 		return tree.leaf_end();
	// 	}

	// 	// postincrement 
	// 	self_type operator++(int blah){
	// 		it++;
	// 		while (lit != tree.mLevelMaps.end()){
	// 			while (it != lit->second.end()){
	// 				if (it->second.mIsLeaf) return *this;
	// 				it++;				
	// 			}
	// 			// reached end of level
	// 			lit++;

	// 			if (lit == tree.mLevelMaps.end()) break;
	// 			it = lit->second.begin();
	// 		}

	// 		// have reached the end of all the cells
	// 		return tree.leaf_end();
	// 	}

	// 	// pointer
	// 	pointer operator->() {return it.operator->();};

	// 	// inequality
	// 	bool operator!=(const self_type & leaf) const {return it != leaf.it;};

	// 	// equality
	// 	bool operator==(const self_type & leaf) const {return it == leaf.it;};


	// private:
	// 	std::size_t level;
	// 	typename std::map<std::size_t, std::unordered_map<std::size_t, Node>>::iterator lit;
	// 	typename std::unordered_map<std::size_t, Node>::iterator it;
	// 	Orthtree & tree;
	// };

	
	// leaf_iterator leaf_begin(){return leaf_iterator(*this);};
	// leaf_iterator leaf_end(){auto p=mLevelMaps.end(); p--; return leaf_iterator(*this, p->second.end());};




	// template<std::size_t lvl> friend class level_iterator;
	// // iterate over key/node pairs for leaf nodes only 
	// template<std::size_t lvl> 
	// class level_iterator{
	// public:
	// 	typedef level_iterator self_type;
	// 	typedef std::ptrdiff_t difference_type;
	//     typedef std::pair<const std::size_t, Node> value_type;
	//     typedef std::pair<const std::size_t, Node> & reference;
	//     typedef std::pair<const std::size_t, Node> * pointer;
	//     typedef std::forward_iterator_tag iterator_category;

	// 	// construction
	// 	level_iterator(Orthtree & t)
	// 	: tree(t)
	// 	, it(t.mLevelMaps[lvl].begin()){};

	// 	level_iterator(Orthtree & t, typename std::unordered_map<std::size_t, Node>::iterator iter)
	// 	: tree(t)
	// 	, it(iter){};

	// 	// dereferencing
	// 	reference operator*(){ return *it;};

	// 	// preincrement 
	// 	self_type operator++(){
	// 		it++;
	// 		// have reached the end of level
	// 		return *this;
	// 	}

	// 	// preincrement 
	// 	self_type operator++(int blah){
	// 		it++;
	// 		// have reached the end of level
	// 		return *this;
	// 	}

	// 	// pointer
	// 	pointer operator->() {return it.operator->();};

	// 	// inequality
	// 	bool operator!=(const self_type & leaf) const {return it != leaf.it;};

	// 	// equality
	// 	bool operator==(const self_type & leaf) const {return it == leaf.it;};


	// private:
	// 	typename std::unordered_map<std::size_t, Node>::iterator it;
	// 	Orthtree & tree;
	// };

	// template<std::size_t lvl>
	// level_iterator<lvl> level_begin(){return level_iterator<lvl>(*this);};
	// template<std::size_t lvl>
	// level_iterator<lvl> level_end(){return level_iterator<lvl>(*this, mLevelMaps[lvl].end());};



	// template<std::size_t lvl> friend class boundary_iterator;
	// // iterate over key/node pairs for leaf nodes only 
	// template<std::size_t lvl>
	// class boundary_iterator{
	// public:
	// 	typedef boundary_iterator self_type;
	// 	typedef std::ptrdiff_t difference_type;
	//     typedef std::pair<const std::size_t, Node> value_type;
	//     typedef std::pair<const std::size_t, Node> & reference;
	//     typedef std::pair<const std::size_t, Node> * pointer;
	//     typedef std::forward_iterator_tag iterator_category;

	// 	// construction
	// 	boundary_iterator(Orthtree & t)
	// 	: tree(t)
	// 	, bit(t.mBdryMaps[lvl].begin()){
	// 		if (bit == tree.mBdryMaps[lvl].end()) bit = tree.mBdryMaps[lvl].end();
	// 		else it = tree.mLevelMaps[lvl].find(bit->first);
	// 	};

	// 	boundary_iterator(Orthtree & t, typename std::unordered_map<std::size_t, Node *>::iterator iter)
	// 	: tree(t)
	// 	, bit(iter){
	// 		if (bit == tree.mBdryMaps[lvl].end()) bit = tree.mBdryMaps[lvl].end();
	// 		else it = tree.mLevelMaps[lvl].find(bit->first);
	// 	};

	// 	// dereferencing
	// 	reference operator*(){ return *it;};

	// 	// preincrement 
	// 	self_type operator++(){
	// 		bit++;
	// 		if (bit == tree.mBdryMaps[lvl].end()) return tree.boundary_end<lvl>();

	// 		it = tree.mLevelMaps[lvl].find(bit->first);
	// 		return *this;
	// 	}

	// 	// preincrement 
	// 	self_type operator++(int blah){
	// 		bit++;
	// 		if (bit == tree.mBdryMaps[lvl].end()) return tree.boundary_end<lvl>();
	// 		it = tree.mLevelMaps[lvl].find(bit->first);
	// 		return *this;
	// 	}

	// 	// pointer
	// 	pointer operator->() {return it.operator->();};

	// 	// inequality
	// 	bool operator!=(const self_type & leaf) const {return bit != leaf.bit;};

	// 	// equality
	// 	bool operator==(const self_type & leaf) const {return bit == leaf.bit;};


	// private:
	// 	typename std::unordered_map<std::size_t, Node>::iterator it;
	// 	typename std::unordered_map<std::size_t, Node *>::iterator bit;
	// 	Orthtree & tree;
	// };

	// template<std::size_t lvl>
	// boundary_iterator<lvl> boundary_begin(){return boundary_iterator<lvl>(*this);};
	// template<std::size_t lvl>
	// boundary_iterator<lvl> boundary_end(){return boundary_iterator<lvl>(*this, mBdryMaps[lvl].end());};




	// struct node_stencil{
	// 	std::pair<const std::size_t, Node>		*center;
	// 	std::array<std::pair<std::size_t, Node *>, dim> 				neighb_hi;
	// 	std::array<std::pair<std::size_t, Node *>, dim>				neighb_lo;	
	// };
	
	// template<std::size_t lvl> friend class level_stencil_iterator;
	// // iterate over key/node pairs for leaf nodes only 
	// template<std::size_t lvl>
	// class level_stencil_iterator{
	// public:
	// 	typedef level_stencil_iterator self_type;
	// 	typedef std::ptrdiff_t difference_type;
	//     typedef node_stencil value_type;
	//     typedef node_stencil & reference;
	//     typedef node_stencil * pointer;
	//     typedef std::forward_iterator_tag iterator_category;

	// 	// construction
	// 	level_stencil_iterator(Orthtree & t)
	// 	: tree(t)
	// 	, stencil_void(true)
	// 	, it(t.mLevelMaps[lvl].begin()){};

	// 	level_stencil_iterator(Orthtree & t, typename std::unordered_map<std::size_t, Node>::iterator iter)
	// 	: tree(t)
	// 	, stencil_void(true)
	// 	, it(iter){};

	// 	level_stencil_iterator(Orthtree & t, std::size_t key)
	// 	: tree(t)
	// 	, stencil_void(true)
	// 	, it(t.mLevelMaps[lvl].find(key)){};

	// 	// dereferencing
	// 	reference operator*(){if (stencil_void) fill_stencil(); return sten;};

	// 	// preincrement 
	// 	self_type operator++(){
	// 		it++;
	// 		stencil_void = true;
	// 		// have reached the end of level
	// 		return *this;
	// 	}

	// 	// preincrement 
	// 	self_type operator++(int blah){
	// 		it++;
	// 		stencil_void = true;
	// 		// have reached the end of level
	// 		return *this;
	// 	}

	// 	// pointer
	// 	pointer operator->() {if (stencil_void) fill_stencil(); return &sten;};

	// 	// inequality
	// 	bool operator!=(const self_type & leaf) const {return it != leaf.it;};

	// 	// equality
	// 	bool operator==(const self_type & leaf) const {return it == leaf.it;};


	// 	void fill_stencil(){
	// 		// std::cout << "filling stencil" << std::endl;
	// 		// fill the center
	// 		sten.center = it.operator->();
	// 		IntPoint<dim> off = tree.getLevelOffset(it->first);
	// 		std::size_t lvlmax = pow(rfactor,lvl)-1;
	// 		// get neighbors on the high side of each dimension
	// 		for (auto i=0; i<dim; i++){
	// 			sten.neighb_lo[i].first = 0;
	// 			sten.neighb_hi[i].first = 0;
	// 			sten.neighb_lo[i].second = nullptr;
	// 			sten.neighb_hi[i].second = nullptr;
	// 		}
	// 		for (auto i=0; i<dim; i++){
	// 			if (off.x[i] == lvlmax) continue;
	// 			std::size_t nkey = tree.getNeighborKeyMax(it->first, i);
	// 			sten.neighb_hi[i].first = nkey;
	// 			auto nit = tree.mLevelMaps[lvl].find(nkey);
	// 			if (nit != tree.mLevelMaps[lvl].end()) sten.neighb_hi[i].second = &nit->second;
	// 		}
	// 		// get neighbors on the low side of each dimension
	// 		for (auto i=0; i<dim; i++){
	// 			if (off.x[i] == 0) continue;
	// 			std::size_t nkey = tree.getNeighborKeyMin(it->first, i);
	// 			sten.neighb_lo[i].first = nkey;
	// 			auto nit = tree.mLevelMaps[lvl].find(nkey);
	// 			if (nit != tree.mLevelMaps[lvl].end()) sten.neighb_lo[i].second = &nit->second;
	// 		}
	// 		stencil_void = false;
	// 		// std::cout << "filled stencil" << std::endl;
	// 	}

	// 	node_stencil	sten;

	// private:
	// 	typename std::unordered_map<std::size_t, Node>::iterator it;
	// 	Orthtree & tree;
	// 	bool stencil_void;
	// };

	// template<std::size_t lvl>
	// level_stencil_iterator<lvl> level_stencil_begin(){return level_stencil_iterator<lvl>(*this);};
	// template<std::size_t lvl>
	// level_stencil_iterator<lvl> level_stencil_end(){return level_stencil_iterator<lvl>(*this, mLevelMaps[lvl].end());};






	// // define some utility functions (can be specialized)
	// std::size_t getParentKey(std::size_t key) const {return (key-1)/sSize;};
	std::size_t getParentKey(KeyT key) const {return ParentDecoder::decode(key);};


	// std::size_t getLeftChildKey(std::size_t key) const {return key*sSize + 1;};
	std::size_t getLeftChildKey(KeyT key) const {return LeftChildDecoder::decode(key);};


	// std::size_t getLevel(std::size_t key) const {
	// 	std::size_t lvl=0;
	// 	while (key>0){
	// 		lvl++;
	// 		key = (key-1)/sSize;
	// 	}
	// 	return lvl;
	// }

	std::size_t getLevel(KeyT key) const {return LevelDecoder::decode(key);};
	
	// std::size_t getPositionWithinParent(std::size_t key) const {return (key-1)%sSize;};
	
	// IntPoint<dim> getOffsetWithinParent(std::size_t key) const {
	// 	IntPoint<dim> off;
	// 	key = getPositionWithinParent(key);
	// 	for (auto i=0; i<dim; i++) off.x[i] = 0;
	// 	for (auto i=0; i<dim; i++) {
	// 		off.x[i] = key%rfactor; 
	// 		key -= off.x[i];
	// 		key /= rfactor;
	// 	}
	// 	return off;
	// }
	
	// IntPoint<dim> getLevelOffset(std::size_t key) const {

	// 	IntPoint<dim> off;
	// 	// key -= getLevelStartingKey(getLevel(key));
	// 	for (auto i=0; i<dim; i++) off.x[i] = 0;
	// 	std::size_t mult = 1;
	// 	std::size_t pkey;
	// 	while (key>0) {
	// 		// get the offset within parent
	// 		// std::cout << "key: " << key << " offsetWithinParent: " << getOffsetWithinParent(key) <<std::endl;
	// 		off = off + getOffsetWithinParent(key)*mult;
	// 		// begin anew with parent key
	// 		key = getParentKey(key);
	// 		mult *= rfactor;

	// 		// std::cout << "new key: " << key << std::endl;
	// 	}
	// 	return off;
	// }

	// // get the key that starts a given level
	// std::size_t getLevelStartingKey(std::size_t lvl) const {
	// 	std::size_t keystart = 0;	// starting at level 1
	// 	std::size_t lct = 1;
	// 	for (auto l=0; l<lvl; l++){
	// 		keystart += lct;
	// 		lct *= sSize;
	// 	}
	// 	return keystart;
	// }

	std::size_t getLevelStartingKey(std::size_t lvl) const {return mLvlStartKeys[lvl];};

	// // get the key that starts a given level
	// std::size_t getLevelEndingKey(std::size_t lvl) const {
	// 	return getLevelStartingKey(lvl+1)-1;
	// 	// return keystart;
	// }

	std::size_t getLevelEndingKey(std::size_t lvl) const {return mLvlEndKeys[lvl];};

	// // get a key from a offset on a given level
	// std::size_t getKeyFromLevelOffset(std::size_t lvl, IntPoint<dim> off) const {
	// 	// get starting key for the level
	// 	std::size_t keystart = getLevelStartingKey(lvl);

	// 	std::size_t ct = 1;
	// 	std::size_t tot =0;
	// 	IntPoint<dim> mult;
	// 	IntPoint<dim> dotter;
	// 	std::size_t rval = 1;
	// 	for (auto i=0; i<dim; i++){
	// 		mult.x[i] = rval;
	// 		rval *= rfactor;
	// 	}
		
	// 	dotter = off%rfactor;
	// 	tot += ct*IntPoint<dim>::dot(mult, dotter);

	// 	for (auto l=lvl-1; l>0; l--){
	// 		// increase ct
	// 		ct *= sSize;
	// 		// divide by rfactor
	// 		off = off/static_cast<int>(rfactor);
	// 		// take modulus
	// 		dotter = off%rfactor;
	// 		// accumulate dot products
	// 		tot += ct*IntPoint<dim>::dot(mult, dotter);
	// 	}

	// 	// add the start key and return;
	// 	return keystart + tot;
	// }

	// // get the neighboring key to 'key' on the minimum side
	// // along a dimension specified by 'd'
	// std::size_t getNeighborKeyMin(std::size_t key, std::size_t d) const {
	// 	// get the level of this key
	// 	std::size_t lvl = getLevel(key);
	// 	// get the level offset of this key
	// 	IntPoint<dim> loff = getLevelOffset(key);
	// 	// subtract 1 in the chosen dimension
	// 	loff.x[d]--;
	// 	// get the key from level offset
	// 	std::size_t neighbor = getKeyFromLevelOffset(lvl, loff);
	// 	return neighbor;
	// }

	// // get the neighboring key to 'key' on the maximum side
	// // along a dimension specified by 'd'
	// std::size_t getNeighborKeyMax(std::size_t key, std::size_t d) const {
	// 	// get the level of this key
	// 	std::size_t lvl = getLevel(key);
	// 	// get the level offset of this key
	// 	IntPoint<dim> loff = getLevelOffset(key);
	// 	// add 1 in the chosen dimension
	// 	loff.x[d]++;
	// 	// get the key from level offset
	// 	std::size_t neighbor = getKeyFromLevelOffset(lvl, loff);
	// 	return neighbor;
	// }

	// // set an existing key in the level map as a boundary.
	// // this will add the key to the boundary key map
	// void setKeyAsBoundary(std::size_t key) {
	// 	std::size_t lvl = getLevel(key);
	// 	mBdryMaps[lvl][key] = &mLevelMaps[lvl][key];
	// }


	// // return whether the given key exists as a boundary cell
	// bool isKeyBoundary(std::size_t key) {
	// 	std::size_t lvl = getLevel(key);
	// 	return !(mBdryMaps[lvl].find(key) == mBdryMaps[lvl].end());
	// }


	// // for a given level, if a node is on the boundary 
	// // it will be turned into a boundary node
	// template <std::size_t lvl>
	// void annexLevelBoundary() {
	// 	auto lmap = mLevelMaps[lvl];
	// 	std::list<std::size_t> bkeys;
	// 	auto l = level_begin<lvl>();
	// 	// build list of cells to be added to level
	// 	for (auto lit = level_begin<lvl>(); lit!=level_end<lvl>(); lit++){
	// 		// check to see if each neighbor exists in the level map
	// 		std::size_t key = lit->first;
	// 		std::size_t lmax = pow(rfactor, lvl) - 1;//Power<rfactor, lvl>::value - 1;
	// 		IntPoint<dim> lvlind = getLevelOffset(key);

	// 		// if any of the indices is 0 or lmax, it is a boundary point
	// 		bool bdryfound = false;
	// 		for (auto d=0; d<dim; d++){
	// 			// check min
	// 			if (lvlind.x[d] == 0){
	// 				bdryfound = true;
	// 				bkeys.push_back(key);
	// 				break;
	// 			}
	// 			if (lvlind.x[d] == lmax){
	// 				bdryfound = true;
	// 				bkeys.push_back(key);
	// 				break;
	// 			}
	// 		}

	// 		if (bdryfound) continue;


	// 		// if the cell is not a boundary cell, check neighbors	
	// 		std::size_t minKey, maxKey;
	// 		for (auto d=0; d<dim; d++){
	// 			// check min
	// 			minKey = getNeighborKeyMin(lit->first, d);
	// 			if (lmap.find(minKey) == lmap.end()){
	// 				bkeys.push_back(key);
	// 				break;
	// 			}
	// 			// check max
	// 			maxKey = getNeighborKeyMax(lit->first, d);
	// 			if (lmap.find(maxKey) == lmap.end()){
	// 				bkeys.push_back(key);
	// 				break;
	// 			}
	// 		}		
	// 	}

	// 	// now add all the bkeys as boundary nodes
	// 	// push all keys into the appropriate level map and boundary map
	// 	// std::cout << "YEEHAW" << std::endl;
	// 	for (auto lsit = bkeys.begin(); lsit != bkeys.end(); lsit++){
	// 		// std::cout << "annexing boundary key: " << *lsit << " address: " << &(mLevelMaps[lvl][*lsit]) << " offset: " << getLevelOffset(*lsit) << std::endl;
	// 		mBdryMaps[lvl].insert({*lsit, &(mLevelMaps[lvl][*lsit])});
	// 		mLevelMaps[lvl][(*lsit)].mIsLeaf = true;
	// 	}
	// }


	// template <std::size_t lvl,
	// 		  class PrototypeMap>
	// void reassignLevelBoundary(const PrototypeMap & pm){
	// 	// iterate through the level boundary and replace values
	// 	for (auto it=boundary_begin<lvl>(); it != boundary_end<lvl>(); it++){
	// 		it->second.mVal = std::make_shared<ValueT>(pm.getPrototype());
	// 		// std::cout << "addr: " << &val << " addr_new: " << it->second.mVal.get()->get() << std::endl;
	// 	}
	// }

	// // build out a level boundary by going through all the points
	// // and adding boundary cells to the left/right if the neighbors
	// // on a given side don't exist
	// template <std::size_t lvl,
	// 		  class PrototypeMap>
	// void buildLevelBoundary(const PrototypeMap & pm) {
	// 	auto lmap = mLevelMaps[lvl];
	// 	std::list<std::size_t> bkeys;
	// 	// build list of cells to be added to level
	// 	for (auto lit = level_begin<lvl>(); lit!=level_end<lvl>(); lit++){
	// 		// check to see if each neighbor exists in the level map
	// 		std::size_t minKey, maxKey;
	// 		for (auto d=0; d<dim; d++){
	// 			// check min
	// 			minKey = getNeighborKeyMin(lit->first, d);
	// 			if (lmap.find(minKey) == lmap.end()) bkeys.push_back(minKey);
	// 			// check max
	// 			maxKey = getNeighborKeyMax(lit->first, d);
	// 			if (lmap.find(maxKey) == lmap.end()) bkeys.push_back(maxKey);
	// 		}
	// 	}

	// 	// push all keys into the appropriate level map and boundary map
		
	// 	for (auto lsit = bkeys.begin(); lsit != bkeys.end(); lsit++){
	// 		// std::cout << "building bkey: " << *lsit << " offset: " << getLevelOffset(*lsit) << std::endl;
	// 		Node n;
	// 		n.mVal = std::make_shared<ValueT>(pm.getPrototype());
	// 		n.mIsLeaf = true;
	// 		mLevelMaps[lvl][*lsit] = std::move(n);

	// 		mBdryMaps[lvl][*lsit] = &(mLevelMaps[lvl][*lsit]);
	// 	}

	// 	// // print out the pointers
	// 	// for (auto lsit = mBdryMaps[lvl].begin(); lsit != mBdryMaps[lvl].end(); lsit++){
	// 	// 	// std::cout << "building bkey: " << *lsit << " offset: " << getLevelOffset(*lsit) << std::endl;
	// 	// 	std::cout << "bdry key: " << lsit->first << " bdry address: " << (lsit->second) << " isLeaf? " << lsit->second->mIsLeaf << std::endl;
	// 	// }
	// }

	
};




}

#endif