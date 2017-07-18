#include "effect_fadeOutBlack.h"

namespace effect
{
	Effect_FadeOutBlack::Effect_FadeOutBlack(int x,int y)
		:
		EffectBase("fade_out_black", x, y)
	{
		alpha_ = 255;
	}

	void Effect_FadeOutBlack::update()
	{
		camera_ = gplib::camera::GetLookAt();
		posx_ = (int)camera_.x - (gplib::system::WINW / 2);
		posy_ = (int)camera_.y - (gplib::system::WINH / 2);
		alpha_-= 3;

		if (alpha_ <= 0)
			kill();
	}

	void Effect_FadeOutBlack::render()
	{
		gplib::draw::Box(posx_, posy_, posx_ + gplib::system::WINW, posy_ + gplib::system::WINH, 0.f,
			ARGB(alpha_, 0, 0, 0), ARGB(alpha_, 0, 0, 0), 1, true);
	}

}