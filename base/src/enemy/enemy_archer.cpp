#include "enemy_archer.h"
#include "../config.h"
#include "../stage/map.h"
#include "../player/player.h"
#include "enemy_arrow.h"
namespace enemy
{
	Enemy_Archer::Enemy_Archer(int x, int y, int id)
		:
		EnemyBase("enemy_archer", "enemy_archer", x, y, id, 9,20), SPRITE_SIZEX(150.f), SPRITE_SIZEY(110.0f)
	{
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y(), 0.8));
	}

	Enemy_Archer::~Enemy_Archer()
	{}

	void Enemy_Archer::init()
	{
		auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		nowInMapName_ = map.lock()->getNowMapName();

		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZEX, SPRITE_SIZEY));
		body_.setScale(ci_ext::Vec2f(enemyScale, enemyScale));
		movespeed = 5.f;
		degree = 0.0f;
		dir_ = DIR::LEFT;
		anim_.setAnim(7, 7);
		cnt = 0;
		atkcnt = 0;
		isInitState = false;
		setState(State::FALL);
		isNowFiring = false;
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));

	}

	void Enemy_Archer::update()
	{

		//あたり判定
		//offence (Yellow)
		defsetRt_.top = (long)(body_.y() - ((SPRITE_SIZEY / 2.0f) - 12.0f)* enemyScale);
		defsetRt_.bottom = (long)(body_.y() + ((SPRITE_SIZEY / 2.0f) - 12.0f)* enemyScale);

		//defence (Red)
		offsetRt_.top = (long)(body_.y() - ((SPRITE_SIZEY / 2.0f) - 12.0f)* enemyScale);
		offsetRt_.bottom = (long)(body_.y() + ((SPRITE_SIZEY / 2.0f) - 12.0f)* enemyScale);

		//サーチ
		searchRt_.top = body_.iy() - 250;
		searchRt_.bottom = body_.iy() + 250;

		if (dir_ == LEFT)
		{
			defsetRt_.left = (long)(body_.x() - ((SPRITE_SIZEY / 2.0f) + 15.0f) * enemyScale);
			defsetRt_.right = (long)(body_.x() + ((SPRITE_SIZEY / 2.0f) - 15.0f) * enemyScale);

			offsetRt_.left = (long)(body_.x() - (SPRITE_SIZEY / 2.0f - 20.0f) * enemyScale);
			offsetRt_.right = (long)(body_.x() + (SPRITE_SIZEY / 2.0f - 20.0f) * enemyScale);

			searchRt_.left = body_.ix() - SEARCHRANGE_X;
			searchRt_.right = body_.ix() + SEARCHRANGE_Y;

		}

		setObjCollisionMapRect(offsetRt_);

		//アニメ
		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		//方向	
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE*enemyScale, enemyScale));


		switch (state_)
		{
		case State::IDLE:updateIdle(); break;
		case State::ATTACK:updateAtk(); break;
		case State::FALL:updateFall(); break;
		}

		body_.offsetPos(velocity_);

		checkMask();
		checkPlayerDirection();
		checkHitPlayer();
		checkhitAtk();
		
	}

	void Enemy_Archer::checkPlayerDirection()
	{

		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));

		if (player != nullptr)
		{
			//Face Left or Right
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
		}
	}

	void Enemy_Archer::updateIdle()
	{
		if (!isInitState)
		{
			anim_.resume();
			anim_.setAnimNo(0);
			anim_.setAnim(7, 10);
			body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), (int)state_ * SPRITE_SIZEY));
			isInitState = true;
			isNowFiring = false;
		}

		if (!checkUnderFloor())
		{
			velocity_.offset(ci_ext::Vec3f(0, config.GRAVITY, 0));
			setState(State::FALL);
		}
		else 
		{
			velocity_.y(0.0f);
		}
		
		if (anim_.isLastFrame())
		{
			anim_.setAnim(7, 10);
			anim_.setAnimNo(4);

		}

		if (isSearchPlayer())
		{
			if (++atkcnt > 140)
			{
				setState(State::ATTACK);
				atkcnt = 0;
				isInitState = false;
				gplib::se::Play("arrow");
			}
		}

	}

	void Enemy_Archer::updateAtk()
	{
		if (!isInitState)
		{
			anim_.setAnimNo(0);
			isInitState = true;
		}

		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), (int)state_ * SPRITE_SIZEY));
		anim_.setAnim(2, 20);
		if (anim_.getAnimNo() == 1)
		{
			anim_.pause();
			cnt++;
		}
		if(cnt >= 50)
		{
			cnt = 0;
			setState(State::IDLE);
			isInitState = false;
		}

		if (!isNowFiring)
		{
			insertToParent(new shot::EnemyArrow(20, body_.pos()));
			//insertAsChild(new shot::EnemyArrow(20.0f));
			isNowFiring = true;
		}
		//setState(State::IDLE);
	}

	void Enemy_Archer::updateFall()
	{
		if (!isInitState)
		{
			anim_.setAnimNo(0);
			anim_.setAnim(1, 10);
			body_.setSrc(ci_ext::Vec2f(0, (int)state_ * SPRITE_SIZEY));
			isInitState = true;
		}
		if (!checkUnderFloor())
		{
			body_.setSrc(ci_ext::Vec2f(0, (int)state_ * SPRITE_SIZEY));
			velocity_.offset(ci_ext::Vec3f(0, config.GRAVITY, 0));
		}
		else
		{
			velocity_.y(0.0f);
			setState(State::IDLE);
			isInitState = false;
		}
	}


	void Enemy_Archer::ReverseRt()
	{
		defsetRt_.left = (long)(body_.x() - ((SPRITE_SIZEY / 2.0f) - 15.0f) * enemyScale);
		defsetRt_.right = (long)(body_.x() + ((SPRITE_SIZEY / 2.0f) + 15.0f) * enemyScale);

		searchRt_.left = body_.ix() - 100;
		searchRt_.right = body_.ix() + SEARCHRANGE_X;

	}

	void Enemy_Archer::render()
	{
		body_.render();

#ifdef _DEBUG
		gplib::draw::CkRect(offsetRt_, ARGB(255, 255, 255, 0));
		gplib::draw::CkRect(searchRt_, ARGB(255, 255, 0, 255));
		gplib::draw::CkRect(defsetRt_, ARGB(255, 255, 0, 0));

		/*gplib::font::TextNC(250, 500, 0.0f, "Now Anim  : " + std::to_string(anim_.getAnimNo()), ARGB(255, 0, 0, 255), 0);
		gplib::font::TextNC(250, 520, 0.0f, "State  : " + std::to_string((int)state_), ARGB(255, 0, 0, 255), 0);*/
#endif 

	}

	ci_ext::Vec2f Enemy_Archer::getPos() const
	{
		return{ body_.pos().x(), body_.pos().y() };
	}

	DIR Enemy_Archer::getDir() const
	{
		return dir_;
	}

	void Enemy_Archer::setState(State state)
	{
		state_ = state;
	}
}