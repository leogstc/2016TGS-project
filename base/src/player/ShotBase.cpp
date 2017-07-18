#include "ShotBase.h"

#include "../config.h"
#include "../stage/map.h"
#include "../stage/stageFlag.hpp"
#include "player.h"

//弾を作成するオブジェクト

namespace shot
{

	ShotBase::ShotBase(const std::string& objectName, const std::string& resname, float dmg, RECT offsetRt)
		:
		MapCollisionObj(
			//ObjectName
			objectName,
			//Body
			ci_ext::DrawObjf(
				ci_ext::Vec3f(0, 0, 0.75f),	//pos
				ci_ext::Vec2f::zero(),	//scale
				resname,				//resname
				ci_ext::Vec2f::zero(),	//src
				ci_ext::Vec2f::zero(),	//src size
				0xFFFFFFFF,	//color
				0.0f					//degree
				),
			//Velocity
			ci_ext::Vec3f::zero()
			),
		dmg_(dmg),
		offsetRt_(offsetRt),
		dir_(DIR::LEFT)
	{}

	ShotBase::ShotBase(const std::string& objectName, const std::string& resname, RECT offsetRt)
		:
		MapCollisionObj(
			//ObjectName
			objectName,
			//Body
			ci_ext::DrawObjf(
				ci_ext::Vec3f(0, 0, 0.75f),	//pos
				ci_ext::Vec2f::zero(),	//scale
				resname,				//resname
				ci_ext::Vec2f::zero(),	//src
				ci_ext::Vec2f::zero(),	//src size
				0xFFFFFFFF,	//color
				0.0f					//degree
				),
			//Velocity
			ci_ext::Vec3f::zero()
			),
		offsetRt_(offsetRt),
		dir_(DIR::LEFT)
	{}

	ShotBase::~ShotBase()
	{
		
	}

	void ShotBase::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setupDrawObj();
		setupAnim();

		setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
	}

	void ShotBase::update()
	{
		checkHitPlayer();
	}


	void ShotBase::setupDrawObj()
	{

	}

	void ShotBase::setupAnim()
	{
		anim_.setAnimReset(1, 0);
	}

	void ShotBase::render()
	{
		if (body_.resname() != "")
			body_.render();
	}


	bool ShotBase::checkHitPlayer()
	{
		auto player_ = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));

		if (!player_.expired()) {
			if (player_.lock()->isInvincible()) return false;

			RECT pRt = player_.lock()->getBodyCollisionRect();
			RECT eRt = getObjCollisionMapRect(); //This Rect

			if (gplib::math::RectCheck(pRt, eRt)) {
				auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));

				player_.lock()->doOnceHitByEnemy(dmg_);
				return true;
			}
		}

		return false;
	}
}
