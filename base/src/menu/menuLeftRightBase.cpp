#include "menuLeftRightBase.h"

#include "../lib/gplib.h"

namespace menu {
	MenuLeftRightBase::MenuLeftRightBase(const std::string& objectName, int maxOption, int now)
		:
		Object(objectName), OptionBase(maxOption, now),
		skipUp_(false), skipDown_(false)
	{}

	void MenuLeftRightBase::init()
	{}

	void MenuLeftRightBase::update()
	{
		input();
	}

	void MenuLeftRightBase::render()
	{
		//gplib::font::TextNC(20, 300, 0.0f, std::to_string(step_) + "," + std::to_string(cnt_), ARGB(255, 255, 0, 0), 0);
		//gplib::font::TextNC(20, 320, 0.0f, std::to_string(now_) + "/" + std::to_string(maxOption_ - 1), ARGB(255, 255, 0, 0), 0);
	}

	void MenuLeftRightBase::input()
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

	void MenuLeftRightBase::CursorSpeedControl(int interval)
	{
		//Interval‚ðƒZƒbƒg
		interval_ = interval;

		//Skip Flag
		skipUp_ = false;
		skipDown_ = false;

		const int MOVE_SKIP = 5;

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
		if (gplib::input::CheckPress(gplib::input::KEY_DOWN, interval)) {
			skipDown_ = true;
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