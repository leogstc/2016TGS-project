#pragma once

#include "effectBase.h"
#include "../config.h"

namespace effect {
	class EffectMahoujin : public EffectBase {
		struct Mahoujin {
			bool flag;
			//int x, y;
			int color; //src y slot
			//int sizex, sizey;
			float scalex, scaley;
			float angle;
			float alpha;
			//int cnt;
		};

		Mahoujin eff_[5];
		const std::string resname_;
		int color_;
		const int sizex_, sizey_;

	public:
		EffectMahoujin(int x, int y)
			:
			EffectBase("effect_mahoujin", x, y), resname_("effect_mahoujin"),
			sizex_(64), sizey_(64)
		{
			color_ = gplib::math::GetRandom(0, 7);
			for (auto& e : eff_) {
				e.flag = false;
				//e.color = color;
			}
		}

		void update() override
		{
			//Create Child Obj & Delete This Obj
			if (cnt_ < 20 && cnt_ % 4 == 0) {
				createEffect();
			}
			//Last effect create @ cnt_ = 16 -> last effect alpha = 0 @ 26th frame
			//thus, check all flag @ cnt_ = 42
			else if (cnt_ > 42) {
				bool end = true;
				for (auto& e : eff_) {
					if (e.flag) {
						end = false;
						break;
					}
				}
				if (end) {
					kill();
				}
			}
			cnt_++;

			//Update Children Obj
			for (auto& e : eff_) {
				if (e.flag) {
					e.alpha -= 10.0f;
					if (e.alpha <= 0) {
						e.alpha = 0;
						e.flag = false;
					}

					e.scalex += 0.05f * config.GAME_SCALE;
					e.scaley += 0.05f * config.GAME_SCALE;

					//if (e.cnt > 26)
					//	e.flag = false;
					//e.cnt++;
				}
			}
		}

		//D3DX IMAGE & TEXTUREの重なっている半透明描画のデバッグ
		//描画順番により黒い部分と他の画像の透過のデバッグ
		void renderLater() override
		{
			for (auto& e : eff_) {
				if (e.flag) {
					gplib::draw::Graph(posx_, posy_, posz_, resname_, 0, color_ * sizey_, sizex_, sizey_, e.angle, e.scalex, e.scaley, (u_char)e.alpha);
				}
			}
		}

		void createEffect()
		{
			for (auto& e : eff_) {
				if (!e.flag) {
					//e.x = posx_;
					//e.y = posy_;
					//e.sizex = e.sizey = 64;
					e.scalex = 0.1f * config.GAME_SCALE;
					e.scaley = 1.0f * config.GAME_SCALE;
					e.angle = gplib::math::GetRandom(0.0f, 180.0f); //角度をランダム
					e.alpha = 255.0f;
					//e.cnt = 0;
					e.flag = true;
					break;
				}
			}
		}
	};
}