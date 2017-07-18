#include "menuUpDownBase.h"
#include "../player/mover.h"
#include "../lib/gplib.h"
#include "../lib/xinput.hpp"

namespace menu {
	MenuUpDownBase::MenuUpDownBase(const std::string& objectName, int maxOption, int now)
		:
		Object(objectName), OptionBase(maxOption, now), limit_(RECT())
	{}

	MenuUpDownBase::~MenuUpDownBase()
	{
		gplib::se::Delete("cursormove");
	}

	void MenuUpDownBase::init()
	{
		gplib::se::LoadFile("cursormove", "res/snd/cursor2.wav");
	}

	void MenuUpDownBase::update()
	{
		gplib::se::EndCheck();
		//XInput::CheckKey();
		input();
	}

	void MenuUpDownBase::render()
	{
		//gplib::font::TextNC(20, 300, 0.0f, std::to_string(step_) + "," + std::to_string(cnt_), ARGB(255, 255, 0, 0), 0);
		//gplib::font::TextNC(20, 320, 0.0f, std::to_string(now_) + "/" + std::to_string(maxOption_ - 1), ARGB(255, 255, 0, 0), 0);
	}

	void MenuUpDownBase::input()
	{
		if (step_ == 0) {
			CursorSpeedControl(15);
		}
		else if (step_ == 1) {
			CursorSpeedControl(8);
		}
		else {
			CursorSpeedControl(4);
		}
	}

	void MenuUpDownBase::CursorSpeedControl(int interval)
	{
		const int MOVE_SKIP = 5;
		auto mover_ = std::make_shared<KeyMover>();


		if (XInput::CheckPress(XInput::ButtonsID::up, interval)
			|| gplib::input::CheckPress(gplib::input::KEY_UP, interval)
			) {
			now_--;
			if (now_ < 0)
				now_ = maxItems_ - 1;
			now_ %= maxItems_;
			gplib::se::Play("cursormove");
			gplib::se::EndCheck();
			cnt_++;
		}
		if (XInput::CheckPress(XInput::ButtonsID::down, interval)
			|| gplib::input::CheckPress(gplib::input::KEY_DOWN, interval)
			) {
			now_++;
			now_ %= maxItems_;
			gplib::se::Play("cursormove");
			gplib::se::EndCheck();
			cnt_++;
		}
		if (
			XInput::CheckPress(XInput::ButtonsID::left, interval)
			|| gplib::input::CheckPress(gplib::input::KEY_LEFT, interval)
			) {
			if (now_ > 0) {
				now_ -= MOVE_SKIP;
				if (now_ < 0)
					now_ = 0;
			}
			else {
				now_ = maxItems_ - 1;
			}
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (
			XInput::CheckPress(XInput::ButtonsID::right, interval)
			|| gplib::input::CheckPress(gplib::input::KEY_RIGHT, interval)
			) {
			if (now_ < maxItems_ - 1) {
				now_ += MOVE_SKIP;
				if (now_ > maxItems_ - 1)
					now_ = maxItems_ - 1;
			}
			else {
				now_ = 0;
			}
			now_ %= maxItems_;
			//se::Play("move");
			cnt_++;
		}
		if (
			(XInput::CheckFree(XInput::ButtonsID::up) && XInput::CheckFree(XInput::ButtonsID::down) &&
			XInput::CheckFree(XInput::ButtonsID::left) && XInput::CheckFree(XInput::ButtonsID::right))
			|| (gplib::input::CheckFree(gplib::input::KEY_UP) && gplib::input::CheckFree(gplib::input::KEY_DOWN) &&
			gplib::input::CheckFree(gplib::input::KEY_LEFT) && gplib::input::CheckFree(gplib::input::KEY_RIGHT))
			) {
			cnt_ = 0;
			step_ = 0;
		}
		if (cnt_ >= 5) {
			cnt_ = 0;
			step_++;
		}
	}
}