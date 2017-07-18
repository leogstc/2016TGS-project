#include "enemy_monster.h"
#include "../config.h"
#include "../stage/map.h"
namespace enemy
{
	Enemy_Monster::Enemy_Monster(int x, int y, int id/*ci_ext::Vec3f pos*/)
		:
		enemy::EnemyBase("enemy_monster", "enemy_monster", x, y, id, 10, 15),
		SPRITE_SIZEX(76), SPRITE_SIZEY(82), nonCheckMap_(true)
	{
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y(), 0.8f));
		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZEX, SPRITE_SIZEY));
		//body_.setScale(ci_ext::Vec2f(0.8f, 0.8f));
		anim_.setAnim(6,15);
		movespeed_x = 1.f;
		dir_ = DIR::RIGHT;
		setState(State::FALL);
		createY_ = body_.pos().y();
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));
	}

	Enemy_Monster::~Enemy_Monster()
	{
		
	}

	void Enemy_Monster::render()
	{
#ifdef _DEBUG
		//状態
		std::string s;
		switch (state_) {
		default: s = "Unknown"; break;
		case State::WALK: s = "Walk"; break;
		case State::FALL: s = "Fall"; break;
		}
		gplib::font::TextNC(20, 100, 0.0f, s, ARGB(255, 255, 255, 255), 0);

		//座標
		gplib::font::TextNC(20, 120, 0.0f, std::to_string(body_.pos().x()) + "," + std::to_string(body_.pos().y()), ARGB(255, 255, 255, 255), 0);

		gplib::font::TextNC(20, 140, 0.0f,"left:"+ std::to_string(offsetRt_.left) + " right:" + std::to_string(offsetRt_.right), ARGB(255, 255, 255, 255), 0);
		gplib::font::TextNC(20, 160, 0.0f, "top:" + std::to_string(offsetRt_.top) + "   bottom:" + std::to_string(offsetRt_.bottom), ARGB(255, 255, 255, 255), 0);
		gplib::draw::CkRect(offsetRt_);
#endif
		body_.render();
		
	}

	void Enemy_Monster::update()
	{
		//あたり判定更新
		offsetRt_.left	= body_.ix() - (int)SPRITE_SIZEX / 4;
		offsetRt_.right	= body_.ix() + (int)SPRITE_SIZEX / 4;
		offsetRt_.top	= body_.iy() - (int)SPRITE_SIZEY / 3;
		offsetRt_.bottom = body_.iy() + (int)SPRITE_SIZEY / 4;
		setObjCollisionMapRect(offsetRt_);

		defsetRt_ = offsetRt_;

		//アニメーション
		anim_.step();
		
		//状態のアニメーション画像
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		switch (state_)
		{
		case State::WALK:	updateWalk();
			break;
		case State::FALL:	updateFall();
			break;
		}

		mapCheckMoveXY();
		

		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE, body_.scale().y()));

		checkMask();
		checkHitPlayer();
		checkhitAtk();
	}

	void Enemy_Monster::updateWalk()
	{
		switch (dir_)
		{
		case DIR::LEFT:		velocity_.x(-movespeed_x);
			break;
		case DIR::RIGHT:	velocity_.x(movespeed_x);
			break;
		}

		mapCheckCollisionX(true);

		if (checkMoveMapX() != -1) {
			reverseDir();
		}

		if (!checkUnderFloor())
			setState(State::FALL);
	}

	void Enemy_Monster::updateFall()
	{
		velocity_.offset(ci_ext::Vec3f(0, config.GRAVITY, 0));
		//velocity_.y(5);
		if (checkUnderFloor())
		{
			velocity_.y(0);
			setState(State::WALK);
		}
	}

	void Enemy_Monster::setState(int state)
	{
		state_ = state;
	}

}
