#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../object/root.h"
//#include "../object/movableObject.hpp"

#include "../lib/xinput.hpp"

#include "../stage/map.h"
#include "../stage/stageInfo.h"

#include "../gameFlag.hpp"
#include "../player/playerInfo.hpp"
#include "../event/worldEventManager.h"

#include "../debug/debug.h"
#include "../menu/menuEllipse.h"
//#include "../effect/effectManager.h"

#include "loading.h"

#include "../scene/gameOver.hpp"
//#include "stageSelect.h"
//#include <iostream>

namespace scene {
	class Stage : public ci_ext::Object {
		std::string stageName_;
		map::StageInfo stageInfo_;
		int cmd_;
	public:
		Stage(const std::string& stageName, map::StageInfo& stageInfo)
			:
			Object("scene_stage_" + stageName), stageName_(stageName), stageInfo_(stageInfo)
		{}

		~Stage()
		{}

		void init() override
		{
			auto gameFlag = getObjectFromRoot("game_flag");
			if (gameFlag.expired())
				getRootObject().lock()->insertAsChild(new game::GameFlag());
			auto playerInfo = getObjectFromRoot("player_info");
			if (playerInfo.expired())
				getRootObject().lock()->insertAsChild(new player::PlayerInfo());
			auto eventManager = getObjectFromRoot("event_manager");
			if (eventManager.expired())
				getRootObject().lock()->insertAsChild(new gameevent::WorldEventManager());

			//Create map & send stageInfo_ as copy
			insertAsChild(new map::Map(stageName_, stageInfo_));
			stageInfo_.clear(); //Delete buffer

			cmd_ = 0;

		}

		void update() override
		{
			//Debug
			debug::MouseMoveCamera();
			//auto mouse = debug::GetGameMousePosition();
			//gplib::debug::TToM("mouse in game pos(%d,%d)", mouse.x, mouse.y);

			//if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
			//	auto efMana = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			//	if (!efMana.expired())
			//		efMana.lock()->createEffect("fadeinblack", 0, 0);
			//	sleep(45);
			//}
				
			/*if (gplib::input::CheckPush(gplib::input::KEY_F1)) {
				insertAsChild(new menu::MenuEllipse());
			}*/

			if (XInput::CheckPress(XInput::ButtonsID::L2) && XInput::CheckPress(XInput::ButtonsID::R2)) {
				if (XInput::CheckPush(XInput::ButtonsID::L) && cmd_ == 0) cmd_++;
				if (XInput::CheckPush(XInput::ButtonsID::R) && cmd_ == 1) cmd_++;
				if (XInput::CheckPush(XInput::ButtonsID::back) && cmd_ == 2) cmd_++;
				if (XInput::CheckPush(XInput::ButtonsID::start) && cmd_ == 3) cmd_++;
				if (XInput::CheckPush(XInput::ButtonsID::B) && cmd_ == 4) {

					//insertAsChild(new menu::MenuEllipse());
					auto rootC = getObjectsFromRoot({ "scene" });
					for (auto& c : rootC)
						c.lock()->kill();

					getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::LOGO));
				}
			}
			else {
				cmd_ = 0;
			}
		}

		void resume() override
		{
			getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::GAME_OVER));
			//insertToParent(new scene::GameOver());
			kill();
		}
	};

}