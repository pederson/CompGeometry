#ifndef _FRAME_H
#define _FRAME_H

#include <memory>
#include <map>

namespace csg{

template <typename SceneType, std::size_t scene_dim>
class Frame1
{
private:
	std::shared_ptr<SceneType> 	mScene;
	Point<scene_dim> 			mStart;
	Point<scene_dim>			mEnd;
	Point<scene_dim>			mVec;
public:
	Frame1(const SceneType & s, Point<scene_dim> start, Point<scene_dim> end)
	: mScene(std::make_shared<SceneType>(s)) 
	, mStart(start)
	, mEnd(end)
	{mVec = mEnd - mStart; mVec = mVec.normalize();};

	decltype(auto) query_point(double x) const {
		// std::cout << " mVec: " << mVec << " querying point: " << mStart + x*mVec << " " ;
		return mScene->query_point(mStart + x*mVec);
	}

	double length() const {return (mEnd-mStart).norm();};
};


template <std::size_t dim, typename SceneType>
Frame1<SceneType, dim> make_frame_1d(const SceneType & s, Point<dim> start, Point<dim> end){
	return Frame1<SceneType, dim>(s, start, end);
}

} // end namespace csg
#endif