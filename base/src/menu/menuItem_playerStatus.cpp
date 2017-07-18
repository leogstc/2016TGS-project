#include "menuItem_playerStatus.h"

namespace menu {
	namespace item {
		MenuItemPlayerStatus::MenuItemPlayerStatus()
			:
			Object("menuItem_playerStatus"),
			intro_(true), outro_(false), t_(0),
			bgX_(0), bgY_(0), bgScaleX_(1), bgScaleY_(1), bgMoveX_(0), bgMoveY_(0)
		{
			//BG
			bgScaleX_ = BG_MAX_X / gplib::draw::GetImageWidth("submenu_left");
			bgScaleY_ = 544.f / gplib::draw::GetImageHeight("submenu_left");
			bgX_ = -BG_MAX_X / 2;
			bgY_ = float(gplib::system::WINH / 2);
		}

		void MenuItemPlayerStatus::init()
		{
			getParentPtr().lock()->pauseAll();
			run();
		}

		void MenuItemPlayerStatus::update()
		{
			//Intro/Outro Animation
			if (intro_) {
				const float FRAME = 15;
				if (t_ == 0) {
					bgMoveX_ = BG_MAX_X / FRAME;
				}
				t_++;
				bgX_ += bgMoveX_;
				if (t_ >= FRAME) {
					t_ = 0;
					intro_ = false;
				}
				return;
			}
			if (outro_) {
				const float FRAME = 15;
				if (t_ == 0) {
					bgMoveX_ = BG_MAX_X / FRAME;
				}
				t_++;
				bgX_ -= bgMoveX_;
				if (t_ >= FRAME) {
					getParentPtr().lock()->runAll();
					kill();
				}
				return;
			}

			if (gplib::input::CheckPush(gplib::input::KEY_BTN1)) {
				outro_ = true;
			}
		}

		void MenuItemPlayerStatus::render()
		{
			//BG
			gplib::draw::GraphNC(bgX_, bgY_, 0.349f, "submenu_left",
				0, 0, gplib::draw::GetImageWidth("submenu_left"), gplib::draw::GetImageHeight("submenu_left"),
				0.0f, bgScaleX_, bgScaleY_);

			float x = bgX_ - (BG_MAX_X / 2) + 20;

			gplib::font::TextNC((int)x, 20, 0.33f, "Player Status", ARGB(255, 255, 0, 0), 0);

			auto pInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
			if (!pInfo.expired()) {
				int hp = (int)pInfo.lock()->getHp();
				int maxhp = 100;
				int mp = (int)pInfo.lock()->getMp();
				int maxmp = 100;
				int y = 3;
				gplib::font::TextNC((int)x, (y++ * 20), 0.33f, "Name: KirOOamO MOriOO", ARGB(255, 255, 0, 0), 0);

				gplib::font::TextNC((int)x, (y++ * 20), 0.33f, "HP:" + std::to_string(hp) + "/" + std::to_string(maxhp), ARGB(255, 255, 0, 0), 0);
				gplib::font::TextNC((int)x, (y++ * 20), 0.33f, "MP:" + std::to_string(mp) + "/" + std::to_string(maxmp), ARGB(255, 255, 0, 0), 0);
				
			}
		}
	}
}