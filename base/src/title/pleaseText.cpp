#include "pleaseText.h"

namespace titleObj
{
	TitleText::TitleText(const int& MAX_CNT)
		:
		TitleObj("titleText", "please", ci_ext::Vec3f::zero(),0.f,0.f),
		MAX_CNT_(MAX_CNT)
	{
	}

	void TitleText::update()
	{
		if (cnt == 6)
			show = true;
		
		if (++cnt >= MAX_CNT_)
		{
			cnt = 0;
			show = false;
		}
	}

	void titleObj::TitleText::render()
	{
		if (show)
			gplib::draw::Graph((float)gplib::system::WINW / 2.f, 600.f, 0.f,
				"please", 0, 0, 248, 48);
	}
}