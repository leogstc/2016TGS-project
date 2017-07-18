#pragma once

#include "menuUpDownBase.h"
#include "../lib/gplib.h"
#include "../scene/stage.hpp"
#include "../scene/loading.h"
#include "../gameFlag.hpp"

namespace menu {
	class MenuStageSelect : public MenuUpDownBase {
		std::string difficulty[3] = { "easy","nomal","hard" };
		float degree;
	public:
		std::vector<std::string> items_;

		MenuStageSelect()
			:
			MenuUpDownBase("menu_stage_select", 3)
		{
			items_.push_back("easy");
			items_.push_back("nomal");
			items_.push_back("hard");
			degree = 0.f;
		}

		~MenuStageSelect()
		{
			items_.clear();
		}

		void update() {
			input();
			degree+=5.f;
			if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
				//getRootObject().lock()->insertAsChild(new scene::Stage(items_[now_]));
				gplib::se::Play("decision");
				gplib::bgm::Stop("title");
				getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::STAGE, items_[now_]));
				getParentPtr().lock()->getParentPtr().lock()->kill();
			}
		}

		void render() {
			//gplib::font::TextCenterNC(gplib::system::WINW / 2, gplib::system::WINH / 2 - 50, 0.03f, "Stage Select", ARGB(255, 255, 255, 255), 1);

			float firstx = static_cast<float>(gplib::system::WINW / 2);
			float firsty = static_cast<float>(gplib::system::WINH / 2 + 200);
			float spacey = 40;

			auto gameFlag = ci_ext::weak_cast<game::GameFlag>(getObjectFromRoot("game_flag"));

			int i = 0;
			for (auto& item : items_) {
				float posx = firstx;
				float posy = (i * spacey) + firsty;
				D3DCOLOR color = ARGB(255, 255, 0, 0);

				if (gameFlag.lock()->isClearedFlagOn(item)) {
					color = ARGB(255, 255, 255, 255);
				}

				gplib::draw::Graph(firstx,posy, 0.f, difficulty[i], 0, 0, 143
					, 51);
				//gplib::font::TextCenterNC((int)posx, (int)posy, 0.03f, item, color, 0);
				i++;
			}

			gplib::draw::Graph((int)firstx-90, (now_*(int)spacey)+(int)firsty, 0.f, "cursol", 0, 0, 200, 200,
				degree,40.f/200.f, 40.f / 200.f);

			//RECT rt = {
			//	LONG(now_ + firstx - 80),
			//	LONG((now_ * spacey) + firsty - 10),
			//	LONG(now_ + firstx + 80),
			//	LONG((now_ * spacey) + firsty + 10)
			//};
			//gplib::draw::CkRect(rt, ARGB(255, 255, 255, 0));
		}
	};
}