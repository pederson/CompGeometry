#ifndef _OCTREE_H
#define _OCTREE_H

#include "Orthtree.hpp"

namespace csg{

// have to do this sort of odd thing in order to call it a "Octree"
template<class ValueT>
struct OctreeTypedef{
	typedef Orthtree<3,2,ValueT> type;
};

template<class ValueT>
using Octree = typename OctreeTypedef<ValueT>::type;


// partial template specializations would go here

}// end namespace csg

#endif