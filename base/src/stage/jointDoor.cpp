#include "jointDoor.h"

#include "map.h" //sprite::
#include "../player/player.h" //controlableñΩóﬂ

namespace map {
	enum AnimationSetting {
		MAX_PIC = 5,
		STEP_FRAME = 10,
	};

	JointDoor::JointDoor(int id, info::JointType type, RECT rt, bool close, bool anim)
		:
		Object("joint_door_" + std::to_string(id)),
		close_(close), finishCloseFlag_(false),
		type_(type), useAnim_(anim),
		body_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),	//XYZ
			ci_ext::Vec2f::one(),	//scale
			"joint_door",			//resname
			ci_ext::Vec2f::zero(),	//src
			ci_ext::Vec2f::zero(),	//src size
			0xFFFFFFFF,				//color
			0.0f					//degree
		))
	{
		body_.setSrc(ci_ext::Vec2f(sprite::SPRITE_OFFSET, sprite::SPRITE_OFFSET));
		float sizey = ((sprite::TILE_SIZE + sprite::SPRITE_OFFSET) * 2);
		body_.setSrcSize(ci_ext::Vec2f(sprite::TILE_SIZE, sizey));

		float scalex, scaley;
		//Calc Scale & Set Pos
		if (type_ == info::JointType::VERTICAL) {
			body_.setPos(ci_ext::Vec3f(rt.left, rt.top, 0.95f));
			scalex = (float)(rt.right - rt.left) / sprite::TILE_SIZE;
			scaley = (float)(rt.bottom - rt.top) / sizey;
		}
		else {
			body_.setPos(ci_ext::Vec3f(rt.left, rt.bottom, 0.95f));
			scalex = (float)(rt.bottom - rt.top) / sprite::TILE_SIZE;
			scaley = (float)(rt.right - rt.left) / sizey;

			//set degree
			body_.setDegree(90.f);
		}
		body_.setScale(ci_ext::Vec2f(scalex, scaley));

		//Animation Setup
		//Open Door
		if (!close) {
			anim_.setAnimReset(MAX_PIC, STEP_FRAME);
			anim_.pause();
		}
		//Close Door
		else {
			anim_.setAnimReset(MAX_PIC, STEP_FRAME);

			//Set Src by Anim
			if (anim) {
				anim_.setAnimNo(MAX_PIC - 1);
				anim_.setBackward();
			}
			else {
				//Still in closed state
				anim_.pause();
				anim_.setAnimNo(0);
			}

			body_.setSrc(ci_ext::Vec2f((sprite::SPRITE_SIZE * anim_.getAnimNo()) + sprite::SPRITE_OFFSET, sprite::SPRITE_OFFSET));
		}
	}

	void JointDoor::init()
	{
		//If Close Door
		if (close_ && useAnim_) {
			getParentPtr().lock()->pause();
			selfPtr().lock()->run();
		}
	}

	void JointDoor::update()
	{
		anim_.step();
		body_.setSrc(ci_ext::Vec2f((sprite::SPRITE_SIZE * anim_.getAnimNo()) + sprite::SPRITE_OFFSET, sprite::SPRITE_OFFSET));

		if (anim_.isLastFrame()) {
			//Open Door
			if (!close_) {
				auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
				if (player.lock()->getPlayerState() == player::State::STAND) {
					player.lock()->setPlayerStateJoint(player::State::WALK);
				}
				if (player.lock()->getPlayerState() == player::State::START_FLY) {
					player.lock()->setPlayerStateJoint(player::State::FALL);
				}

				getParentPtr().lock()->runAll();
				kill();
			}
			//Closing Door
			else {
				if (!finishCloseFlag_) {
					anim_.pause();
					getParentPtr().lock()->runAll();
					ci_ext::weak_cast<Map>(getParentPtr()).lock()->finishClosingDoorProcess();

					//ÉvÉåÉCÉÑÅ[ÇìÆÇØÇÈÇÊÇ§Ç…
					auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
					player.lock()->setControlable(true);

					finishCloseFlag_ = true;
				}
			}
		}
	}

	void JointDoor::render()
	{
		body_.renderLeftTop();
	}

	void JointDoor::resumeAnim()
	{
		//Init of Opening Door

		anim_.resume();
		getParentPtr().lock()->pause();
		selfPtr().lock()->run();

		//If Open Door
		if (!close_) {
			auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
			player.lock()->setVelocity(0, 0);
			if (player.lock()->getPlayerState() == player::State::WALK) {
				player.lock()->setPlayerStateJoint(player::State::STAND);
			}
		}
	}
}