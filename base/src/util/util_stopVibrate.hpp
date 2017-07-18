#pragma once
#include "../lib/ci_ext/object.hpp"
#include "../lib/xinput.hpp"

namespace util {
	class StopVibrate : public ci_ext::Object {
		int t_;
	public:
		StopVibrate(int stopInFrame) : Object("stop_vibrate"), t_(stopInFrame) {}
		void update() override
		{
			t_--;
			if (t_ <= 0) {
				XInput::Vibration(0, 0);
				kill();
			}
		}
	};
}