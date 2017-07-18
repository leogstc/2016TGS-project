#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"

namespace enemy
{
	class Enemy_Monster :public EnemyBase
	{
		enum State { FALL, WALK };
		float movespeed_x;
		const int SPRITE_SIZEX;
		const int SPRITE_SIZEY;
		//anim::Anim anim;
		//int dir_;
		int state_;
		//RECT rt_;
		float createY_;
		bool nonCheckMap_;
	public:
		Enemy_Monster(int x,int y,int id/*ci_ext::Vec3f pos*/);
			~Enemy_Monster();

			//void init() override;
			void update() override;
			void render() override;
			void updateWalk();
			void updateFall();
			//void setDir(int dir);
			void setState(int state);
			//void reversalDir();
			//void getMapCollisionRect();
	};
}

