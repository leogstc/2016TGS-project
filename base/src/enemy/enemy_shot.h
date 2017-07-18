#pragma once
#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "../player/ShotBase.h"
#include "enemy_bat.h"

namespace shot
{
	class EnemyShot : public ShotBase
	{
		ci_ext::Vec3f tpos;
		std::weak_ptr<enemy::Enemy_Bat> enemybat_;
	public:
		EnemyShot(float dmg);
		~EnemyShot();
		void init() override;
		void render() override;
		void update() override;
		void updatefire();
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