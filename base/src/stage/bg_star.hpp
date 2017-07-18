#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

namespace map {
	namespace bg {
		class BgStar : public ci_ext::Object {
			POINT stars_[200];
			int spd_;

		public:
			BgStar(int spd = -2.0f)
				:
				Object("bg_star"), spd_(spd)
			{}

			void init() override
			{
				for (int i = 0; i < 200; ++i) {
					stars_[i].x = gplib::math::GetRandom(0, gplib::system::WINW);
					stars_[i].y = gplib::math::GetRandom(0, gplib::system::WINH);
				}
			}

			void update() override
			{
				for (int i = 0; i < 200; ++i) {
					//¯‚ÌˆÚ“®
					stars_[i].x += spd_;

					//‰æ–Ê‚Éo‚½‚çAV‚µ‚¢¯‚ðì‚é
					if (stars_[i].x < 0) {
						stars_[i].x = gplib::system::WINW + 1;
						stars_[i].y = gplib::math::GetRandom(0, gplib::system::WINH);
					}
				}
			}

			void render() override
			{
				//•`‰æ
				for (int i = 0; i < 200; ++i) {
					gplib::draw::BoxNC(stars_[i].x, stars_[i].y, stars_[i].x + 1, stars_[i].y + 1, 1.0f, ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), 0, 1);
				}
			}
		};
	}
}