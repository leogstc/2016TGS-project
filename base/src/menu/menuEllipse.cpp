#include "menuEllipse.h"
#include "../lib/gplib.h"

#include "menuItem_playerStatus.h"
#include "../stage/map.h"
#include "../event/eventTutorial.h"

#include <sstream> //debug 用

#define RADIUS_INNER_X 100.0f
#define RADIUS_INNER_Y 65.0f
#define RADIUS_OUTER_X 150.0f
#define RADIUS_OUTER_Y 150.0f
#define INTRO_ITEM 3.0f //intで入力 floatで計算
#define OUTRO_ITEM 1.0f //intで入力 floatで計算

namespace menu {
	MenuEllipse::MenuEllipse()
		:
		MenuLeftRightBase("menu_pauseGame", 8),
		skipItem_(4), available_(8),
		radiusX_(RADIUS_OUTER_X), radiusY_(RADIUS_OUTER_Y),
		nowAngle_(-360.0f / maxItems_), firstItemPosAngle_(-90),
		intro_(true), outro_(false),
		t_(0),
		selectable_(false), lastFrameNow_(0),
		incAngle_(0), incRadiusX_(0), incRadiusY_(0),
		alpha_(0), incAlpha_(0)
		
	{
		if (INTRO_ITEM != 1)
			nowAngle_ *= INTRO_ITEM;

		for (auto& a : available_) {
			a = true;
		}
		available_[4] = false;
		available_[6] = false;

		midPos_.x = gplib::system::WINW / 2;
		midPos_.y = gplib::system::WINH / 2;
		anim_.setAnimReset(4, 5);


		gplib::draw::LoadObject("menu", "res/gra/menu/menu.png");
		gplib::draw::LoadObject("submenu_left", "res/gra/menu/submenu_l.png");
		gplib::draw::LoadObject("submenu_bottom", "res/gra/menu/submenu_b.png");
		gplib::draw::LoadObject("cursor", "res/gra/menu/cursor.png");
	}

	MenuEllipse::~MenuEllipse()
	{
		gplib::draw::DeleteObject("menu");
		gplib::draw::DeleteObject("submenu_left");
		gplib::draw::DeleteObject("submenu_bottom");
		gplib::draw::DeleteObject("cursor");
	}

	void MenuEllipse::init()
	{
		//Pause All
		getRootObject().lock()->pauseAll();

		//Run this
		run();
	}

