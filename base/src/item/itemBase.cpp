#include "itemBase.h"

#include "../config.h"
#include "../stage/map.h"
#include "../stage/stageFlag.hpp"
#include "../gameFlag.hpp"
//#include "../player/player.h" //Playerのあたり判定取得のため
//#include "../player/playerInfo.hpp" //Flag in that Player Life

namespace item {
	ItemBase::ItemBase(const std::string& objectName, const std::string& resname, int srcY, int x, int y, int id, int uniqueId, RECT offsetRt)
		:
		MapCollisionObj(
			objectName,
			ci_ext::DrawObjf(
				ci_ext::Vec3f(x, y, 0.8f),	//xyz
				ci_ext::Vec2f(config.GAME_SCALE, config.GAME_SCALE),		//scale
				resname,					//resname
				ci_ext::Vec2f(
					map::sprite::SPRITE_OFFSET,
					map::sprite::SPRITE_OFFSET + (srcY * map::sprite::SPRITE_SIZE)
				),		//src
				ci_ext::Vec2f(map::sprite::TILE_SIZE, map::sprite::TILE_SIZE),		//size
				//ci_ext::Vec2f(2, 2),		//src
				//ci_ext::Vec2f(60, 60),	//size
				0xFFFFFFFF,					//color
				0.0f						//degree
			),
			ci_ext::Vec3f::zero()
		),
		id_(id),
		uid_(uniqueId),
		offsetRt_(offsetRt)
	{
	}

	void ItemBase::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		//アニメーションSetup
		setupAnim();
		body_.setSrc(ci_ext::Vec2f((anim_.getAnimNo() * map::sprite::SPRITE_SIZE) + map::sprite::SPRITE_OFFSET, body_.src().y()));

		//RECT & Map Collision
		setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
		//mapCheckCollisionY(true);
	}

	void ItemBase::setupAnim()
	{
		anim_.setAnimReset(1, 0);
	}

	void ItemBase::update()
	{
		//RECT & Map Collision
		{
			//Set RECT for checking Map Collision
			//setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);
			//^ do in init() for first frame
			//2nd frame and goes on are set by below setObjCollisionMapRect() for player collision
			//これで判定ずれなし

			//落下
			//velocity_.offset(0, config.GRAVITY);

			//Check
			mapCheckMoveXY();

			//Move完了後
			//Set RECT for checking Player Collision
			setObjCollisionMapRect(offsetRt_.left, offsetRt_.top, offsetRt_.right, offsetRt_.bottom);

			//gplib::draw::CkRect(getObjCollisionMapRect());
		}

		//アニメーション
		anim_.step();
		body_.setSrc(ci_ext::Vec2f((anim_.getAnimNo() * map::sprite::SPRITE_SIZE) + map::sprite::SPRITE_OFFSET, body_.src().y()));

		//Playerとの判定
		checkHitPlayer();
	}

	void ItemBase::render()
	{
		body_.render();
	}

	void ItemBase::getPlayerPtr()
	{
		player_ = ci_ext::weak_cast<player::Player>(getParentPtr().lock()->getParentPtr().lock()->getObjectFromMyChildren("player"));
	}

	bool ItemBase::checkHitPlayer()
	{
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		if (!player_.expired()) {
			RECT pRt = player_.lock()->getBodyCollisionRect();
			RECT iRt = getObjCollisionMapRect(); //This Rect
			if (gplib::math::RectCheck(pRt, iRt)) {
				//Set flag get this object in this player life
				//auto pInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
				//if (!pInfo.lock()->isItemFlagOn(id_))
				//	pInfo.lock()->addItemFlag(id_);


				//Set flag get this object
				auto stageFlag = ci_ext::weak_cast<map::StageFlag>(getObjectFromRoot("stage_flag"));
				if (!stageFlag.lock()->isItemFlagOn(id_))
					stageFlag.lock()->addItemFlag(id_);
				//Unique Item
				{
					//If not common
					if (uid_ != -1) {
						//Set unique flag
						auto gameFlag = ci_ext::weak_cast<game::GameFlag>(getObjectFromRoot("game_flag"));
						if (!gameFlag.lock()->isUniqueItemFlagOn(uid_))
							gameFlag.lock()->addUniqueItemFlag(uid_);
					}
				}

				//Do when get this item
				getItem();

				//Delete This Obj
				kill();

				return true;
			}
		}
		else {
			getPlayerPtr();
		}
		return false;
	}
}