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



//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################




template <class ValueT>
struct DefaultNode{
	ValueT  		mVal;
	bool 			mIsLeaf;

	DefaultNode() {};
	DefaultNode(ValueT & v, bool islf): mVal(v), mIsLeaf(islf) {};

	ValueT & getValue() {return mVal;};
	bool & isLeaf() {return mIsLeaf;};
};





//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################





// Default decoder assumes that the KeyT is an integral type
// that solely contains the global index info (no level or subdomain info)
template<std::size_t dim,
		 std::size_t rfactor,
		 class KeyT>
class IntegralKeyDecoder{
public:
	static_assert(std::is_integral<KeyT>::value,"Integral type required for default Orthtree decoder");

	const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	constexpr std::size_t getIndex(KeyT key) const {return key;};

	constexpr std::size_t getLevel(KeyT key) const {
		std::size_t lvl=0;
		while (key>0){
			lvl++;
			key = (key-1)/sSize;
		};
		return lvl;
	};

	constexpr std::size_t getSubdomain(KeyT key) const {return 0;};

	constexpr KeyT getParentKey(KeyT key) const {
		return (key-1)/sSize;
	};

	constexpr KeyT getChildKey(KeyT key, std::size_t siblingIdx) const {
		return key*sSize + 1 + siblingIdx;
	};

	constexpr std::size_t getSiblingIndex(KeyT key) const {return (key-1)%sSize;};



	// get the neighboring key to 'key' on the maximum side
	// along a dimension specified by 'd'
	constexpr KeyT getNeighborKeyMax(KeyT key, std::size_t d) const {
		// get the level of this key
		std::size_t lvl = getLevel(key);
		// get the level offset of this key
		IntPoint<dim> loff = getOffsetWithinLevel(key);
		// add 1 in the chosen dimension
		loff.x[d]++;
		// get the key from level offset
		return getKeyFromLevelOffset(lvl, loff);
	};

	// get the neighboring key to 'key' on the minimum side
	// along a dimension specified by 'd'
	constexpr KeyT getNeighborKeyMin(KeyT key, std::size_t d) const {
		// get the level of this key
		std::size_t lvl = getLevel(key);
		// get the level offset of this key
		IntPoint<dim> loff = getLevelOffset(key);
		// subtract 1 in the chosen dimension
		loff.x[d]--;
		// get the key from level offset
		return getKeyFromLevelOffset(lvl, loff);;
	}


	constexpr bool isBoundary(KeyT key) const {
		IntPoint<dim> off = getLevelOffset(key);
		std::size_t lvl = getLevel(key);
		std::size_t imax = pow(rfactor,lvl)-1;
		for (auto d=0; d<dim; d++) if (off.x[d]==0) return true;
		for (auto d=0; d<dim; d++) if (off.x[d]==imax) return true;
		return false;
	};

// protected:


	constexpr IntPoint<dim> getLevelOffset(KeyT key) const {

		IntPoint<dim> off;
		// key -= getLevelStartingKey(getLevel(key));
		for (auto i=0; i<dim; i++) off.x[i] = 0;
		std::size_t mult = 1;
		// std::size_t pkey;
		while (key>0) {
			// get the offset within parent
			// std::cout << "key: " << key << " offsetWithinParent: " << getOffsetWithinParent(key) <<std::endl;
			off = off + getOffsetWithinParent(key)*mult;
			// begin anew with parent key
			key = getParentKey(key);
			mult *= rfactor;

			// std::cout << "new key: " << key << std::endl;
		}
		return off;
	}


	constexpr IntPoint<dim> getOffsetWithinParent(KeyT key) const {
		IntPoint<dim> off;
		std::size_t k = getSiblingIndex(key);
		for (auto i=0; i<dim; i++) off.x[i] = 0;
		for (auto i=0; i<dim; i++) {
			off.x[i] = k%rfactor; 
			k -= off.x[i];
			k /= rfactor;
		}
		return off;
	}


