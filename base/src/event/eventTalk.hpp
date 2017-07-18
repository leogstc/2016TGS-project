#pragma once

#include "eventBase.h"
#include "../player/player.h"
#include "../effect/effectManager.h"

//Debug
#include "../lib/gplib.h"

namespace gameevent {

	class EventTalk : public EventBase {
		bool release_; //‰ð•ú‚ð—\–ñ‚·‚é
		ci_ext::DrawObjf tutorial_box_;
		ci_ext::DrawObjf button_;

	public:
		EventTalk(const std::istringstream& command)
			:
			EventBase("event_talk", command),
			release_(false)
			,
				tutorial_box_(ci_ext::DrawObjf(
					ci_ext::Vec3f::zero(),							//pos
					ci_ext::Vec2f::one(),							//scale
					"dialog_box",									//resname
					ci_ext::Vec2f::zero(),							//src
					ci_ext::Vec2f::zero(),							//src size
					ci_ext::Color(255, 255, 255, 255),				//color
					0.0f											//degree
					))
			,
			button_(ci_ext::DrawObjf(
				ci_ext::Vec3f::zero(),							//pos
				ci_ext::Vec2f::one(),							//scale
				"button",									//resname
				ci_ext::Vec2f::zero(),							//src
				ci_ext::Vec2f::zero(),							//src size
				ci_ext::Color(255, 255, 255, 255),				//color
				0.0f											//degree
				))

		{}


		void init() override
		{


			float scale_X = (float)(gplib::system::WINW / DIALOG_BOX_WIDTH  );
			
			tutorial_box_.setPos(0, gplib::system::WINH - DIALOG_BOX_HEIGHT * 0.5f, 0.0f);
			tutorial_box_.setSrcSize(ci_ext::Vec2f(DIALOG_BOX_WIDTH, DIALOG_BOX_HEIGHT));
			tutorial_box_.setScale(ci_ext::Vec2f(scale_X, 0.5f));

		}

		//Resume from Sleep
		void resume() override
		{
			//Stop All
			getRootObject().lock()->pauseAll();

			{
				//Set controlable of player
				auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
				if (!player.expired()) {
					player.lock()->run();
					if (player.lock()->getPlayerState() != player::State::STAND)
						player.lock()->setPlayerState(player::State::STAND);
					player.lock()->setVelocity(0.0f, 0.0f);
					player.lock()->setControlable(false);
				}

				//Set map bg to run till the end
				auto map = getObjectFromRoot("map");
				if (!map.expired()) {
					map.lock()->run();
					auto bgs = map.lock()->getObjectsFromChildren({ "bg_" });
					for (auto& bg : bgs) {
						bg.lock()->run();
					}
				}

				//Set effect to run till the end
				auto effect = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
				if (!effect.expired()) {
					effect.lock()->runAll();
				}
			}
			//Run this obj
			run();
		}

		void update() override
		{
			if (!release_) {
				if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
					release_ = true;
				}
			}
			else {
				getRootObject().lock()->runAll();

				//Set controlable of player
				auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
				if (!player.expired()) {
					player.lock()->setControlable(true);
				}

				kill();
			}
		}

		void render() override
		{
			gplib::font::TextNC(20, 450, 0.0f, "Event Talk running, press A to exit", ARGB(255, 255, 0, 0), 0);
			tutorial_box_.renderLeftTopNC();

		}
	};
}