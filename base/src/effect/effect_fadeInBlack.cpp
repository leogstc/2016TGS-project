#include "effect_fadeInBlack.h"
namespace effect
{
	Effect_FadeInBlack::Effect_FadeInBlack(int x, int y)
		:
		EffectBase("fade_in_black",x,y)
	{
		alpha_ = 0;
	}

	void Effect_FadeInBlack::init()
	{

	}

	void Effect_FadeInBlack::update()
	{
		camera_ = gplib::camera::GetLookAt();
		posx_ = (int)camera_.x - (gplib::system::WINW / 2);
		posy_ = (int)camera_.y - (gplib::system::WINH / 2);
		alpha_ += 5;
		
		if (alpha_ >= 255)
			kill();
	}

	void Effect_FadeInBlack::renderLater()
	{
		gplib::draw::Box(posx_, posy_,posx_ + gplib::system::WINW,posy_ + gplib::system::WINH, 0.f,
			ARGB(alpha_, 0, 0, 0), ARGB(alpha_, 0, 0, 0), 1, true);
	}


}