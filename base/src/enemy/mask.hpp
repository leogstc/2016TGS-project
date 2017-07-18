#pragma once
#include<string>
#include "../object/movableObject.hpp"
#include "../config.h"
namespace enemy
{
	class Mask :public game::MovableObject
	{
		bool show_;
		int cnt_;
	public:
		Mask(const std::string& resname, const std::string& objname,ci_ext::DrawObjf& f)
			:
			game::MovableObject(objname,f,0.f,0.f)
		{
			body_.setResname(resname);
			show_ = false;
		}

		void maskON(ci_ext::DrawObjf& f)
		{
			show_ = true;
			cnt_ = 0;
			body_.setPos(f.x(),f.y(),0.f);
			body_.setSrc(f.src());
			body_.setScale(f.scale());
			body_.setSrcSize(f.srcSize());
		}

		void update(ci_ext::DrawObjf& f)
		{
			body_.setPos(f.x(), f.y(), 0.f);
			body_.setSrc(f.src());
			body_.setScale(f.scale());
			body_.setSrcSize(f.srcSize());
			
			if (++cnt_ == 2)
				show_ = false;
		}

		void render() override
		{
			if (show_)
				body_.render();
		}

		bool isMaskShow()
		{
			return show_;
		}

	};
}