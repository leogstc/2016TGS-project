#include "enemy_boss_shot.h"
#include "../config.h"
#include "../stage/map.h"
#include "../lib/gplib.h"
#include "enemy_boss.h"
#include "../player/player.h"

namespace shot
{
	BossShot::BossShot(int x, int y, float dmg)
		:
		ShotBase("boss_shot", "boss_shot", dmg)
	{
		body_.setSrc(ci_ext::Vec2f(0.0f, 0.0f));
		body_.setSrcSize(ci_ext::Vec2f(152, 44));
		body_.setScale(ci_ext::Vec2f(1.f, 1.f));
		body_.setPos((float)x, (float)y, 0.75f);
	}

	BossShot::~BossShot()
	{
	}

	void BossShot::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		enemyboss_ = ci_ext::weak_cast<enemy::Enemy_Boss>(getParentPtr().lock()->getParentPtr().lock()->getObjectFromMyChildren("enemy_boss"));
		isShot = false;
		atkspeed = 2.f;
		anim_.setAnimReset(3, 5);

		enemy::DIR shotdir = enemyboss_.lock()->getDir();
		if ((int)shotdir > 0)
		{
			dir_ = DIR::RIGHT;
		}
		else
		{
			dir_ = DIR::LEFT;
		}
	}

	void BossShot::render()
	{
		body_.render();
#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);
#endif
	}

	void BossShot::update()
	{
		setObjCollisionMapRect(getShotMapCollisionRect());
		body_.offsetPos(velocity_);

		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));
		body_.setScale(ci_ext::Vec2f((int)dir_ * config.GAME_SCALE, 1.f));
		if (++cnt > 60)
		{
			updatefire();
		}

		int i = checkMoveMapX();
		if (i != -1) kill();

		checkHitPlayer();
	}

	void BossShot::updatefire()
	{
		enemy::DIR shotdir = enemyboss_.lock()->getDir();

		if (!isShot) //•ûŒü‚É‚æ‚Á‚Ä•Ï‚¦‚é
		{
			if (dir_==DIR::RIGHT)
			{
				velocity_.x(atkspeed);
				isShot = true;
			}
			else 
			{
				velocity_.x(-atkspeed);
				isShot = true;
			}
		}
		body_.offsetPos(velocity_);
	}

	void BossShot::reverseDir()
	{
		dir_ = (DIR)((int)dir_ * -1);
	}

	RECT BossShot::getShotMapOffset()
	{
		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT BossShot::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

}