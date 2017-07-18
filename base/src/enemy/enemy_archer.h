#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"

namespace enemy
{
	class Enemy_Archer :public EnemyBase
	{
		enum class State
		{
			IDLE, ATTACK,FALL
		};

		float movespeed;
		float degree;
		State state_;
		void setState(State state = State::FALL);
		const float SPRITE_SIZEX;
		const float SPRITE_SIZEY;
		const int SEARCHRANGE_X = 1000;
		const int SEARCHRANGE_Y = 250;
		const float enemyScale = 1.0f;
		ci_ext::Vec3f tpos;
		int cnt;
		int atkcnt;
		void ReverseRt();
		bool isInitState;
		bool isNowFiring;
	public:
		Enemy_Archer(int x, int y, int id);
		~Enemy_Archer();

		void init() override;
		void update() override;
		void render() override;

		void updateIdle();
		void updateAtk();
		void updateFall();
		void checkPlayerDirection();

		ci_ext::Vec2f getPos() const;
		DIR getDir() const;
	};
}
