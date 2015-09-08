#ifndef _SORT_H
#define _SORT_H

#include <algorithm>

using namespace std;

namespace Sort{

	template<class T>
	void cardsort(vector<T> & arr){
		unsigned int i, j, n = arr.size();
		T a;
		for (j=1; j<n; j++){
			a = arr[j];
			i=j;
			while (i>0 && arr[i-1] > a){
				arr[i] = arr[i-1];
				i--;
			}
			arr[i] = a;
		}
	}


	template<class T>
	void quicksort(vector<T> & arr){
		static const unsigned int M = 7, NSTACK = 64;
		int i, ind_right, j, k, jstack = -1, ind_left = 0, n = arr.size();
		T a;
		vector<int> istack(NSTACK);
		ind_right = n-1;
		for (;;){
			if (ind_right - ind_left < M){
				a = arr[j];
				for (i=j-1; i>=1; i--){
					if (arr[i] <= a) break;
					arr[i+1] = arr[i];
				}
				arr[i+1] = a;
			}
			if (jstack < 0) break;
			ind_right = istack[jstack--];
			ind_left = istack[jstack--];
		}
		else {
			k = (ind_left + ind_right) >> 1;

			swap(arr[k], arr[ind_left + 1]);
			if (arr[ind_left] > arr[ind_right]) swap(arr[ind_left], arr[ind_right]);
			if (arr[ind_left + 1] > arr[ind_right]) swap(arr[ind_left + 1], arr[ind_right]);
			if (arr[ind_left] > arr[ind_left + 1]) swap(arr[ind_left], arr[ind_left + 1]);
			i = ind_left + 1;
			j = ind_right;
			a = arr[i];
			for (;;){
				do i++; while (arr[i] < a);
				do j--; while (arr[j] > a);
				if (j<i) break;
				swap(arr[i], arr[j]);
			}
			arr[ind_left+1] = arr[j];
			arr[j] = a;

			jstack += 2;
			if (jstack >= NSTACK) throw("NSTACK is too small for quicksort");
			if (ind_right - i + 1 >= j-1){
				istack[jstack] = ind_right;
				istack[jstack-1] = i;
				ind_right = j - 1;
			}
			else {
				istack[jstack] = j-1;
				istack[jstack-1] = ind_left;
				ind_left = i;

			}

		}
	}


	template <class T>
	void heapsort(vector<T> & arr);

}

#endif