	constexpr IntPoint<dim> getOffsetWithinLevel(KeyT key) const{

		IntPoint<dim> off;
		// key -= getLevelStartingIndex(getLevel(key));
		for (auto i=0; i<dim; i++) off.x[i] = 0;
		std::size_t mult = 1;
		// std::size_t pkey;
		while (key>0) {
			// get the offset within parent
			// std::cout << "key: " << key << " offsetWithinParent: " << getOffsetWithinParent(key) <<std::endl;
			off = off + getOffsetWithinParent(key)*mult;
			// begin anew with parent key
			key = getParentKey(key);
			mult *= rfactor;

			// std::cout << "new key: " << key << std::endl;
		}
		return off;
	}


	// get a key from a offset on a given level
	constexpr KeyT getKeyFromLevelOffset(std::size_t lvl, IntPoint<dim> off) const{
		// get starting key for the level
		// std::size_t keystart = mLvlStartInds[lvl];
		std::size_t keystart = getLevelStartingIndex(lvl);


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

		for (auto l=lvl-1; l>0; l--){
			// increase ct
			ct *= sSize;
			// divide by rfactor
			off = off/static_cast<int>(rfactor);
			// take modulus
			dotter = off%rfactor;
			// accumulate dot products
			tot += ct*IntPoint<dim>::dot(mult, dotter);
		}

		// add the start key and return;
		return keystart + tot;
	}


	constexpr KeyT getLevelStartingIndex(std::size_t lvl) const{
		if (lvl==0) return 0;

		return pow(sSize,lvl-1)+getLevelStartingIndex(lvl-1);
	}
};




//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################





template <class KeyT, class MappedT>
struct LevelContainer{
	// these map an integer LEVEL (starting from 0) to another map
	// that maps a KEY to a NODE
	std::map<std::size_t, std::unordered_map<KeyT, MappedT>> 		mKeyMaps; 		

	// is required to have the following:
 // *							- MappedT & operator[](KeyT key)
 // *							- void insert(KeyT key, MappedT node)
 // *							- void erase(KeyT key)
 // *							- ::iterator, begin(), end()
 // *							- iterator find(KeyT key)


	friend class iterator;
	// iterate over key/node pairs for all nodes 
	class iterator{
	public:
		typedef iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		iterator(LevelContainer & t)
		: cont(t)
		, lit(t.mKeyMaps.begin())
		, it((t.mKeyMaps.begin())->second.begin()){};

		iterator(LevelContainer & t, std::size_t lvl, typename std::unordered_map<KeyT, MappedT>::iterator iter)
		: cont(t)
		, lit(t.mKeyMaps.find(lvl))
		, it(iter) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			while (lit != cont.mKeyMaps.end()){
				if (it != lit->second.end()){
					return *this;				
				}

				// reached end of level
				lit++;

				if (lit == cont.mKeyMaps.end()) break;
				it = lit->second.begin();
			}
			// have reached the end of all the cells
			return cont.end();
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			while (lit != cont.mKeyMaps.end()){
				if (it != lit->second.end()){
					return *this;				
				}

				// reached end of level
				lit++;

				if (lit == cont.mKeyMaps.end()) break;
				it = lit->second.begin();
			}
			// have reached the end of all the cells
			return cont.end();
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename std::map<std::size_t, std::unordered_map<KeyT, MappedT>>::iterator lit;
		typename std::unordered_map<KeyT, MappedT>::iterator it;
		LevelContainer & cont;
	};

	
	iterator begin(){return iterator(*this);};
	iterator end(){auto p=mKeyMaps.end(); p--; return iterator(*this, p->first, p->second.end());};





	friend class level_iterator;
	// iterate over key/node pairs for all nodes 
	class level_iterator{
	public:
		typedef level_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, MappedT> value_type;
	    typedef std::pair<const KeyT, MappedT> & reference;
	    typedef std::pair<const KeyT, MappedT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		level_iterator(LevelContainer & t, std::size_t level)
		: cont(t)
		, lvl(level)
		, it(t.mKeyMaps[level].begin()){};

		level_iterator(LevelContainer & t, std::size_t level, typename std::unordered_map<KeyT, MappedT>::iterator iter)
		: cont(t)
		, lvl(level)
		, it(iter) {};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			return *this;
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			return *this;
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename std::unordered_map<KeyT, MappedT>::iterator it;
		LevelContainer & cont;
		std::size_t lvl;
	};



