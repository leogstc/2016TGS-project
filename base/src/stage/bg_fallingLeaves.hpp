#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

namespace map {
	namespace bg {
		class BgFallingLeaves : public ci_ext::Object {
			struct Leaf {
				int type;

				float posx, posy;
				float spd; //Falling speed
				float sizeScaleX, sizeScaleY;
				float swingStr; //Swing strength @ x-pos (calc by cos()) | this value has range between 0~1
				float swingStep; //Step of swinging each frame
				float degree; //Degree of rotation of leaf (calculate x-scale) | flip

				//render
				float scalex;
				float alpha;
			};

			const float FALL_SPD = 0.3f; //Gravity -> for random
			const int IMG_WIDTH = 72;
			const int IMG_HEIGHT = 72;

			Leaf leaves_[30];


		public:
			BgFallingLeaves()
				:
				Object("bg_falling_leaves")
			{
				gplib::draw::LoadObject("bg_falling_leaf", "res/gra/bg/falling_leaf.png");
			}

			~BgFallingLeaves()
			{
				gplib::draw::DeleteObject("bg_falling_leaf");
			}

			void initLeaf(Leaf& leaf)
			{
				//Type
				leaf.type = gplib::math::GetRandom(0, 3);

				//Starting position of leaves
				leaf.posy = gplib::math::GetRandom(-250.0f, (float)-IMG_HEIGHT / 2.0f);
				leaf.posx = (float)gplib::math::GetRandom(0, gplib::system::WINW);

				//Falling speed
				leaf.spd = gplib::math::GetRandom(1.0f, FALL_SPD + 2.0f);

				//Random width and height
				leaf.sizeScaleX = leaf.sizeScaleY = gplib::math::GetRandom(0.3f, 1.5f);

				//Swing
				leaf.swingStr = 0;
				leaf.swingStep = gplib::math::GetRandom(0.05f, 0.1f);
				leaf.degree = -90; // -90 (270) degree is corresponding to swingStr = 0

				//Render
				leaf.alpha = 255.0f;
			}

			void init() override
			{
				for (auto& leaf : leaves_) {
					initLeaf(leaf);
				}
			}

			void update() override
			{
				for (auto& leaf : leaves_) {
					//float prevSwingx = leaf.spd * cos(leaf.swingStr - leaf.swingStep);
					float swingx = leaf.spd * cos(leaf.swingStr);

					//Flip
					//if (prevSwingx < 0 && swingx >= 0) {
					//	leaf.degree = 180;
					//}
					//else if (prevSwingx >= 0 && swingx < 0) {
					//	leaf.degree = 0;
					//}

					//Move
					leaf.posy += leaf.spd;
					leaf.posx += swingx;

					//Calculate scale at that degree rotation (including flip/unflip)
					float scale = (1 - (float)((int)leaf.degree % 180) / 90) * (leaf.degree >= 180 ? -1 : 1);
					leaf.scalex = scale * leaf.sizeScaleX;

					// +Degree
					leaf.degree += float(leaf.swingStep / M_PI * 180);
					//leaf.degree = (int)leaf.degree % 360;
					if (leaf.degree >= 360)
						leaf.degree = leaf.degree - 360;

					//Alpha & Out of Screen Check
					if (leaf.posy > gplib::system::WINH - (IMG_HEIGHT * 4)) {
						leaf.alpha -= 0.5f;
						if (leaf.alpha < 0)
							leaf.alpha = 0;
					}
					if (leaf.posy > gplib::system::WINH + IMG_HEIGHT) {
						initLeaf(leaf);
					}

					//Next Swing
					leaf.swingStr += leaf.swingStep;
				}
			}

			void render() override
			{
				for (auto& leaf : leaves_) {
					gplib::draw::Graph(leaf.posx, leaf.posy, 1.0f, "bg_falling_leaf", 0, leaf.type * IMG_HEIGHT, IMG_WIDTH, IMG_HEIGHT, 0.0f, leaf.scalex, leaf.sizeScaleY, (u_char)leaf.alpha);
				}
			}
		};
	}
}