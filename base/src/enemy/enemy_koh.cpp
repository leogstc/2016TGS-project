#include "enemy_koh.h"
#include "../config.h"
#include "../stage/map.h"
#include "../player/shot.h"
namespace enemy
{
	EnemyKoh::EnemyKoh(int x, int y, int id)
		:
		EnemyBase("enemy_eplayer", "enemy003", x, y,0, id)
	{

	}

	EnemyKoh::~EnemyKoh()
	{
	}

	void EnemyKoh::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		setState(State::STAND);
		body_.setSrcSize(ci_ext::Vec2f(128, 128));
		body_.setScale(ci_ext::Vec2f(config.GAME_SCALE, config.GAME_SCALE));

		velocity_.y(2.f);

		
		//RECT & Map Collision
		setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
	}

	void EnemyKoh::update()
	{
		setObjCollisionMapRect(getMapCollisionRect());
		body_.offsetPos(velocity_);

		switch (state_) 
		{
		case State::STAND: updateStand(); break;
		case State::RWALK: updateRWalk(); break;
		case State::LWALK: updateLWalk(); break;
		//case State::FALL: updateFall(); break;
		}
		mapCheckMoveXY();
		checkHit();
		
	}

	void EnemyKoh::updateStand()
	{
		if (checkUnderFloor())
		{
			velocity_.y(0);
			setState(State::RWALK);
		}
	}

	void EnemyKoh::updateRWalk()
	{
		velocity_.x(2.f);
		mapCheckCollisionX(true);
		int i = checkMoveMapX();
		if (i != -1) setState(State::LWALK);
	}

	void EnemyKoh::updateLWalk()
	{
		velocity_.x(-2.f);
		mapCheckCollisionX(true);
		int i = checkMoveMapX();
		if (i != -1) setState(State::RWALK);
	}

	void EnemyKoh::setState(State state)
	{
		state_ = state;
	}

	void EnemyKoh::render()
	{
		body_.render();

		auto rt = getMapCollisionRect();
		gplib::draw::CkRect(rt);
	}

	void EnemyKoh::checkHit()
	{
		auto shot = ci_ext::weak_cast<shot::PlayerShot>(getObjectFromRoot("shot"));
		if (!shot.expired())
		{
			RECT sRt = shot.lock()->getShotMapCollisionRect();
			RECT inRt;
			RECT eRt = getObjCollisionMapRect();
			IntersectRect(&inRt, &sRt, &eRt);

			if (inRt.bottom - inRt.top > 0)
			{
				auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));
				if (!stageFlag.lock()->isEnemyFlagOn(id_))
					stageFlag.lock()->addEnemyFlag(id_);

					kill();
			}
		}

		
	}

	RECT EnemyKoh::getMapOffset()
	{
		RECT rt = { 0,0,0,0 };
		if (body_.pos().y() > 110)
		{
			rt = {
				sprite::MAP_COL_OFFSET_FALL_LEFT,
				sprite::MAP_COL_OFFSET_FALL_TOP,
				sprite::MAP_COL_OFFSET_FALL_RIGHT,
				sprite::MAP_COL_OFFSET_FALL_BOTTOM
			};
		}
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT EnemyKoh::getMapCollisionRect()
	{
		RECT offset = getMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

	ci_ext::Vec2f EnemyKoh::getpos()
	{
		return{ body_.pos().x(),body_.pos().y() };
	}
}