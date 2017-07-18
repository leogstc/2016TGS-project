#pragma once

#include "eventBase.h"
#include "../player/player.h"
#include "../effect/effectManager.h"

//Debug
#include "../lib/gplib.h"

namespace gameevent {
	class EventTalk : public EventBase {
		bool release_; //‰ð•ú‚ð—\–ñ‚·‚é
	public:
		EventTalk(const std::istringstream& command)
			:
			EventBase("event_talk", command),
			release_(false)
		{}

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
		}
	};
}