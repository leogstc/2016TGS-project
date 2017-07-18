#include "enemyBase.h"

#include "../config.h"
#include "../stage/map.h"
#include "../stage/stageFlag.hpp"
#include "mask.hpp"
#include "bound_object.h"
//#include "../player/player.h" //Playerのあたり判定取得のため

namespace enemy {
	EnemyBase::EnemyBase(const std::string& objectName, const std::string& resname, int x, int y, int id, float life, float dmg, RECT offsetRt, RECT defsetRt, RECT searchRt)
		:
		MapCollisionObj(
			objectName,
			ci_ext::DrawObjf(
				ci_ext::Vec3f(x, y, 0.85f),	//xyz
				ci_ext::Vec2f(config.GAME_SCALE, config.GAME_SCALE),		//scale
				resname,					//resname
				ci_ext::Vec2f::zero(),		//src
				ci_ext::Vec2f::zero(),		//size
				0xFFFFFFFF,					//color
				0.0f						//degree
			),
			ci_ext::Vec3f::zero()			//velocity
		),
		id_(id),
		life_(life),
		dmg_(dmg),
		offsetRt_(offsetRt),
		defsetRt_(defsetRt),
		searchRt_(searchRt),
		dir_(DIR::LEFT),
		notCheckHit_(false),
		isbound_(false)
	{}

	EnemyBase::EnemyBase(const std::string& objectName, int x, int y, int id)
		:
		MapCollisionObj(
			objectName,
			ci_ext::DrawObjf(
				ci_ext::Vec3f(x, y, 0.85f),	//xyz
				ci_ext::Vec2f::one(),		//scale
				"",					//resname
				ci_ext::Vec2f::zero(),		//src
				ci_ext::Vec2f::zero(),		//size
				0xFFFFFFFF,					//color
				0.0f						//degree
			),
			ci_ext::Vec3f::zero()			//velocity
		),
		id_(id),
		dmg_(0.0f),
		offsetRt_(RECT()),
		defsetRt_(RECT()),
		dir_(DIR::LEFT),
		notCheckHit_(true),
		isbound_(false)
	{}

	void EnemyBase::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();

			getPlayerPtr();
		}

		//RECT & Map Collision
		setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
		makedefRect(defsetRt_.left, defsetRt_.top, defsetRt_.right, defsetRt_.bottom);
	}

	void EnemyBase::update()
	{
		//-- MapのCollisionをチェックしたい場合はこの関数を追加してください
		//mapCheckMoveXY();
		//-- 以上MapのCollision


		//----- 以下はサンプル
		{
			//アニメーション
			anim_.step();
			body_.setSrc(ci_ext::Vec2f((anim_.getAnimNo() * body_.srcSize().x()), body_.src().y()));
			body_.setScale(ci_ext::Vec2f((int)dir_ * config.GAME_SCALE, body_.scale().y()));

			checkHitPlayer();
		}
		//RECT & Map Collision
		setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
		makedefRect(defsetRt_.left, defsetRt_.top, defsetRt_.right, defsetRt_.bottom);
		//----- 以上はサンプル
	}

	void EnemyBase::render()
	{
		if (body_.resname() != "")
			body_.render();
	}

	void EnemyBase::getPlayerPtr()
	{
		player_ = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player")/*getParentPtr().lock()->getParentPtr().lock()->getObjectFromMyChildren("player")*/);
	}

	void EnemyBase::reverseDir()
	{
		dir_ = (DIR)((int)dir_ * -1);
	}

	bool EnemyBase::checkHitPlayer()
	{
		if (notCheckHit_) return false;

		//Playerとの判定
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		if (!player_.expired()) {
			if (player_.lock()->isInvincible()) return false;

			RECT pRt = player_.lock()->getBodyCollisionRect();
			RECT eRt = getObjCollisionMapRect(); //This Rect

			if (gplib::math::RectCheck(pRt, eRt)) {
				auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));
				if (!stageFlag.lock()->isEnemyFlagOn(id_))
					stageFlag.lock()->addEnemyFlag(id_);

				player_.lock()->doOnceHitByEnemy(dmg_);
				return true;
			}
		}
		else {
			getPlayerPtr();
		}
		return false;
	}

	bool EnemyBase::isSearchPlayer()
	{
		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));

		if (player != nullptr)
		{
			RECT pRt = player->getBodyCollisionRect();
			if (gplib::math::RectCheck(searchRt_, pRt))
				return true;
			else
				return false;
		}
		return false;
	}

	void EnemyBase::damage(float damagePt,/*const std::string& shottype,*/ci_ext::Vec3f velocity)
	{
		gplib::se::Play("hit");
		mask_ = ci_ext::weak_cast<Mask>(getObjectFromChildren("mask_" + body_.resname()));
		if (!mask_.expired()) 
			mask_.lock()->maskON(body_);

		life_ -= damagePt;
		if (life_ <= 0.f)
		{
			kill();
			if (!isbound_)
			{
				gplib::se::Play("bound");
				getRootObject().lock()->insertAsChild(new Bound_Object("bound_" + name(), body_, defsetRt_,/*shottype,*/velocity));
				//insertToParent(new Bound_Object("bound", body_.resname(), body_.x(), body_.y()));
				isbound_ = true;
			}
		}
	}

	//-------------------------------------------------------------------------------------------
	//攻撃のあたり判定
	void EnemyBase::checkhitAtk()
	{
		auto shots = getParentPtr().lock()->getParentPtr().lock()->getObjectsFromRoot({ "playershot" }, { "enemy" });
		for (auto shot : shots)
		{
			if (!shot.expired())
			{
				auto s = ci_ext::weak_cast<shot::PlayerShot>(shot);
				auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
				RECT eRt = getdefRECT();
				RECT sRt = s.lock()->getShotMapCollisionRect();

				if (!effMngr.expired())
				{
					if (gplib::math::RectCheck(eRt, sRt))
					{
						auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));
						if (!stageFlag.lock()->isEnemyFlagOn(id_))
							stageFlag.lock()->addEnemyFlag(id_);

						//kill();
						damage(s.lock()->getPow(),/*shot.lock()->getType(),*/s.lock()->getVelocity());
						s.lock()->kill();
						//effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
					}
				}
			}
		}
			//if (!player_.expired())
			//{
			//	RECT eRt = getdefRECT();
			//	RECT patkRt = player_.lock()->getAtkCollision();
			//	if (gplib::math::RectCheck(eRt, patkRt))
			//	{
			//		auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));
			//		if (!stageFlag.lock()->isEnemyFlagOn(id_))
			//			stageFlag.lock()->addEnemyFlag(id_);

			//		damage(1.3f, ci_ext::Vec3f((float)player_.lock()->getDir()));
			//	}
			//}
	}

	void enemy::EnemyBase::checkMask()
	{
		mask_ = ci_ext::weak_cast<Mask>(getObjectFromChildren("mask_" + body_.resname()));
		if (!mask_.expired())
		{
			if (mask_.lock()->isMaskShow())
				mask_.lock()->update(body_);
		}

	}

	void EnemyBase::makedefRect(int defsetSx, int defsetSy, int defsetEx, int defsetEy)
	{
		defsetRt_ = makeRectScale(defsetSx, defsetSy, defsetEx, defsetEy);
	}

	RECT EnemyBase::getdefRECT()
	{
		return defsetRt_;
	}


	int EnemyBase::getId() const
	{
		return id_;
	}
}