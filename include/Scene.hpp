#ifndef _SCENE_H
#define _SCENE_H

#include <memory>
#include <map>
// #include "GeomUtils.hpp"
// #include "Delaunay.hpp"

namespace csg{

template <typename Identifier, typename PrimitiveType>
class Scene : public std::map<Identifier, std::shared_ptr<PrimitiveType>>
{
private:
	typedef std::map<Identifier, std::shared_ptr<PrimitiveType>> base_type;
	using base_type::begin;
	using base_type::end;
	using base_type::cbegin;
	using base_type::cend;
	using iterator = typename base_type::iterator;
public:
	using base_type::insert;

	Scene(Identifier bg) : mBackground(bg) {};


	// template <unsigned int ndim>
	// Identifier query_point(const Point<ndim> & pt) const {
	// 	Identifier id = mBackground;
	// 	for (auto it=begin(); it!=end(); it++){
	// 		if (it->second->contains_point(pt)){
	// 			id = it->first;
	// 			break;
	// 		}
	// 	}
	// 	return id;
	// }

	Identifier query_point(const Point<2> & pt) const {
		Identifier id = mBackground;
		for (auto it=begin(); it!=end(); it++){
			if (it->second->contains_point(pt)){
				id = it->first;
				break;
			}
		}
		return id;
	}

	Identifier query_point(const Point<3> & pt) const {
		Identifier id = mBackground;
		for (auto it=begin(); it!=end(); it++){
			if (it->second->contains_point(pt)){
				id = it->first;
				break;
			}
		}
		return id;
	}

	template <typename CastableType>
	std::pair<iterator, bool> insert(const Identifier & id, const CastableType & p){
		auto castable_shared = std::make_shared<CastableType>(p);
		return insert(make_pair(id, castable_shared));
	}


protected:
	Identifier 			mBackground;
};

} // end namespace csg
#endif