#include "effect_gameOver.h"
#include "../lib/easing.h"
namespace effect
{
	using namespace gplib;
	using namespace system;
	EffectGameOver::EffectGameOver(int x,int y)
		:
		EffectBase("gameover",WINW/2,WINH/2)
	{
		posy_ = -100;
		easing::Init();
		easing::Set("gameover", easing::BOUNCEOUT, -100.f, (float)gplib::system::WINH / 2.f, 150);
		easing::Start("gameover");
	}

	void EffectGameOver::update()
	{
		easing::Step();
		posy_ = (int)easing::GetPos("gameover");
		
	}

	void EffectGameOver::render()
	{
		draw::Graph((float)posx_, (float)posy_, 0.1f, "effect_gameover", 0, 0, 475, 101);
	}


}