#include "enemy_shot.h"
#include "../config.h"
#include "../stage/map.h"
#include "../lib/gplib.h"
#include "enemy_bat.h"
#include "../player/player.h"

namespace shot
{
	EnemyShot::EnemyShot(float dmg)
		:
		ShotBase("enemyshot", "enemyshot", dmg)
	{
		body_.setSrc(ci_ext::Vec2f(0.0f, 0.0f));
		body_.setSrcSize(ci_ext::Vec2f(32, 32));
		body_.setScale(ci_ext::Vec2f(1.f, 1.f));
	}

	EnemyShot::~EnemyShot()
	{
	}

	void EnemyShot::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		enemybat_ = ci_ext::weak_cast<enemy::Enemy_Bat>(getObjectFromRoot("enemy_bat"));
		ci_ext::Vec2f shotp = enemybat_.lock()->getPos();
		//enemy::DIR shotdir = enemybat_.lock()->getDir();
		body_.setPos(shotp.x(), shotp.y(), 0.75f);
		//if ((int)shotdir > 0)
		//{
		//	dir_ = DIR::RIGHT;
		//}
		//else
		//{
		//	dir_ = DIR::LEFT;
		//}

		isShot = false;
		atkspeed = 2.f;
	}

	void EnemyShot::render()
	{
		body_.render();
#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);

#endif
	}

	void EnemyShot::update()
	{
		setObjCollisionMapRect(getShotMapCollisionRect());
		body_.offsetPos(velocity_);

		updatefire();

		int i = checkMoveMapX();
		if (i != -1) kill();

		setObjCollisionMapRect(getShotMapCollisionRect());
		checkHitPlayer();
	}

	void EnemyShot::updatefire()
	{
		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));
		if (player != nullptr)
		{
			tpos = player->getPos();
			if (!isShot)
			{
				//if (dir_ == DIR::RIGHT)
				//{
					velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
					velocity_.y(gplib::math::ROUND_Y(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
					isShot = true;
				//}
				//else 
				//{
				//	velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
				//	velocity_.y(gplib::math::ROUND_Y(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
				//	isShot = true;
				//}
			}
		}

		body_.offsetPos(velocity_);
	}

	RECT EnemyShot::getShotMapOffset()
	{
		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT EnemyShot::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

}