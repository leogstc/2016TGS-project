#pragma once
#include "../object/movableObject.hpp"
#include "../lib/ci_ext/DrawObj.hpp"
#include "tree.hpp"
#include <string>
namespace titleObj
{
	class Tree :public game::MovableObject
	{
	public:
			Tree(const std::string& resname)
			:
			MovableObject("tree",
			ci_ext::DrawObjf(ci_ext::Vec3f::zero(),
							 ci_ext::Vec2f::one(),
							 resname,
							 ci_ext::Vec2f::zero(),
							 ci_ext::Vec2f(282.f,301.f),
							 ci_ext::Color(255,255,255,255),
							 0.f),
				0.f//speed
			)
		{
			using namespace ci_ext;
			using namespace gplib;
			body_.setPos(Vec3f((float)system::WINW, (float)system::WINH - 62.f, 0.2f));
		}


		void update() override
		{
			body_.offsetPos(-1.f);
		}


	};
}