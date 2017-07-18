#pragma once

#include "../lib/gplib.h"
#include "../lib/ci_ext/vec2.hpp"

namespace camera {
	struct StageCamera {
		RECT limitPos;
		ci_ext::Vec2f nowLookAt;
		ci_ext::Vec2f velocity;

		StageCamera() {
			limitPos = { 0, 0, 0, 0 };
			nowLookAt = ci_ext::Vec2f::zero();
			velocity = ci_ext::Vec2f::zero();
		}
	};
}