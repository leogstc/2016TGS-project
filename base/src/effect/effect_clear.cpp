#include "effect_clear.h"

#include "../scene/loading.h"
#include "../player/player.h" //Lock Player

//Do after all text gone
#include "../stage/map.h"
#include "../scene/end.h"

#define FULLTEXT_FREEZE_TIME 60
#define WAIT_LAST 60 //Wait after all text disappeared

namespace effect {
	EffectClear::EffectClear(int x, int y)
		:
		EffectBase("effect_clear", gplib::system::WINW / 2, gplib::system::WINH / 2),
		resname_("effect_clear"),
		sizex_(100), sizey_(128), i_(HALF)
	{
		dispFullText = false;
		finalFlag = false;
		for (auto& e : eff_) {
			e.flag = false;
		}
	}

	void EffectClear::init()
	{
		auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		if (!player.expired())
		{
			player.lock()->setVelocity(0.0f, 0.0f);
			player.lock()->setPlayerState(player::State::STAND);
			player.lock()->setControlable(false);

		}
	}

	void EffectClear::update()
	{
		if (finalFlag) {
			cnt_++;

			if (cnt_ > WAIT_LAST) {
				auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
				if (!map.expired()) {
					//getRootObject().lock()->insertAsChild(new scene::End());
					getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::END));
					map.lock()->getParentPtr().lock()->kill();
					gplib::bgm::Delete("bossbgm");
					//map.lock()->nextScene();
				}
				kill();
			}

			return;
		}



		//Create each char & Create full text
		if (i_ >= 0 && cnt_ % 10 == 0) {
			if (i_ != 0)
				createEffects(2);
			else
				createEffects(1);
			i_--; //Move to next pos
		}
		//Check Each Char
		else if (!dispFullText) {
			bool next = true;
			for (auto& e : eff_) {
				if (!e.stopScale) {
					next = false;
					break;
				}
			}
			if (next) {
				//Delete all chars
				for (auto& e : eff_) {
					e.flag = false;
				}
				dispFullText = true;
				//Create Full Text
				createText();
				//Reset counter
				cnt_ = 0;
			}
		}
		else {
			if (!eff_[0].flag) {
				finalFlag = true;
				cnt_ = 0;
				return;
			}
		}
		cnt_++;

		//Update Children Obj
		for (auto& e : eff_) {
			//Each Char
			if (!dispFullText) {
				if (e.flag & !e.stopScale) {
					//ägëÂ
					if (!e.flipScale) {
						e.alpha += 12.75f; // 255/20
						//if (e.alpha >= 255) {
						//	e.alpha = 255;
						//}

						e.scale += 0.05f; // 1.5 -> 2.0 in 10 frames

						e.cnt++;
						if (e.cnt >= 10) {
							e.flipScale = true;
							e.cnt = 0;
						}
					}
					//èkè¨
					else {
						e.alpha += 12.75f; // 255/20
						if (e.alpha >= 255) {
							e.alpha = 255;
						}

						e.scale -= 0.1f; // 2.0 -> 1.0 in 10 frames

						e.cnt++;
						if (e.cnt >= 10)
							e.stopScale = true;
					}
				}
			}
			//Full Text
			else {
				auto& e = eff_[0];
				if (cnt_ > FULLTEXT_FREEZE_TIME) {
					//ägëÂ
					if (!e.flipScale) {
						e.alpha -= 4.25f; // 255/60
						//if (e.alpha <= 0) {
						//	e.alpha = 0;
						//}

						e.scale += 0.025f; // 1.0 -> 1.5 in 20 frames

						e.cnt++;
						if (e.cnt >= 20) {
							e.flipScale= true;
							e.cnt = 0;
						}
					}
					//èkè¨
					else {
						e.alpha -= 4.25f; // 255/60
						if (e.alpha <= 0) {
							e.alpha = 0;
						}

						e.scale -= 0.0375f; // 1.5 -> 0 in 40 frames

						e.cnt++;
						if (e.cnt >= 40) {
							e.flag = false;
						}
					}
				}
			}
		}
	}

	void EffectClear::renderLater()
	{
		//Each Char
		if (!dispFullText) {
			for (auto& e : eff_) {
				if (e.flag) {
					gplib::draw::GraphNC(posx_ + (e.i * sizex_), posy_, posz_, resname_, ((e.i + HALF) * sizex_), 0, sizex_, sizey_, 0.0f, e.scale, e.scale, (u_char)e.alpha);
				}
			}
		}
		//Full Text
		else {
			auto& e = eff_[0];
			gplib::draw::GraphNC(posx_, posy_, posz_, resname_, 0, 0, sizex_ * SIZE, sizey_, 0.0f, e.scale, e.scale, (u_char)e.alpha);
		}
	}

	void EffectClear::createEffects(int no)
	{
		for (auto& e : eff_) {
			if (!e.flag) {
				e.i = i_ * (int)pow(-1, no);
				e.scale = 1.5f;
				e.flipScale = false;
				e.stopScale = false;
				e.alpha = 0.0f;
				e.cnt = 0;
				e.flag = true;

				no--;
				if (no == 0)
					break;
			}
		}
	}

	void EffectClear::createText()
	{
		auto& e = eff_[0];
		e.i = 0;
		e.scale = 1.0f;
		e.flipScale = false;
		e.alpha = 255.0f;
		e.cnt = 0;
		e.flag = true;
	}

}