	void MenuEllipse::update()
	{
		//Debug immediately call this menu
		init();

		const int INTRO_ONE_LOOP_FRAME = 15;
		if (intro_) {
			if (t_ == 0) {
				incRadiusX_ = (radiusX_ - RADIUS_INNER_X) / (float)INTRO_ONE_LOOP_FRAME;
				incRadiusY_ = (radiusY_ - RADIUS_INNER_Y) / (float)INTRO_ONE_LOOP_FRAME;
				incAlpha_ = (255.0f - alpha_) / (float)INTRO_ONE_LOOP_FRAME;
			}
			t_++;
			nowAngle_ += (INTRO_ITEM * 360.f / (float)maxItems_) / (float)INTRO_ONE_LOOP_FRAME;
			radiusX_ -= incRadiusX_;
			radiusY_ -= incRadiusY_;
			alpha_ += incAlpha_;
			if (t_ >= INTRO_ONE_LOOP_FRAME) {
				t_ = 0;
				intro_ = false;
			}
			return;
		}
		if (outro_) {
			if (t_ == 0) {
				incRadiusX_ = (RADIUS_OUTER_X - radiusX_) / (float)INTRO_ONE_LOOP_FRAME;
				incRadiusY_ = (RADIUS_OUTER_Y - radiusY_) / (float)INTRO_ONE_LOOP_FRAME;
				incAlpha_ = alpha_ / (float)INTRO_ONE_LOOP_FRAME;
			}
			t_++;
			nowAngle_ += (OUTRO_ITEM * 360.f / (float)maxItems_) / (float)INTRO_ONE_LOOP_FRAME;
			radiusX_ += incRadiusX_;
			radiusY_ += incRadiusY_;
			alpha_ -= incAlpha_;
			if (t_ >= INTRO_ONE_LOOP_FRAME) {
				//Run All
				getRootObject().lock()->runAll();

				//Kill this obj
				kill();
			}
			return;
		}
		if (!selectable_) {
			t_++;
			nowAngle_ += incAngle_;
			if (t_ >= interval_) {
				t_ = 0;
				selectable_ = true;
			}
			return;
		}

		anim_.step();

		input();

		//Animation & Selectable
		if (lastFrameNow_ != now_) {
			selectable_ = false;
			t_ = 0;
			//Skip
			if (skipUp_ || skipDown_) {
				if (skipUp_) {
					incAngle_ = ((skipItem_ * 360.f / (float)maxItems_) / (float)interval_);
				}
				else if (skipDown_) {
					incAngle_ = -((skipItem_ * 360.f / (float)maxItems_) / (float)interval_);
				}
			}
			//One by one
			else {
				if (lastFrameNow_ == maxItems_ - 1 && now_ == 0) {
						incAngle_ = -((360.f / (float)maxItems_) / (float)interval_);
				}
				else if (lastFrameNow_ == 0 && now_ == maxItems_ - 1) {
						incAngle_ = (360.f / (float)maxItems_) / (float)interval_;
				}
				else {
					if (lastFrameNow_ < now_) {
						incAngle_ = -((360.f / (float)maxItems_) / (float)interval_);
					}
					else {
						incAngle_ = (360.f / (float)maxItems_) / (float)interval_;
					}
				}
			}
			lastFrameNow_ = now_;
		}

		
		if (selectable_) {
			if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
				selectItem();
			}

			if (gplib::input::CheckPush(gplib::input::KEY_BTN1)) {
				outro_ = true;
				t_ = 0;
			}
		}
	}


	void MenuEllipse::render()
	{
		//これを使って、(-)角度を対応になる
		float selx = midPos_.x + (float)cos(firstItemPosAngle_ * M_PI / 180) * radiusX_;
		float sely = midPos_.y + (float)-sin(firstItemPosAngle_ * M_PI / 180) * radiusY_;

#ifdef _DEBUG
		gplib::draw::BoxNC(10, 90, 200, 500, 0.4f, ARGB(255, 0, 0, 0), ARGB(0, 0, 0, 0), 0, 1);
		gplib::font::TextNC(20, 100, 0.35f, std::to_string(firstItemPosAngle_), ARGB(255, 255, 255, 0), 0);
		gplib::font::TextNC(20, 120, 0.35f, std::to_string(nowAngle_), ARGB(255, 255, 255, 0), 0);
#endif

		for (float i = nowAngle_ + firstItemPosAngle_, j = 0; i < 360 + nowAngle_ + firstItemPosAngle_; i += (360 / maxItems_), ++j) {
			//Parametric Equation of an Ellipse
			//x = h + a cos (t)
			//y = k + b sin (t)
			float x = midPos_.x + (float)cos(i * M_PI / 180) * radiusX_;
			float y = midPos_.y + (float)-sin(i * M_PI / 180) * radiusY_;

			//描画手前・奥を決める
			float z = 0.35f + ((float)sin(i * M_PI / 180) / 100.f);

			u_char r = 255;
			u_char g = 255;
			u_char b = 255;

			if (!available_[(u_int)j]) {
				r = g = b = 66;
			}

			//Other Item & Animation
			if (!selectable_ || !(x == selx && y == sely) || !available_[(u_int)j]) {
				gplib::draw::GraphNC((int)x, (int)y, z, "menu", 0, (int)j * 64, 64, 64, 0.0f, 1.0f, 1.0f, (u_char)alpha_, r, g, b);

#ifdef _DEBUG
				gplib::font::TextNC(20, 140 + (int)(j*20), 0.35f, std::to_string((int)i % 360), ARGB(255, 255, 255, 0), 0);
				gplib::font::TextNC(120, 140 + (int)(j * 20), 0.35f, std::to_string(z), ARGB(255, 255, 255, 0), 0);
#endif
			}
			//Now Selecting Item
			else {
				gplib::draw::GraphNC((int)x, (int)y, 0.0f, "menu", 64 * anim_.getAnimNo(), (int)j * 64, 64, 64, 0.0f, 1.0f, 1.0f, (u_char)alpha_, r, g, b);
				
#ifdef _DEBUG
				gplib::font::TextNC(20, 140 + (int)(j * 20), 0.35f, std::to_string((int)i % 360), ARGB(255, 255, 255, 0), 0);
				gplib::font::TextNC(120, 140 + (int)(j * 20), 0.35f, std::to_string(z), ARGB(255, 255, 255, 0), 0);
#endif
			}
		}

		//Description
		//Selecting Item Border & Text
		if (!intro_ && !outro_ && selectable_) {
			float x = midPos_.x + (float)cos(firstItemPosAngle_ * M_PI / 180) * radiusX_;
			float y = midPos_.y + (float)-sin(firstItemPosAngle_ * M_PI / 180) * radiusY_;

			//Border
			//仮素材
			gplib::draw::GraphNC((int)x, (int)y, 0.399f, "menu", 0, 6 * 64, 64, 64, 0.0f, 1.2f, 1.2f, (u_char)alpha_, 128, 128, 128);

			//Text
			std::string s;
			switch (now_) {
			default:
			case 0: s = "profile"; break;
			case 1: s = "(仮)facebook"; break;
			case 2: s = "(仮)whatapp"; break;
			case 3: s = "(仮)vine"; break;
			case 4: s = "(仮)cp"; break;
			case 5: s = "(仮)target"; break;
			case 6: s = "(仮)green violet"; break;
			case 7: s = "back to stage select"; break;
			}

			if (!available_[now_])
				s = "???";

			gplib::draw::BoxCenterNC((int)x, (int)y + 60, 200, 60, 0.4f, ARGB(255, 0, 0, 0), 0x00, 0, 1);
			//gplib::draw::BoxNC((int)x - 80, (int)y + 60, (int)x + 120, (int)y + 120, 0.4f, ARGB(255, 0, 0, 0), ARGB(0, 0, 0, 0), 0, 1);
			gplib::font::TextCenterNC((int)x, (int)y + 60, 0.39f, s.c_str(), ARGB(255,  255, 0, 0), 0);
		}

#ifdef _DEBUG
		//Debug
		gplib::font::TextNC(20, 300, 0.35f, std::to_string(now_), ARGB(255, 255, 0, 0), 0);

		std::stringstream ss;
		ss << "selected: " << debugStr_;
		gplib::font::TextNC(20, 320, 0.35f, ss.str().c_str(), ARGB(255, 255, 0, 0), 0);
#endif
	}

	void MenuEllipse::selectItem() {
		switch (now_) {
		default:
		case 0: debugStr_ = "twitter";
			if (available_[now_])
				insertAsChild(new item::MenuItemPlayerStatus());
			break;
		case 1: debugStr_ = "facebook"; break;
		case 2: debugStr_ = "whatapp"; break;
		case 3: debugStr_ = "vine"; break;
		case 4: debugStr_ = "cp"; break;
		case 5: debugStr_ = "target"; break;
		case 6: debugStr_ = "green violet"; break;
		case 7: debugStr_ = "kfc";
			if (available_[now_]) {
				//Run All
				getRootObject().lock()->runAll();

				auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
				auto event = ci_ext::weak_cast<gameevent::EventTutorial>(getObjectFromRoot("event_tutorial"));
				if (!event.expired())
				{
					event.lock()->~EventTutorial();
				}
				if (!map.expired())
					map.lock()->nextScene();

				kill();
			}
			break;
		}

		if (!available_[now_])
			debugStr_ = "unknown";
	}

	void MenuEllipse::CursorSpeedControl(int interval)
	{
		//Intervalをセット
		interval_ = interval;

		//Skip Flag
		skipUp_ = false;
		skipDown_ = false;

		if (gplib::input::CheckPress(gplib::input::KEY_LEFT, interval)) {
			now_--;
			if (now_ < 0)
				now_ = maxItems_ - 1;
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (gplib::input::CheckPress(gplib::input::KEY_RIGHT, interval)) {
			now_++;
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (gplib::input::CheckPress(gplib::input::KEY_UP, interval)) {
			skipUp_ = true;
			now_ -= skipItem_;
			if (now_ < 0)
				now_ = maxItems_ + now_;
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (gplib::input::CheckPress(gplib::input::KEY_DOWN, interval)) {
			skipDown_ = true;
			now_ += skipItem_;
			if (now_ > maxItems_ - 1)
				now_ = now_ - maxItems_;
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (gplib::input::CheckFree(gplib::input::KEY_UP) && gplib::input::CheckFree(gplib::input::KEY_DOWN) &&
			gplib::input::CheckFree(gplib::input::KEY_LEFT) && gplib::input::CheckFree(gplib::input::KEY_RIGHT)) {
			cnt_ = 0;
			step_ = 0;
		}
		if (cnt_ >= 5) {
			cnt_ = 0;
			step_++;
		}
	}

}