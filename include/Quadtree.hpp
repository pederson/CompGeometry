#ifndef _QUADTREE_H
#define _QUADTREE_H

#include "Orthtree.hpp"

namespace csg{

// have to do this sort of odd thing in order to call it a "Quadtree"
template<class ValueT>
struct QuadtreeTypedef{
	typedef Orthtree<2,2,ValueT> type;
};

template<class ValueT>
using Quadtree = typename QuadtreeTypedef<ValueT>::type;


// partial template specializations would go here

}

#endif