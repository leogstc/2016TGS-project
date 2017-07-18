#include "enemy_fly.h"
#include "../config.h"
//#include "../stage/map.h"
namespace enemy
{
	Enemy_Fly::Enemy_Fly(int x, int y, int id)
		:
		EnemyBase("enemy_fly", "enemy_bat", x, y, id, 3, 10) , SPRITE_SIZEX(197), SPRITE_SIZEY(117)
	{
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y(), 0.8));
	}

	Enemy_Fly::~Enemy_Fly()
	{}

	void Enemy_Fly::init()
	{
		//auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		//nowInMapName_ = map.lock()->getNowMapName();
		player_= ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZEX, SPRITE_SIZEY));
		body_.setScale(ci_ext::Vec2f(0.5f, 0.5f));
		setState(State::FLY);
		setDir(DIR::RIGHT);
		anim_.setAnim(4, 5);
		movespeed_x = 2.f;
		movespeed_y = 7.f;
		atkspeed = 9.f;
		climbspeed = 6.f;
		atkcnt = 0;
		flycnt = 0;
		degree = 0.0f;
		standY = body_.y();
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));
	}

	void Enemy_Fly::update()
	{
		//off
		offsetRt_.left = body_.ix() - 64 / 4;
		offsetRt_.right = body_.ix() + 64 / 4;
		offsetRt_.top = body_.iy() - 64 / 4;
		offsetRt_.bottom = body_.iy() + 64 / 4;
		setObjCollisionMapRect(offsetRt_);
		
		//def
		defsetRt_.left = body_.ix() - 197 / 4;
		defsetRt_.right = body_.ix() + 197 / 4;
		defsetRt_.top = body_.iy() - 117 / 5;
		defsetRt_.bottom = body_.iy() + 117 / 6;

		//ƒT[ƒ`
		searchRt_.left = body_.ix() - 200;
		searchRt_.right = body_.ix() + 200;
		searchRt_.top = body_.iy() - 150;
		searchRt_.bottom = body_.iy() + 400;

		switch (state_)
		{
		case State::FLY:updateFly(); break;
		case State::ATTACK:updateAttack(); break;
		case State::CLIMB:updateClimb(); break;
		}

		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		//•ûŒü	
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE*0.5f, 0.5f));

		//body_.offsetPos(ci_ext::Vec3f(2.f));
		//mapCheckMoveXY();
		body_.offsetPos(velocity_);

		checkMask();
		checkHitPlayer();
		checkhitAtk();
	}

	void Enemy_Fly::updateFly()  //ˆê”Êó‘Ô
	{
		switch (dir_)
		{
		case DIR::LEFT:
			velocity_.x(-movespeed_x); 
			velocity_.y(gplib::math::ROUND_X(degree,2));
			degree += 10;
			break;
		case DIR::RIGHT:
			velocity_.x(movespeed_x); 
			velocity_.y(gplib::math::ROUND_X(degree, 2));
			degree += 10;
			break;
		}

		if (++flycnt > 90)
		{
			ReverseDir();
			flycnt = 0;
		}

		if (!player_.expired())
		{
			//RECT pRt = player->getBodyCollisionRect();
			if (isSearchPlayer())
			{
				anim_.setAnim(4, 1);
				if (++atkcnt > 90)
				{
					setState(State::ATTACK);
					tpos = player_.lock()->getPos();

					if (body_.x() > tpos.x())
					{
						dir_ = DIR::LEFT;
						velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
						velocity_.y(gplib::math::ROUND_Y(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));

						atkcnt = 0;
					}
					else
					{
						dir_ = DIR::RIGHT;
						velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
						velocity_.y(gplib::math::ROUND_Y(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));

						atkcnt = 0;
					}
				}
			}
			else
			{
				anim_.setAnim(4, 5);
				atkcnt = 0;
			}
		}
		body_.offsetPos(velocity_);
	}

	void Enemy_Fly::updateAttack()
	{

		if (body_.y() >= tpos.y())
		{
			setState(State::CLIMB);
			velocity_.set(0, 0);
		}

	}

	void Enemy_Fly::updateClimb()
	{
		
		if (body_.x() > tpos.x())
		{
			dir_ = DIR::RIGHT;
		}
		else
		{
			dir_ = DIR::LEFT;
		}

		if (body_.y() >= tpos.y() - 250)
		{
			velocity_.y(-climbspeed);
		}
		else if(body_.y()<=standY)
		{
			velocity_.y(0);
			velocity_.x(4);
			setState(State::FLY);
		}
	}

	void Enemy_Fly::setState(State state)
	{
		state_ = state;
	}

	void Enemy_Fly::render()
	{
		body_.render();

#ifdef _DEBUG
		std::string msg("State " + std::to_string((int)state_));
		gplib::font::TextNC(500, 100, 0.f, msg, ARGB(255, 255, 0, 0),0);
		std::string msg2("Tpos x " + std::to_string(tpos.x()) + "Tpos Y : " + std::to_string(tpos.y()));
		gplib::font::TextNC(500, 120, 0.f, msg2, ARGB(255, 255, 0, 0), 0);
		std::string msg3("pos x " + std::to_string(velocity_.x()) + "pos Y : " + std::to_string(body_.pos().y()));
		gplib::font::TextNC(500, 140, 0.f, msg3, ARGB(255, 255, 0, 0), 0);
		gplib::draw::CkRect(searchRt_, ARGB(255, 255, 0, 255));
		gplib::draw::CkRect(defsetRt_, ARGB(255, 255, 0, 0));
		gplib::draw::CkRect(offsetRt_, ARGB(255, 255, 255, 0));

#endif


	}


	void Enemy_Fly::setDir(int dir)
	{
		dir_ = dir;
	}

	void Enemy_Fly::ReverseDir()
	{
		dir_ *= -1;
	}
}