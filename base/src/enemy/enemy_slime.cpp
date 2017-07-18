#include "enemy_slime.h"
#include "../config.h"
#include "../stage/map.h"
namespace enemy
{
	Enemy_Slime::Enemy_Slime(int x, int y, int id)
		:
		EnemyBase("enemy_slime", "enemy_slime", x, y, id, 8,5),
		SPRITE_SIZE(90.f),
		jumpflag(true),
		attack_mode(false)
	{
		state_ = State::STAND;
		dir_ = DIR::LEFT;
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y()+10.f,0.8f));
		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZE, SPRITE_SIZE));
		body_.setSrc(ci_ext::Vec2f(0.f, SPRITE_SIZE));
		anim_.setAnim(5, 13);
		colrt.x = 10;
		colrt.y = 0;
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));
	}

	void Enemy_Slime::render()
	{
#ifdef _DEBUG
		gplib::draw::CkRect(defsetRt_);
		gplib::draw::CkRect(offsetRt_, ARGB(255, 255, 255, 0));
		gplib::draw::CkRect(searchRt_,ARGB(255,255,0,255));
#endif // DEBUG
		body_.render();
	}

	void Enemy_Slime::update()
	{
		updateRect();
		velocity_.offset(ci_ext::Vec3f(0.f, config.GRAVITY, 0.f));
			switch (state_)
			{
			case State::STAND:		Enemy_Slime::updateStand();
				break;
			case State::STARTJUMP:	Enemy_Slime::startJump();
				break;
			case State::JUMP:		Enemy_Slime::updateJamp();
				break;
			case State::FALL:		Enemy_Slime::updateFall();
				break;
			}
		body_.setScale(ci_ext::Vec2f((float)dir_, 1.f));
		mapCheckMoveXY();
		checkMask();
		checkhitAtk();
		checkHitPlayer();
	}
	void Enemy_Slime::updateStand()
	{
		//アニメ切り替え
		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), SPRITE_SIZE));
		
		if (attack_mode)
		{
			if (anim_.isLastFrame())
			{
				anim_.resetNo();
				state_ = State::STARTJUMP;
			}
		}
		else
		{
			if (isSearchPlayer())
				attack_mode = true;
		}
	}
	void Enemy_Slime::startJump()
	{
		//アニメ切り替え
		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), SPRITE_SIZE*2.f));
		//ジャンプするコマになったらジャンプさせる
		if (isJumpAnim())
		{
			float x;
			if (dir_ == DIR::LEFT)
				x = -2.f;
			else
				x = 2.f;

			velocity_.set(x, -10.f);
			state_ = State::JUMP;
			gplib::se::Play("slimejump");
			colrt.y -= 10;
		}
	}
	void Enemy_Slime::updateJamp()
	{
		//velocity_.offset(ci_ext::Vec3f(0.f, config.GRAVITY, 0.f));
		//if (checkCollisionMapX())
		//	velocity_.x(velocity_.x()*-1.f);
		//body_.offsetPos(velocity_);
		//if (collisionMapRight(0, 0, true) != 0)
		//	reverseDir();
		//if (checkRightMap()||checkLeftMap()) {
		//	reverseDir();
		//}
		//if (checkMoveMapX() != -1) {
		//	reverseDir();
		//}
		if (velocity_.y() >= 0.f)
		{
			state_ = State::FALL;
			body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * 3, SPRITE_SIZE*2.f));
			colrt.y = 0;
		}
	}
	void Enemy_Slime::updateFall()
	{
		//velocity_.offset(ci_ext::Vec3f(0.f, config.GRAVITY, 0.f));
		if (checkUnderFloor())
		{
			velocity_.set(0.f, 0.f);
			anim_.resetNo();
			setDir();
			state_=State::STAND;
		}
	}
	void Enemy_Slime::updateRect()
	{
		defsetRt_.left = (body_.ix() - (int)SPRITE_SIZE / 4) + colrt.x;
		defsetRt_.right = (body_.ix() + (int)SPRITE_SIZE / 4) + colrt.x;
		defsetRt_.top = (body_.iy() - (int)SPRITE_SIZE / 4) + colrt.y;
		defsetRt_.bottom = (body_.iy() + (int)SPRITE_SIZE / 4) + colrt.y;
		offsetRt_ = defsetRt_;
		setObjCollisionMapRect(defsetRt_);

		
		searchRt_.left = body_.ix() - 150;
		searchRt_.right = body_.ix() + 150;
		searchRt_.top = body_.iy() - 50;
		searchRt_.bottom = body_.iy() + 50;
	}

	void Enemy_Slime::setDir()
	{
		//プレイヤーがいる方向を向く
		if (player_.lock() != nullptr)
		{
			ci_ext::Vec2f Ppos = player_.lock()->getPos();
			if (Ppos.x() < body_.x())
			{
				dir_ = DIR::LEFT;
			}
			else
			{
				dir_ = DIR::RIGHT;
			}
		}
	}

	bool Enemy_Slime::isJumpAnim()
	{
		return anim_.getAnimNo() == 2;
	}

}