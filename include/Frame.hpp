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
	Box<1> get_bounding_box() const {return Box<1>({0}, {length()});};
};


template <std::size_t dim, typename SceneType>
Frame1<SceneType, dim> make_frame_1d(const SceneType & s, Point<dim> start, Point<dim> end){
	return Frame1<SceneType, dim>(s, start, end);
}








template <typename SceneType, std::size_t scene_dim>
class Frame2
{
private:
	std::shared_ptr<SceneType> 	mScene;
	Point<scene_dim> 			mStart;
	Point<scene_dim>			mEnd;
	Point<scene_dim>			mVecX;

	double length_diag() const {return (mEnd-mStart).norm();};
	Point<scene_dim> vecy() const {return ((mEnd-mStart) - lengthx()*mVecX).normalize();};
public:
	Frame2(const SceneType & s, Point<scene_dim> start, Point<scene_dim> end, Point<scene_dim> xvec)
	: mScene(std::make_shared<SceneType>(s)) 
	, mStart(start)
	, mEnd(end)
	, mVecX(xvec.normalize())
	{};

	decltype(auto) query_point(Point<2> p) const {
		// std::cout << " xvec: " << mVecX << " yvec: " << vecy() << " ";
		return mScene->query_point(mStart + p.x[0]*mVecX + p.x[1]*vecy());
	}

	double lengthx() const {return Point<scene_dim>::dot((mEnd-mStart), mVecX);};
	double lengthy() const {return sqrt(length_diag()*length_diag() - lengthx()*lengthx());};
	Box<2> get_bounding_box() const {return Box<2>({0,0}, {lengthx(), lengthy()});};
};



template <std::size_t dim, typename SceneType>
Frame2<SceneType, dim> make_frame_2d(const SceneType & s, Point<dim> start, Point<dim> end, Point<dim> xvec){
	return Frame2<SceneType, dim>(s, start, end, xvec);
}






template <typename SceneType, std::size_t scene_dim>
class Frame3
{
private:
	std::shared_ptr<SceneType> 	mScene;
	Point<scene_dim> 			mStart;
	Point<scene_dim>			mEnd;
	Point<scene_dim>			mVecX;
	Point<scene_dim> 			mVecY;

	double length_diag() const {return (mEnd-mStart).norm();};
	Point<scene_dim> vecz() const {return ((mEnd-mStart) - lengthx()*mVecX - lengthy()*mVecY).normalize();};
public:
	Frame3(const SceneType & s, Point<scene_dim> start, Point<scene_dim> end, Point<scene_dim> xvec, Point<scene_dim> yvec)
	: mScene(std::make_shared<SceneType>(s)) 
	, mStart(start)
	, mEnd(end)
	, mVecX(xvec.normalize())
	, mVecY(yvec.normalize())
	{};

	decltype(auto) query_point(Point<3> p) const {
		// std::cout << " xvec: " << mVecX << " yvec: " << vecy() << " ";
		return mScene->query_point(mStart + p.x[0]*mVecX + p.x[1]*mVecY + p.x[2]*vecz());
	}

	double lengthx() const {return Point<scene_dim>::dot((mEnd-mStart), mVecX);};
	double lengthy() const {return Point<scene_dim>::dot((mEnd-mStart), mVecY);};
	double lengthz() const {return sqrt(length_diag()*length_diag() - lengthx()*lengthx() - lengthy()*lengthy());};
	Box<3> get_bounding_box() const {return Box<3>({0,0,0}, {lengthx(), lengthy(), lengthz()});};

};



template <std::size_t dim, typename SceneType>
Frame3<SceneType, dim> make_frame_3d(const SceneType & s, Point<dim> start, Point<dim> end, Point<dim> xvec, Point<dim> yvec){
	return Frame3<SceneType, dim>(s, start, end, xvec, yvec);
}




} // end namespace csg
#endif