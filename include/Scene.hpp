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


	template <typename CastableType>
	std::pair<iterator, bool> insert(const Identifier & id, std::shared_ptr<CastableType> & castable_shared){
		return insert(make_pair(id, castable_shared));
	}


	Identifier & background() {return mBackground;};
	const Identifier & background() const {return mBackground;};

	void print_summary(std::ostream & os = std::cout, unsigned int ntabs=0) const{
		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "<Scene>" << std::endl;
		for (auto i=0; i<ntabs+1; i++) os << "\t" ;
		os << "<Background>" << mBackground << "</Background>" << std::endl;
		
		for (auto it=begin(); it!=end(); it++){
			for (auto i=0; i<ntabs+1; i++) os << "\t" ;
			os << "<Object>" << std::endl;

			for (auto i=0; i<ntabs+2; i++) os << "\t" ;
			os << "<Identifier>" << it->first << "</Identifier>" << std::endl;

			for (auto i=0; i<ntabs+2; i++) os << "\t" ;
			os << "<Primitive>" << std::endl;

			it->second->print_summary(os, ntabs+3);

			for (auto i=0; i<ntabs+2; i++) os << "\t" ;
			os << "</Primitive>" << std::endl;


			for (auto i=0; i<ntabs+1; i++) os << "\t" ;
			os << "</Object>" << std::endl;
		}


		for (auto i=0; i<ntabs; i++) os << "\t" ;
		os << "</Scene>" << std::endl;
	}

protected:
	Identifier 			mBackground;
};

} // end namespace csg
#endif