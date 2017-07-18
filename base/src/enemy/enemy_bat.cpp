#include "enemy_bat.h"
#include "../config.h"
//#include "../stage/map.h"
//#include "../player/player.h"
#include "enemy_shot.h"
namespace enemy
{
	Enemy_Bat::Enemy_Bat(int x, int y, int id)
		:
		EnemyBase("enemy_bat", "enemy_bat", x, y, id, 3, 10) , SPRITE_SIZEX(197), SPRITE_SIZEY(117)
	{
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y(), 0.8f));
	}

	Enemy_Bat::~Enemy_Bat()
	{}

	void Enemy_Bat::init()
	{
		//auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		//nowInMapName_ = map.lock()->getNowMapName();

		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZEX, SPRITE_SIZEY));
		body_.setScale(ci_ext::Vec2f(0.5f, 0.5f));
		movespeed = 5.f;
		degree = 0.0f;
		dir_ = DIR::LEFT;
		anim_.setAnim(4, 5);
		cnt = 0;
		atkcnt = 0;
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));

	}

	void Enemy_Bat::update()
	{
		//off
		offsetRt_.left = body_.ix() - 24;
		offsetRt_.right = body_.ix() + 10;
		offsetRt_.top = body_.iy() - 24;
		offsetRt_.bottom = body_.iy() + 10;
		setObjCollisionMapRect(offsetRt_);

		//def
		defsetRt_.left = body_.ix() - 197 / 4;
		defsetRt_.right = body_.ix() + 197 / 4;
		defsetRt_.top = body_.iy() - 117 / 5;
		defsetRt_.bottom = body_.iy() + 117 / 6;
		//makeRectScale(defsetRt_.left, defsetRt_.top, defsetRt_.right, defsetRt_.bottom);

		//サーチ
		searchRt_.left = body_.ix() - 400;
		searchRt_.right = body_.ix() + 400;
		searchRt_.top = body_.iy() - 150;
		searchRt_.bottom = body_.iy() + 150;

		//アニメ
		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		//方向	
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE*0.5f, 0.5f));


		switch (state_)
		{
		case State::FLY:updateFly(); break;
		case State::ATTACK:updateAtk(); break;
		}

		body_.offsetPos(velocity_);

		checkMask();
		checkHitPlayer();
		checkhitAtk();
	}

	void Enemy_Bat::updateFly()
	{
		velocity_.y(gplib::math::ROUND_X(degree, 2));
		degree += 10;

		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));
		if (player != nullptr)
		{
			tpos = player->getPos();
			if (tpos.x() < body_.x())
			{
				dir_ = DIR::LEFT;
			}
			else
			{
				dir_ = DIR::RIGHT;
				ReverseRt();
			}
			//RECT pRt = player->getBodyCollisionRect();
			if (isSearchPlayer())
			{
				if (++atkcnt > 140)
				{
					setState(State::ATTACK);
					atkcnt = 0;
				}
			}
		}
	}

	void Enemy_Bat::updateAtk()
	{
		insertToParent(new shot::EnemyShot(20));
		//insertAsChild(new shot::EnemyShot(20));
		setState(State::FLY);
	}

	void Enemy_Bat::ReverseRt()
	{
		offsetRt_.left = body_.ix() - 10;
		offsetRt_.right = body_.ix() + 24;
	}

	void Enemy_Bat::render()
	{
		body_.render();

#ifdef _DEBUG
		gplib::draw::CkRect(offsetRt_, ARGB(255, 255, 255, 0));
		gplib::draw::CkRect(searchRt_, ARGB(255, 255, 0, 255));
		gplib::draw::CkRect(defsetRt_, ARGB(255, 255, 0, 0));
#endif 

	}

	ci_ext::Vec2f Enemy_Bat::getPos() const
	{
		return{ body_.pos().x(), body_.pos().y() };
	}

	DIR Enemy_Bat::getDir() const
	{
		return dir_;
	}

	void Enemy_Bat::setState(State state)
	{
		state_ = state;
	}
}