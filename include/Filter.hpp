#ifndef _FILTER_H
#define _FILTER_H

namespace Filter{

	// ***** decimating filters *****
	// these filters return a subset of the input values as their output
	template <class T>
	void crop(const vector<T> & values, T minval, T maxval, vector<bool> & mask);


	// ***** in-place filters *****
	// these filters operate on the input values in-situ

}

#endif
