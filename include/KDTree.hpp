#ifndef _KDTREE_H
#define _KDTREE_H

#include <iostream>
#include <vector>

#include "Point.hpp"

using namespace std;


template <size_t dim>
class KDTree{
public:

	// constructor
	KDTree(vector<Point<dim>> & pts);


private:

	unsigned int m_num_dims;
	vector<double> m_data;

};

namespace KDTree{

	struct Node{
		unsigned int nodeIdx;
		unsigned int parentIdx;
		unsigned int leftChildIdx;
		unsigned int rightChildIdx;
	};

}




#endif
