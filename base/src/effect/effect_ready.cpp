#include "effect_ready.h"
#include "../stage/map.h"

#define STAR_ROT_SPD 3.0f
#define WAIT_ENTRY 20 //wait time (frame) before anything appear
#define WAIT_TIME 60 //full text display frame

namespace effect {
	EffectReady::EffectReady(int x, int y)
		:
		EffectBase("effect_ready", gplib::system::WINW / 2, y),
		resname_("effect_ready"), starRes_("effect_ready_star"),
		sizex_(500), sizey_(128),
		scale_(1.0f), alpha_(0.0f),
		flipScale_(false), out_(false),
		starSize_(64)
	{}

	void EffectReady::init()
	{
		int i = 0;
		
		float offsetx[] = { -10, 45, -30 };
		float offsety[] = { -20, -15, 35 };
		float scale[] = { 2.0f, 1.75f, 1.5f, 2.0f, 1.5f, 1.75f };
		for (auto& s : stars_) {
			s.offsetx = offsetx[i % 3];
			s.offsety = offsety[i % 3];
			if (i < 3) {
				s.pic = (i + 2) % 3;
				s.degStep = STAR_ROT_SPD;
			}
			else {
				s.pic = i % 3;
				s.degStep = -STAR_ROT_SPD;
			}
			s.deg = gplib::math::GetRandom(0.0f, 360.0f);
			s.scale = scale[i];

			i++;
		}
	}

	void EffectReady::update()
	{
		if (!out_) {
			if (cnt_ < WAIT_ENTRY) {
				//‰½‚à‚µ‚È‚¢
			}
			else if (cnt_ < WAIT_ENTRY + 20) {
				//Text
				//scale_ -= 0.05f;
				//Both
				alpha_ += 12.75f;
				if (alpha_ >= 255) {
					alpha_ = 255;
				}

				//Star
				for (int i = 0; i < 6; ++i) {
					auto& s = stars_[i];
					s.scale -= 0.05f;
					if (i < 3) {
						s.posx = (posx_ - sizex_ / 2 * scale_) + s.offsetx;
						s.posy = (posy_ - sizey_ / 2 * scale_) + s.offsety;
					}
					else {
						s.posx = (posx_ + sizex_ / 2 * scale_) - s.offsetx;
						s.posy = (posy_ + sizey_ / 2 * scale_) - s.offsety;
					}
				}

			}
			else if (cnt_ > WAIT_ENTRY + 20 + WAIT_TIME) {
				out_ = true;
				cnt_ = 0;
			}

			//Star Angle
			for (auto& s : stars_) {
				s.deg += s.degStep;
			}
		}
		else {
			if (cnt_ < 10) {
				//Text
				scale_ -= 0.05f;
				//Star
				for (int i = 0; i < 6; ++i) {
					auto& s = stars_[i];
					s.scale -= 0.05f;
					if (i < 3) {
						s.posx = (posx_ - sizex_ / 2 * scale_) + s.offsetx;
						s.posy = (posy_ - sizey_ / 2 * scale_) + s.offsety;
					}
					else {
						s.posx = (posx_ + sizex_ / 2 * scale_) - s.offsetx;
						s.posy = (posy_ + sizey_ / 2 * scale_) - s.offsety;
					}
				}
			}
			else if (cnt_ < 20) {
				//Text
				scale_ += 0.25f;

				//Both
				alpha_ -= 25.5f;
				if (alpha_ <= 0) {
					alpha_ = 0;
				}

				//Star
				for (int i = 0; i < 6; ++i) {
					auto& s = stars_[i];
					s.scale += 0.25f;
					if (i < 3) {
						s.posx = (posx_ - sizex_ / 2 * scale_) + s.offsetx;
						s.posy = (posy_ - sizey_ / 2 * scale_) + s.offsety;
					}
					else {
						s.posx = (posx_ + sizex_ / 2 * scale_) - s.offsetx;
						s.posy = (posy_ + sizey_ / 2 * scale_) - s.offsety;
					}
				}
			}

			//Star Angle
			for (auto& s : stars_) {
				s.deg += s.degStep;
			}


			//Has 10 frames delay after full text disappeared
			if (cnt_ > 30) {
				kill();
				auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
				map.lock()->createPlayer();

			}
		}

		cnt_++;
	}

	//void EffectReady::render()
	//{
	//	gplib::draw::GraphNC(posx_, posy_, posz_, resname_, 0, 0, sizex_, sizey_, 0.0f, scale_, scale_, (u_char)alpha_);
	//	for (auto& s : stars_) {
	//		gplib::draw::GraphNC(s.posx, s.posy, posz_, starRes_, 0, s.pic * starSize_, starSize_, starSize_, s.deg, s.scale, s.scale, (u_char)alpha_);
	//	}
	//}

	void effect::EffectReady::renderLater()
	{
		gplib::draw::GraphNC(posx_, posy_, posz_, resname_, 0, 0, sizex_, sizey_, 0.0f, scale_, scale_, (u_char)alpha_);
		for (auto& s : stars_) {
			gplib::draw::GraphNC(s.posx, s.posy, posz_, starRes_, 0, s.pic * starSize_, starSize_, starSize_, s.deg, s.scale, s.scale, (u_char)alpha_);
		}

	}
}