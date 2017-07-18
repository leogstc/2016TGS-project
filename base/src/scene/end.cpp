#include "end.h"
#include "Logo.hpp"
namespace scene
{
	End::End()
		:
		Object("endscene")
	{
		startx = 100;
		cnt = 0;
		flag = false;
	}

	void scene::End::init()
	{
		efMana = insertAsChild(new effect::EffectManager());
		timer_ = insertAsChild(new game::Timer("end_timer",3));
	}

	void End::update()
	{
		++cnt;
		if (!flag)
		{
			auto efMana_ = ci_ext::weak_cast<effect::EffectManager>(efMana);
			if (!efMana_.expired())
			{
				flag = true;
				efMana.lock()->createEffect("fadeoutblack");
			}
		}

		auto timer = ci_ext::weak_cast<game::Timer>(timer_);
		if (!timer.expired())
		{
			if (timer.lock()->isTimeOver())
			{
				auto efMana_ = ci_ext::weak_cast<effect::EffectManager>(efMana);
				if (!efMana_.expired())
				{
					efMana_.lock()->createEffect("fadeinblack");
				}

				sleep(45);
			}
		}
		//insertAsChild(new Logo);
		//kill();
	}

	void End::resume()
	{
		//insertToParent(new Logo());
		getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::LOGO));
		kill();
	}

	void End::render()
	{
		if (!flag||cnt<=10) return;
		D3DCOLOR color = ARGB(255, 255, 255, 255);
		RECT rt{ 100,100,100 + 400,100 + 400 };

		gplib::draw::GraphLeftTop(startx, 50, 0.1f, "endText1", 0, 0, 405, 88,0,nullptr,0.7f,0.7f);
		gplib::draw::GraphLeftTop(startx + 20, 100, 0.1f, "programmer1", 0, 0, 416, 62);
		gplib::draw::GraphLeftTop(startx + 500, 100, 0.1f, "programmer2", 0, 0, 648, 62);
		gplib::draw::GraphLeftTop(startx + 20, 200, 0.1f, "programmer3", 0, 0, 329, 61);
		gplib::draw::GraphLeftTop(startx + 500, 200, 0.f, "programmer4", 0, 0, 616, 64);

		gplib::draw::GraphLeftTop(startx + 500, 300, 0.f, "endText2", 0, 0, 372, 93, 0, nullptr, 0.7f, 0.7f);
		gplib::draw::GraphLeftTop(startx + 530, 350, 0.f, "designer1", 0, 0, 246, 82, 0, nullptr, 0.8f, 0.8f);
		gplib::draw::GraphLeftTop(startx + 530, 400, 0.f, "designer2", 0, 0, 628, 86, 0, nullptr, 0.8f, 0.8f);

		gplib::font::TextCenterNC(startx, 300, 0.f, "‚¨ŽØ‚è‚µ‚½‘fÞ",color,1);
		gplib::font::TextCenterNC(startx + 100, 350, 0.f, "ŠÃ’ƒ‚Ì‰¹ŠyH–[", color, 1);
		gplib::font::TextCenterNC(startx + 80, 400, 0.f, "Œø‰Ê‰¹ƒ‰ƒ{", color, 1);
		gplib::font::TextCenterNC(startx + 265, 450, 0.f, "GAME ART 2D...http://www.gameart2d.com/", color, 1);
		gplib::font::TextCenterNC(startx, 500, 0.f, "SpecialThanks", color, 1);
		gplib::font::TextCenterNC(startx+160, 550, 0.f, "2ci1‚Ì‰ž‰‡‚µ‚Ä‚­‚ê‚½‚Ý‚ñ‚È", color, 1);
		gplib::font::TextCenterNC(startx+240, 600, 0.f, "ƒXƒe[ƒW§ì‚ð‹¦—Í‚µ‚Ä‚­‚ê‚½“n‰Á•~@KG‚³‚Ü",color, 1);
		gplib::font::TextCenterNC(startx+60, 650, 0.f, "Šeæ¶•û", color, 1);

	//	gplib::draw::Graph(startx , 150, 0.1f, "programmer3", 0, 0, 329, 61, 0, 0.7f, 0.7f);
		//gplib::font::TextCenterNC(rt, 0.1f, "–k—Ñ@ƒŽ÷", ARGB(255, 255, 255, 255), 1);
		
		//gplib::font::TextCenterNC((int)posx, (int)posy, 0.03f, item, color, 0);

	}
}