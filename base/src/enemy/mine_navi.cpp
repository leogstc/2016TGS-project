#include "mine_navi.h"
#include "../stage/map.h"
#include "../lib/gplib.h"
#include "enemy_mine.h"

namespace shot
{
	MineNavi::MineNavi()
		:
		ShotBase("minenavi", "enemymine", 0)
	{
		body_.setSrc(ci_ext::Vec2f(0.f, 0.f));
		body_.setSrcSize(ci_ext::Vec2f(32.f, 32.f));
		body_.setScale(ci_ext::Vec2f(1.f, 1.f));
	}

	MineNavi::~MineNavi()
	{
	}

	void MineNavi::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		enemyboss_ = ci_ext::weak_cast<enemy::Enemy_Boss>(getParentPtr().lock()->getParentPtr().lock()->getObjectFromMyChildren("enemy_boss"));
		ci_ext::Vec2f shotp = enemyboss_.lock()->getPos();
		body_.setPos(shotp.x(), shotp.y(), 0.75f);
		atkspeed = 1.8f;
		isShot = false;
		minecnt = 0;
		no = 0;
		create_timer_ = insertAsChild(new game::Timer("generator_timer", 0.5f));
	}

	void MineNavi::render()
	{
		//body_.render();
#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);

#endif
	}

	void MineNavi::update()
	{
		setObjCollisionMapRect(getShotMapCollisionRect());
		body_.offsetPos(velocity_);

		updatenavi();

		int i = checkMoveMapX();
		if (i != -1) velocity_.x(0);

		if (velocity_.x() != 0)
		{
			auto create_navitimer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
			if (create_navitimer->isPause())
			{
				create_navitimer->run();
			}
			if (create_navitimer->isTimeOver())
			{
				MineCreate(); //地面の弾を作る
				gplib::se::Play("ice");
				create_timer_.lock()->kill();
				create_timer_ = insertAsChild(new game::Timer("generator_timer", 0.4f));
			}
		}
		else
		{
			create_timer_.lock()->pause();//タイマー停止
			if (++minecnt > 60)
			{
				create_timer_.lock()->kill();
				kill();
			}

		}
	}

	void MineNavi::updatenavi()
	{
		enemy::DIR shotdir = enemyboss_.lock()->getDir();

			if (!isShot)
			{
				if ((float)shotdir > 0)
				{
					velocity_.x(atkspeed);
					isShot = true;
				}
				else if ((float)shotdir < 0)
				{
					velocity_.x(-atkspeed);
					isShot = true;
				}
			}
			
		body_.offsetPos(velocity_);
	}

	void MineNavi::MineCreate()
	{
		if (!enemyboss_.expired())
		{
			enemy::DIR minedir = enemyboss_.lock()->getDir();
			if ((float)minedir > 0)
			{
				insertAsChild(new shot::EnemyMine(20, no));
				no--;
			}
			else if ((float)minedir < 0)
			{
				insertAsChild(new shot::EnemyMine(20, no));
				no++;
			}
		}
		
	}

	RECT MineNavi::getShotMapOffset()
	{

		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT MineNavi::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}


}