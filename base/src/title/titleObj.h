#pragma once
#include "../object/movableObject.hpp"
#include "../lib/ci_ext/DrawObj.hpp"
#include <string>

namespace titleObj
{
	class TitleObj :public game::MovableObject
	{
		float posy_;
		float posz_;
	public:
		TitleObj(const std::string& objname, const std::string& resname,const ci_ext::Vec3f& pos, float offsetY,float speed);
		void update() override;
		bool isOutScreen();
	};
}