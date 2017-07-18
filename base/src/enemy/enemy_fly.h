#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"
//#include "../player/player.h"

namespace enemy
{
	
	class Enemy_Fly :public EnemyBase
	{
		enum class State
		{
			FLY, ATTACK, CLIMB
		};
		int dir_;
		State state_;
		void setState(State state = State::FLY);
		const int SPRITE_SIZEX;
		const int SPRITE_SIZEY;
		float movespeed_x;
		float movespeed_y;
		float atkspeed;
		float climbspeed;
		int atkcnt;
		int flycnt;
		ci_ext::Vec3f tpos;
		float degree;
		float standY;

	public:
		Enemy_Fly(int x, int y, int id);
		~Enemy_Fly();

		void init() override;
		void update() override;
		void render() override;
		void updateFly();
		void updateAttack();
		void updateClimb();
		void setDir(int dir);
		void ReverseDir();

	};
}