	level_iterator level_begin(std::size_t lvl){return level_iterator(*this, lvl);};
	level_iterator level_end(std::size_t lvl){return level_iterator(*this, lvl, mKeyMaps[lvl].end());};


	iterator find(KeyT key, std::size_t lvl){
		return iterator(*this,lvl,mKeyMaps[lvl].find(key));
	}



	// MappedT & operator[](const KeyT & key){};

	void insert(const KeyT & key, std::size_t lvl, MappedT & val){
		mKeyMaps[lvl][key] = val;
	};

	void erase(const KeyT & key, std::size_t lvl){mKeyMaps[lvl].erase(key);};

	void erase(iterator position, std::size_t lvl){mKeyMaps[lvl].erase(position);};



};




//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################
//##############################################################





/** @class Orthtree
 *	@brief Orthogonal multi-level tree structure with constant refinement factor
 *
 *	@tparam dim 		how many dimensions does the tree cover
 *	@tparam rfactor 	refinement factor between levels
 *	@tparam ValueT 		the object stored within the nodes
 *	@tparam KeyT 		the key type
 *	@tparam KeyDecoderRing	the decoder class for KeyT
 *							is required to have the following:
 *							- std::size_t getLevel(KeyT key)
 *							- KeyT getParentKey(KeyT key)
 *							- KeyT getChildKey(KeyT key, std::size_t siblingIdx)
 *	@tparam NodeT 		the node type that stores the values
 *							is required to have the following:
 *							- ValueT & getValue()
 *							- bool & isLeaf()
 *	@tparam ContainerT 	the container for <KeyT, NodeT> pairs
 *							is required to have the following:
 *							- NodeT & operator[](KeyT key)
 *							- void insert(KeyT key, NodeT node)
 *							- void insert(KeyT key, std::size_t lvl, NodeT node)
 *							- void erase(KeyT key)
 *							- ::iterator, begin(), end()
 *							- iterator find(KeyT key)
 *							- iterator find(KeyT key, std::size_t lvl)
 *
 *
 * @details An orthogonal multi-level tree (orthree) recursively refines
 *			each cell and stores some value in the leaf nodes. 
 *			Additional functionality can be defined if the templated types have
 * 			extended functionality
 */
template <	std::size_t dim,
			std::size_t rfactor,
			class ValueT,
			class KeyT = std::size_t,
			template <std::size_t,std::size_t,typename> class KeyDecoderRing = IntegralKeyDecoder,
			class NodeT = DefaultNode<ValueT>,
			template <typename,typename> class ContainerT = LevelContainer,
			std::size_t lvlmax = 16>
class Orthtree : public KeyDecoderRing<dim,rfactor,KeyT>, public ContainerT<KeyT, NodeT>{
public:
	typedef KeyDecoderRing<dim,rfactor,KeyT> KeyDecoder;
	typedef ContainerT<KeyT, NodeT> Container;
	typedef ValueT ValueType;
	typedef KeyT KeyType;
	typedef NodeT NodeType;

protected:

	static const std::size_t 	sSize = Power<rfactor, dim>::value;	// the number of children possible

	// some convenience values created at compile time
	std::array<std::size_t, lvlmax+1>			mLvlStartInds;//={createStartingKeys<dim,rfactor,lvlmax>()};
	std::array<std::size_t, lvlmax+1>			mLvlEndInds;//={createStartingKeys<dim,rfactor,lvlmax>()};

public:


	Orthtree()
	: mLvlStartInds(createStartingKeys<dim,rfactor,lvlmax+1>())
	, mLvlEndInds(createEndingKeys<dim,rfactor,lvlmax+1>()){};


	// ********** define some construction functions

	// build tree using an indicator function that outputs an int for a given point,
	// then use a map to a prototype Value to fill the tree
	template <class PrototypeMap, 
			  class RefineOracle>
	void buildTree(std::size_t lvlstop, 
				  const PrototypeMap & pm, 
				  const RefineOracle & ro,
				  KeyT key, std::size_t lvl){

		// create node for key
		NodeT n; n.isLeaf() = true; 
		n.getValue() = pm.getValue(key);
		Container::insert(key, lvl, n);

		// decide if refinement is necessary
		if (lvl == lvlstop) return;
		if (ro.isUniform(key)) return;

		// mKeyMaps[lvl][subd][key].mIsLeaf = false;
		Container::find(key,lvl)->second.isLeaf() = false;

		// if refining, 
		for (auto so=0; so<sSize; so++){
			KeyT kc = KeyDecoder::getChildKey(key, so);
			buildTree(lvlstop, pm, ro, kc, lvl+1);
		}
	}

