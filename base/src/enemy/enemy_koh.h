#pragma once

#include "../lib/ci_ext/DrawObj.hpp"
#include "../lib/ci_ext/object.hpp"
#include "enemyBase.h"

namespace enemy
{
	namespace sprite {
		enum {
			SPRITE_SIZE = 128,

			OFFSET_STAND_TOP = 43,
			OFFSET_STAND_BOTTOM = 8,
			OFFSET_STAND_LEFT = 47,
			OFFSET_STAND_RIGHT = 49,

			OFFSET_JUMP_TOP1 = 43,
			OFFSET_JUMP_BOTTOM1 = 8,
			OFFSET_JUMP_LEFT1 = 47,
			OFFSET_JUMP_RIGHT1 = 49,

			OFFSET_JUMP_TOP2 = 43,
			OFFSET_JUMP_BOTTOM2 = 8,
			OFFSET_JUMP_LEFT2 = 47,
			OFFSET_JUMP_RIGHT2 = 49,

			OFFSET_JUMP_TOP3 = 55,
			OFFSET_JUMP_BOTTOM3 = 8,
			OFFSET_JUMP_LEFT3 = 47,
			OFFSET_JUMP_RIGHT3 = 49,

			OFFSET_JUMP_TOP4 = 43,
			OFFSET_JUMP_BOTTOM4 = 8,
			OFFSET_JUMP_LEFT4 = 47,
			OFFSET_JUMP_RIGHT4 = 49,

			OFFSET_JUMP_TOP5 = 30,
			OFFSET_JUMP_BOTTOM5 = 8,
			OFFSET_JUMP_LEFT5 = 47,
			OFFSET_JUMP_RIGHT5 = 49,

			OFFSET_FALL_TOP1 = 40,
			OFFSET_FALL_BOTTOM1 = 8,
			OFFSET_FALL_LEFT1 = 50,
			OFFSET_FALL_RIGHT1 = 46,

			OFFSET_FALL_TOP2 = 59,
			OFFSET_FALL_BOTTOM2 = 8,
			OFFSET_FALL_LEFT2 = 53,
			OFFSET_FALL_RIGHT2 = 40,

			OFFSET_FALL_TOP3 = 68,
			OFFSET_FALL_BOTTOM3 = 8,
			OFFSET_FALL_LEFT3 = 53,
			OFFSET_FALL_RIGHT3 = 40,

			OFFSET_FALL_TOP4 = 73,
			OFFSET_FALL_BOTTOM4 = 8,
			OFFSET_FALL_LEFT4 = 53,
			OFFSET_FALL_RIGHT4 = 40,

			OFFSET_FALL_TOP5 = 66,
			OFFSET_FALL_BOTTOM5 = 8,
			OFFSET_FALL_LEFT5 = 53,
			OFFSET_FALL_RIGHT5 = 40,

			OFFSET_ATK_C_TOP1 = 43,
			OFFSET_ATK_C_BOTTOM1 = 8,
			OFFSET_ATK_C_LEFT1 = 47,
			OFFSET_ATK_C_RIGHT1 = 49,

			OFFSET_ATK_C_TOP2 = 43,
			OFFSET_ATK_C_BOTTOM2 = 8,
			OFFSET_ATK_C_LEFT2 = 47,
			OFFSET_ATK_C_RIGHT2 = 49,

			OFFSET_ATK_C_TOP3 = 43,
			OFFSET_ATK_C_BOTTOM3 = 8,
			OFFSET_ATK_C_LEFT3 = 47,
			OFFSET_ATK_C_RIGHT3 = 42,

			OFFSET_ATK_C_TOP4 = 43,
			OFFSET_ATK_C_BOTTOM4 = 8,
			OFFSET_ATK_C_LEFT4 = 47,
			OFFSET_ATK_C_RIGHT4 = 42,

			OFFSET_ATK_C_TOP5 = 43,
			OFFSET_ATK_C_BOTTOM5 = 8,
			OFFSET_ATK_C_LEFT5 = 47,
			OFFSET_ATK_C_RIGHT5 = 42,

			OFFSET_ATK_C_TOP6 = 43,
			OFFSET_ATK_C_BOTTOM6 = 8,
			OFFSET_ATK_C_LEFT6 = 47,
			OFFSET_ATK_C_RIGHT6 = 49,

