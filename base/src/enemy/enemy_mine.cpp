#include "../config.h"
#include "../stage/map.h"
#include "../lib/gplib.h"
#include "enemy_mine.h"
#include "enemy_boss.h"

namespace shot
{
	EnemyMine::EnemyMine(float dmg, int no)
		:
		ShotBase("enemymine", "enemymine", dmg),no_(no)
	{
		body_.setSrc(ci_ext::Vec2f(0.0f, 0.0f));
		body_.setSrcSize(ci_ext::Vec2f(158, 152));
		body_.setScale(ci_ext::Vec2f(1.f, 1.f));
	}

	EnemyMine::~EnemyMine()
	{
	}

	void EnemyMine::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		auto enemy = ci_ext::weak_to_shared <enemy::Enemy_Boss>(getObjectFromRoot("enemy_boss"));
		ci_ext::Vec2f shotp = enemy->getPos();
		enemy::DIR dir = enemy->getDir();
		body_.setPos(shotp.x() - 158 * no_ + ((float)dir * 200), shotp.y() + 180, 0.75f);
		anim_.setAnimReset(4, 5);
		//velocity_.x(-1.f);
	}

	void EnemyMine::render()
	{
		body_.render();
#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);

#endif
	}

	void EnemyMine::update()
	{
		setObjCollisionMapRect(getShotMapCollisionRect());

		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));
		if (anim_.getAnimNo() == 3)
		{
			anim_.pause();
		}
		body_.offsetPos(velocity_);
		checkHitPlayer();
		//updatefire();
	}

	//void EnemyMine::updatefire()
	//{
	//	insertAsChild(new EnemyMine(20, i));
	//}

	RECT EnemyMine::getShotMapOffset()
	{
		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT EnemyMine::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top+30, offset.right, offset.bottom);
	}
}