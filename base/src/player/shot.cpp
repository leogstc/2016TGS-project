#include "shot.h"
#include "../config.h"
#include "../player/player.h"
#include "../stage/map.h"
#include "../lib/gplib.h"

namespace shot
{
	PlayerShot::PlayerShot(float pow, const std::string& type)
		:
		ShotBase("playershot", "shot",dmg_),
		pow_(pow),
		type_(type)
	{
		body_.setSrc(ci_ext::Vec2f(0.f, 0.f/*96.f*/));
		body_.setSrcSize(ci_ext::Vec2f(32.f, 32.f));
		body_.setScale(ci_ext::Vec2f(1.f, 1.f));
	}

	PlayerShot::~PlayerShot()
	{}

	void PlayerShot::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		initVelocity();

		auto player = ci_ext::weak_to_shared <player::Player>(getObjectFromRoot("player"));
		ci_ext::Vec2f f = player->getPos();
		body_.setPos(f.x(),f.y(),0.75f);

		isShot = false;
		body_.setColor(255, 255, 250, 92);
		//anim_.setAnimReset(3, 6);
	}

	void shot::PlayerShot::initVelocity()
	{
		//velocity_.x(config.PLAYER_MAGIC_SPD);
		//velocity_ = firstVelocity[type_];
		using namespace gplib;
		using namespace math;
		using namespace ci_ext;
		if(type_=="nomal")
			velocity_.set(Vec3f(config.PLAYER_MAGIC_SPD, 0.f));
		else if(type_=="up")
			velocity_.set(Vec3f(ROUND_X(45, config.PLAYER_MAGIC_SPD), ROUND_Y(45, config.PLAYER_MAGIC_SPD)));
		else
			velocity_.set(Vec3f(ROUND_X(315, config.PLAYER_MAGIC_SPD), ROUND_Y(315, config.PLAYER_MAGIC_SPD)));
	}

	void PlayerShot::update()
	{
		setObjCollisionMapRect(getShotMapCollisionRect());
		body_.offsetPos(velocity_);
		body_.offsetDegree(2.f);
		//anim_.step();
		//body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 96));
		updatefire();
	
		int i = checkMoveMapX();
		if (i != -1) kill();

		setObjCollisionMapRect(getShotMapCollisionRect());
		
	}

	void PlayerShot::render()
	{
		body_.render();

#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);

		
#endif
	}

	void PlayerShot::updatefire()
	{
		auto player = ci_ext::weak_to_shared <player::Player>(getObjectFromRoot("player"));
		player::DIR v = player->getDir();
		if (!isShot)
		{
			if ((float)v > 0)
			{
				velocity_.x(config.PLAYER_MAGIC_SPD);
				isShot = true;
			}
			else if ((float)v < 0)
			{
				velocity_.x(-config.PLAYER_MAGIC_SPD);
				isShot = true;
			}
		}
	}

	RECT PlayerShot::getShotMapOffset()
	{
		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT PlayerShot::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

	float shot::PlayerShot::getPow()
	{
		return pow_;
	}

	std::string shot::PlayerShot::getType()
	{
		return type_;
	}
}