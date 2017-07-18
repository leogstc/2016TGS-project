#pragma once
#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "../player/ShotBase.h"

namespace shot
{
	class EnemyMine : public ShotBase
	{
		ci_ext::Vec3f tpos;
		int no_;

	public:
		EnemyMine(float dmg,int no);
		~EnemyMine();
		void init() override;
		void render() override;
		void update() override;
		//void updatefire();
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