	// //********** random access


	// // split parent key, and endow all children
	// // with a copy of the parent value
	// void refineCell(KeyT key) {
	// 	std::size_t lvl = LevelDecoder::decodeLevel(key);
	// 	std::size_t subd = SubdomainDecoder::decodeSubdomain(key);
	// 	if (!mKeyMaps[lvl][subd][key].mIsLeaf){
	// 		std::cerr << "Orthtree: Cell is already refined!" << std::endl;
	// 		throw -1;
	// 	}
	// 	Node n = mKeyMaps[lvl][subd].at(key);
	// 	for (auto so=0; so<sSize; so++){
	// 		KeyT kc = ChildDecoder::getChildKey(key, so);
	// 		std::size_t sd = SubdomainDecoder::decodeSubdomain(kc);
	// 		mKeyMaps[lvl+1][sd][kc] = n;
	// 	}
	// 	mKeyMaps[lvl][subd][key].mIsLeaf = false;
	// }


	// delete all child cells of a given parent key
	void pruneChildren(KeyT key, std::size_t lvl) {
		auto pk = Container::find(key,lvl);
		if (pk->second.isLeaf()){
			std::cerr << "Orthtree: Cell is already a leaf!" << std::endl;
			throw -1;
		}

		for (auto so=0; so<sSize; so++){
			KeyT kc = KeyDecoder::getChildKey(key, so);
			auto pc = Container::find(kc,lvl+1);
			if (! pc->second.isLeaf()) pruneChildren(kc,lvl+1);
			Container::erase(pc, lvl);
		}
		pk->second.isLeaf() = true;
	}

	// void print_summary(std::ostream & os = std::cout) const {
	// 	// using comptype = std::pair<const std::size_t, Node>;
	// 	for (auto lit=mKeyMaps.begin(); lit!=mKeyMaps.end(); lit++){
	// 		auto ntot=0;

	// 		for (auto sit=lit->second.begin(); sit!=lit->second.end(); sit++){
	// 			// auto minmax = std::minmax_element(lit->second.begin(), lit->second.end(), [](comptype p1, comptype p2)->bool{return p1.first < p2.first;});
	// 			ntot+=sit->second.size();
	// 		}

	// 		os << "Level (" << lit->first << ") --> " << ntot;
	// 		for (auto sit=lit->second.begin(); sit!=lit->second.end(); sit++){
	// 			os << " subdom(" << sit->first << ")->" << sit->second.size();
	// 		}

	// 		os << std::endl;

	// 	}
	// }


	friend class leaf_iterator;
	// iterate over key/node pairs for all nodes 
	class leaf_iterator{
	public:
		typedef leaf_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, NodeT> value_type;
	    typedef std::pair<const KeyT, NodeT> & reference;
	    typedef std::pair<const KeyT, NodeT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		leaf_iterator(Orthtree & t)
		: tree(t)
		, cit(t.begin()){
			if (! cit->second.isLeaf()){
				this->operator++();
			};
		};

		leaf_iterator(Orthtree & t, typename Container::iterator iter)
		: tree(t)
		, cit(iter){};

		// dereferencing
		reference operator*(){ return *cit;};

		// preincrement 
		self_type operator++(){
			cit++;
			while (cit != tree.end()){
				if (cit->second.isLeaf()) return *this;				
				cit++;
			}
			// have reached the end of all the cells
			return tree.leaf_end();
		}

		// postincrement 
		self_type operator++(int blah){
			cit++;
			while (cit != tree.end()){
				if (cit->second.isLeaf()) return *this;				
				cit++;
			}
			// have reached the end of all the cells
			return tree.leaf_end();
		}

		// pointer
		pointer operator->() {return cit.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return cit != leaf.cit;};

