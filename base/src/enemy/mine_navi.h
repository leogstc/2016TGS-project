#pragma once
#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "../player/ShotBase.h"
#include "../util/timer.h"
#include "enemy_boss.h"
namespace shot
{
	class MineNavi : public ShotBase
	{
		ci_ext::Vec3f tpos;
		std::weak_ptr<Object> create_timer_; //timer
		std::weak_ptr<enemy::Enemy_Boss> enemyboss_; 
		int minecnt;
		int no;
	public:
		MineNavi();
		~MineNavi();
		void init() override;
		void render() override;
		void update() override;
		void updatenavi();
		void MineCreate();
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