			OFFSET_ATK_C_TOP7 = 43,
			OFFSET_ATK_C_BOTTOM7 = 8,
			OFFSET_ATK_C_LEFT7 = 47,
			OFFSET_ATK_C_RIGHT7 = 49,

			OFFSET_FLY_TOP = 45,
			OFFSET_FLY_BOTTOM = 36,
			OFFSET_FLY_LEFT = 42,
			OFFSET_FLY_RIGHT = 24,


			//Map Collision -> MINimum offset
			MAP_COL_OFFSET_STAND_TOP = OFFSET_STAND_TOP,
			MAP_COL_OFFSET_STAND_BOTTOM = OFFSET_STAND_BOTTOM,
			MAP_COL_OFFSET_STAND_LEFT = OFFSET_STAND_LEFT,
			MAP_COL_OFFSET_STAND_RIGHT = OFFSET_STAND_RIGHT,

			MAP_COL_OFFSET_JUMP_TOP = OFFSET_JUMP_TOP5,
			MAP_COL_OFFSET_JUMP_BOTTOM = OFFSET_JUMP_BOTTOM5,
			MAP_COL_OFFSET_JUMP_LEFT = OFFSET_JUMP_LEFT5,
			MAP_COL_OFFSET_JUMP_RIGHT = OFFSET_JUMP_RIGHT5,

			MAP_COL_OFFSET_FALL_TOP = OFFSET_FALL_TOP1,
			MAP_COL_OFFSET_FALL_BOTTOM = OFFSET_FALL_BOTTOM1,
			MAP_COL_OFFSET_FALL_LEFT = OFFSET_FALL_LEFT1,
			MAP_COL_OFFSET_FALL_RIGHT = OFFSET_FALL_RIGHT1,

			MAP_COL_OFFSET_FLY_TOP = OFFSET_FLY_TOP,
			MAP_COL_OFFSET_FLY_BOTTOM = OFFSET_FLY_BOTTOM,
			MAP_COL_OFFSET_FLY_LEFT = OFFSET_FLY_LEFT,
			MAP_COL_OFFSET_FLY_RIGHT = OFFSET_FLY_RIGHT,

			//アニメーションフレーム
			ANIM_STAND = 10,
			ANIM_WALK = 8,
			ANIM_JUMP = 5,
			ANIM_FALL = 5,
			ANIM_ATK_C = 7,
			ANIM_ATK_M = 13,
			ANIM_START_FLY = 3,
			ANIM_FLY = 6,
			ANIM_HIT = 3,

			//アニメーション速度（フレーム）
			ANIM_SPD_STAND = 6,
			ANIM_SPD_WALK = 6,
			ANIM_SPD_JUMP = 3,
			ANIM_SPD_FALL = 1,
			ANIM_SPD_ATK_C = 3,
			ANIM_SPD_ATK_M = 6,
			ANIM_SPD_START_FLY = 3,
			ANIM_SPD_FLY = 6,
			ANIM_SPD_HIT = 6,

			//攻撃できるアニメーションフレーム
			//2^(nフレーム目) | 2^(nフレーム目) | ...
			//nは0からスタート
			ATK_C_AVAILABLE = 0x04 | 0x08 | 0x10,
		};
	}

	enum  class State
	{
		STAND = 0,
		RWALK = 1,
		LWALK = 2,
		FALL = 3,
	};

	class EnemyKoh :public EnemyBase
	{
		State state_;
		void setState(State state = State::STAND);

	public:
		EnemyKoh (int x, int y, int id);
		~EnemyKoh();

		void init() override;
		void update() override;
		void render() override;

		void updateStand();
		void updateLWalk();
		void updateRWalk();
		//void updateFall();
		void checkHit();
		//bool transobj();

		RECT getMapOffset();
		RECT getMapCollisionRect();

		ci_ext::Vec2f getpos();

		template <typename T = int>
		void swapLRbyScale(T& a, T& b, T aVal, T bVal) 
		{
			if (body_.scale().x() < 0) 
			{
				a = bVal;
				b = aVal;
			}
			else
			{
				a = aVal;
				b = bVal;
			}
		}
	};
}
