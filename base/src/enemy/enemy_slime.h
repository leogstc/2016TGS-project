#pragma once
#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"

namespace enemy
{
	class Enemy_Slime : public EnemyBase
	{
		enum State{STAND,STARTJUMP,JUMP,FALL};
		struct CllisionRect
		{
			int x, y;
			RECT rt;
		}colrt;
		const float SPRITE_SIZE;
		int state_;
		bool jumpflag;
		bool attack_mode;
		bool isJumpAnim();
		void updateRect();
		void setDir();
	public:
		Enemy_Slime(int x, int y, int id);
		void render() override;
		void update() override;
		void updateStand();
		void updateJamp();
		void updateFall();
		void startJump();
	};
}