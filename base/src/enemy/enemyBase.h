#pragma once

#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "../player/player.h"
#include "mask.hpp"
#include <string>
namespace enemy {
	enum DIR {
		LEFT = -1,
		RIGHT = 1,
	};

	class EnemyBase : public map::MapCollisionObj {
	protected:
		int id_;
		float life_;
		float dmg_;
		RECT offsetRt_;
		RECT defsetRt_;
		RECT searchRt_;
		anim::Anim anim_;
		DIR dir_;
		std::weak_ptr<player::Player> player_;
		std::weak_ptr<Mask> mask_;
	private:
		bool notCheckHit_; //プレイヤーと判定しないか？
		bool isbound_;
	public:
		EnemyBase(const std::string& objectName, const std::string& resname, int x, int y, int id,float life, float dmg = 0.0f, RECT offsetRt = RECT(), RECT defsetRt = RECT(), RECT searchRt = RECT());
		EnemyBase(const std::string& objectName, int x, int y, int id);
		virtual ~EnemyBase() {}

		void init() override;
		void update() override;
		void render() override;
		void damage(float damagePt,/*const std::string& shottype,*/ci_ext::Vec3f v);
		int getId() const;
		RECT getdefRECT();
	private:
		inline void getPlayerPtr();

	protected:
		void reverseDir();
		bool checkHitPlayer();
		bool isSearchPlayer();//playerがsearchRt_内にいるか調べる
		void checkhitAtk();
		void makedefRect(int defsetSx, int defsetSy, int defsetEx, int defsetEy);
		void checkMask();
	};
}