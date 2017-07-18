#pragma once

#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"

namespace shot
{
	enum DIR {
		LEFT = -1,
		RIGHT = 1,
	};

	class ShotBase :public map::MapCollisionObj
	{
	protected:
		int id_;
		anim::Anim anim_;
		RECT offsetRt_;
		float dmg_;
		DIR dir_;
	public:
		ShotBase(const std::string& objectName, const std::string& resname, float dmg = 0.0f, RECT offsetRt = RECT());
		ShotBase(const std::string& objectName, const std::string& resname, RECT offsetRt = RECT());
		~ShotBase();

		void init() override;
		void update() override;
		void render() override;

		virtual void setupDrawObj();
		virtual void setupAnim();

	protected:
		bool checkHitPlayer();
	};
}
