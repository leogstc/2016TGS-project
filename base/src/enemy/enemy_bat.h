#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"

namespace enemy
{
	class Enemy_Bat :public EnemyBase
	{
		enum class State
		{
			FLY, ATTACK
		};

		float movespeed;
		float degree;
		State state_;
		void setState(State state = State::FLY);
		const int SPRITE_SIZEX;
		const int SPRITE_SIZEY;
		ci_ext::Vec3f tpos;
		int cnt;
		int atkcnt;
		void ReverseRt();
	public:
		Enemy_Bat(int x, int y, int id);
		~Enemy_Bat();

		void init() override;
		void update() override;
		void render() override;

		void updateFly();
		void updateAtk();

		ci_ext::Vec2f getPos() const;
		DIR getDir() const;
	};
}
