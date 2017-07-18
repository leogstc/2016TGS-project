#pragma once
#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "enemyBase.h"
#include "../util/timer.h"
namespace enemy
{
	class Enemy_Boss :public EnemyBase
	{
		enum class ATKMode
		{
			MINE,
			GENERA
		};

		enum class State
		{
			STAND = 0,
			WALK = 1,
			START_Q = 2,
			ATKSHOT = 3,
			QUAKE = 4,
			ATKMINE = 5,
			ENDMINE = 6,
			ATKG = 7,
			DEAD = 8
		};

		int atkpatten[10] = { 2,2,2,2,2,2,3,3,3,3 };

		State state_;
		ATKMode Atkmode_;
		ci_ext::Vec3f tpos;
		std::weak_ptr<Object> create_timer_;	//敵生成タイマー
		void setState(State state = State::STAND);
		void setAtkMode(ATKMode atkmode = ATKMode::MINE);
		const int SPRITE_SIZEX;
		const int SPRITE_SIZEY;
		const int MAX_CREATE_ENEMY;
		const int MAX_SHOT;
		int quakeX;
		int quakeY;
		int quakecnt;
		int genX;
		int cnt;
		int Gid_;
		int shotid;
		int atkcnt;
		int effcnt;
		int e_max;
		float speed;
		float maxHp;
	public:
		Enemy_Boss(int x, int y, int id);
		~Enemy_Boss();

		void init() override;
		void update() override;
		void render() override;

		void updateStand();
		void updateWalk();
		void updateStartQuake();
		void updateQuake();
		void updateMine();
		void updateMineEnd();
		void updateAtkG();
		void updateAtkShot();
		void updateDead();

		void ReverseRt();

		void setStateAnim(State state);

		ci_ext::Vec2f getPos() const;
		DIR getDir() const;
		float getHp() { return life_; };
		float getMaxHp() { return maxHp; };
	};
}
