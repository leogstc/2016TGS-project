#pragma once
#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "../player/ShotBase.h"
#include "enemy_boss.h"
namespace shot
{
	class BossShot : public ShotBase
	{
		ci_ext::Vec3f tpos;
		std::weak_ptr<enemy::Enemy_Boss> enemyboss_;
		int cnt;
	public:
		BossShot(int x, int y, float dmg);
		~BossShot();
		void init() override;
		void render() override;
		void update() override;
		void updatefire();
		void reverseDir();
		bool isShot;
		float atkspeed;

		template <typename T = int>
		void swapLRbyScale(T& a, T& b, T aVal, T bVal)
		{
			a = aVal;
			b = bVal;
		}

		RECT getShotMapOffset();
		RECT getShotMapCollisionRect();
	};
}