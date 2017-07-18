#pragma once

#include "../lib//gplib.h"
#include "../lib/ci_ext/object.hpp"

namespace player {
	class PlayerDeadManager : public ci_ext::Object {
		Point2f pos_;
		int t_;

	public:
		PlayerDeadManager(float x, float y);

		void init() override;
		void update() override;
		void render() override;
	};

	class PlayerDeadCircle : public ci_ext::Object {
		Point2f pos_;
		Point2f speed_;
		float radius_;

	public:
		PlayerDeadCircle(float x, float y, float degree, float speed, float radius);

		void init() override;
		void update() override;
		void render() override;
	};
}