		// equality
		bool operator==(const self_type & leaf) const {return cit == leaf.cit;};


	private:
		typename Container::iterator cit;
		Orthtree & tree;
	};

	
	leaf_iterator leaf_begin(){return leaf_iterator(*this);};
	leaf_iterator leaf_end(){return leaf_iterator(*this, Container::end());};






	bool isBoundary(KeyT key, std::size_t lvl){
		if (KeyDecoder::isBoundary(key)) return true;

		for (auto d=0; d<dim; d++){
			KeyT neighb = KeyDecoder::getNeighborKeyMin(key,d);
			if (Container::find(neighb, lvl) == Container::end()) return true;
		}
		for (auto d=0; d<dim; d++){
			KeyT neighb = KeyDecoder::getNeighborKeyMax(key,d);
			if (Container::find(neighb, lvl) == Container::end()) return true;
		}
		return false;
	}

	friend class boundary_iterator;
	// iterate over key/node pairs for all boundary nodes
	class boundary_iterator{
	public:
		typedef boundary_iterator self_type;
		typedef std::ptrdiff_t difference_type;
	    typedef std::pair<const KeyT, NodeT> value_type;
	    typedef std::pair<const KeyT, NodeT> & reference;
	    typedef std::pair<const KeyT, NodeT> * pointer;
	    typedef std::forward_iterator_tag iterator_category;

		// construction
		boundary_iterator(Orthtree & t, std::size_t level)
		: tree(t)
		, lvl(level)
		, it(t.level_begin(level)){
			if (! t.isBoundary(it->first, lvl)){
				this->operator++();
			};
		};

		boundary_iterator(Orthtree & t, std::size_t level, typename Container::level_iterator iter)
		: tree(t)
		, lvl(level)
		, it(iter){};

		// dereferencing
		reference operator*(){ return *it;};

		// preincrement 
		self_type operator++(){
			it++;
			while (it != tree.level_end(lvl)){
				if (tree.isBoundary(it->first,lvl)) return *this;				
				it++;
			}
			// have reached the end of all the cells
			return tree.boundary_end(lvl);
		}

		// postincrement 
		self_type operator++(int blah){
			it++;
			while (it != tree.level_end(lvl)){
				if (tree.isBoundary(it->first,lvl)) return *this;				
				it++;
			}
			// have reached the end of all the cells
			return tree.boundary_end(lvl);
		}

		// pointer
		pointer operator->() {return it.operator->();};

		// inequality
		bool operator!=(const self_type & leaf) const {return it != leaf.it;};

		// equality
		bool operator==(const self_type & leaf) const {return it == leaf.it;};


	private:
		typename Container::level_iterator it;
		Orthtree & tree;
		std::size_t lvl;
	};

	
	boundary_iterator boundary_begin(std::size_t lvl){return boundary_iterator(*this,lvl);};
	boundary_iterator boundary_end(std::size_t lvl){return boundary_iterator(*this, lvl, Container::level_end(lvl));};



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






	// define some utility functions (can be specialized)


	// // set an existing key in the level map as a boundary.
	// // this will add the key to the boundary key map
	// void setKeyAsBoundary(std::size_t key) {
	// 	std::size_t lvl = getLevel(key);
	// 	mBdryMaps[lvl][key] = &mLevelMaps[lvl][key];
	// }

	// // set an existing key in the level map as a boundary.
	// // this will add the key to the boundary key map
	// void changeKeySubdomain(KeyT key, std::size_t lvl, std::size_t subd) {
	// 	std::size_t old_subd = SubdomainDecoder::decodeSubdomain(key);
	// 	mKeyMaps[lvl][subd][key] = &mKeyMaps[lvl][old_subd][key];
	// 	mKeyMaps[lvl][old_subd].remove(key);
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

	// template <class PrototypeMap>
	// void reassignLevelSubdomain(std::size_t subd,
	// 							std::size_t lvl,
	// 							const PrototypeMap & pm){
	// 	// iterate through the level boundary and replace values
	// 	for (auto it=subdomain_begin(subd,lvl); it != subdomain_end(subd,lvl); it++){
	// 		it->second.mVal = pm.getPrototype();
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