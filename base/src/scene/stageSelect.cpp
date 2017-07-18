#include "stageSelect.h"

#include "../object/root.h"
#include "../gameFlag.hpp"
#include "../stage/bg_star.hpp"
#include "../stage/bg_digitalLine.hpp"
#include "../menu/menuStageSelect.hpp"
//#include "../lib/gplib.h"
//#include "../effect/effectManager.h"
#include "../ui/ui_marquee.hpp"
#include "../stage/bg_fallingLeaves.hpp"
//#include "../ui/ui_loading.hpp"
#include "loading.h"

namespace scene {
	StageSelect::StageSelect()
		:
		Object("scene_stage_select")
	{}
	void StageSelect::init()
	{
		//ルートのオブジェクトにあるwindownに対して、背景色を黒にする命令を送る
		auto root = ci_ext::weak_to_shared<Root>(getRootObject());
		root->changeBkColor(0x000000);

		auto gameFlag = getObjectFromRoot("game_flag");
		if (gameFlag.expired())
			getRootObject().lock()->insertAsChild(new game::GameFlag());

		//insertAsChild(new map::bg::BgStar());
		//insertAsChild(new map::bg::BgDigitalLine());
		insertAsChild(new menu::MenuStageSelect());
		//insertAsChild(new effect::EffectManager());
		//insertAsChild(new ui::MarqueeText("Now Loading"));
		//insertAsChild(new map::bg::BgFallingLeaves());
		//insertAsChild(new ui::NowLoading());
	}

	void StageSelect::update()
	{
		/*if (gplib::input::CheckPush(gplib::input::KEY_SPACE)) {
			auto e = ci_ext::weak_cast<effect::EffectManager>(getObjectFromChildren("effect_manager"));
			e.lock()->createEffect("loading");
		}*/

		//if (gplib::input::CheckPush(gplib::input::KEY_SPACE)) {
		//	insertToParent(new Loading(Scene::STAGE, "demo_big"));
		//	//insertToParent(new Loading(Scene::STAGE_SELECT));
		//	kill();
		//}
	}

	void StageSelect::render()
	{
	}

	
}