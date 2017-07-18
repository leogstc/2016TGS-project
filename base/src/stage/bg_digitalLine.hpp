/*
Motion Calculation Ref.
http://www.engineeringtoolbox.com/motion-formulas-d_941.html
http://www.schoolphysics.co.uk/age14-16/Mechanics/Motion/text/Equations_of_motion/index.html
*/


#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

namespace map {
	namespace bg {
		class BgDigitalLine : public ci_ext::Object {
			struct LineY {
				float posy;
				float spd;
			};
			const int LINE_X = 31; //No. of lines in x-axis
			const int LINE_Y = 4; //No. of lines in y-axis for each direction
			const float OUTER_WIDTH_SCALE = 5.5f; //Width Scale of Screen for LINE_X
			const int HALF_SPACE_HEIGHT = 25; //Space in middle / 2
			const float INIT_SPEED = 1.0f; //LINE_Y
			const float INC_SPEED = 0.2f; //LINE_Y

			//LineY* yUp;
			//LineY* yDown;
			LineY yUp[4]; //Array of LINE_Y
			LineY yDown[4];

		public:
			BgDigitalLine()
				:
				Object("bg_digital_line")
			{}
			
			~BgDigitalLine()
			{
				//delete[] yUp;
				//delete[] yDown;
			}

			void init() override
			{
				//yUp = new LineY[LINE_Y];
				//yDown = new LineY[LINE_Y];

				//Length to move
				int yLen = (gplib::system::WINH / 2) - HALF_SPACE_HEIGHT;

				//Place at first position
				LineY tmp;
				tmp.posy = (float)yLen;
				tmp.spd = INIT_SPEED;
				//Calculate time to travel
				float time = 0;
				while (true) {
					tmp.spd += INC_SPEED;
					tmp.posy -= tmp.spd;
					time++;
					if (tmp.posy < 0) {
						break;
					}
				}
				
				for (int i = 0; i < LINE_Y; ++i) {
					//Calculate traveled distance & speed at that time
					float t = time * i / LINE_Y;
					float distance = (INIT_SPEED * t) + ((INC_SPEED * t * t) / 2);
					float spd = INIT_SPEED + (INC_SPEED * t);

					yUp[i].posy = yLen - distance;
					yDown[i].posy = (gplib::system::WINH / 2) + HALF_SPACE_HEIGHT + distance;

					yUp[i].spd = spd;
					yDown[i].spd = spd;
				}
			}

			void update() override
			{
				for (int i = 0; i < LINE_Y; ++i) {
					yUp[i].spd += INC_SPEED;
					yDown[i].spd += INC_SPEED;

					yUp[i].posy -= yUp[i].spd;
					yDown[i].posy += yDown[i].spd;

					if (yUp[i].posy < 0) {
						yUp[i].posy = (float)((gplib::system::WINH / 2) - HALF_SPACE_HEIGHT);
						yUp[i].spd = INIT_SPEED;
					}
					if (yDown[i].posy > gplib::system::WINH + INIT_SPEED) {
						yDown[i].posy = (float)((gplib::system::WINH / 2) + HALF_SPACE_HEIGHT);
						yDown[i].spd = INIT_SPEED;
					}
				}
			}

			void render() override
			{
				const int offsetX = 10;
				//X-Axis Calc
				int* topUp;
				int* topDown;
				int* bottomUp;
				int* bottomDown;
				topUp = new int[LINE_X];
				topDown = new int[LINE_X];
				bottomUp = new int[LINE_X];
				bottomDown = new int[LINE_X];

				int lenAfterOffset = gplib::system::WINW - (offsetX * 2);

				for (int i = 0; i < LINE_X; ++i) {
					topDown[i] = offsetX + (lenAfterOffset * i / (LINE_X - 1));
					bottomUp[i] = offsetX + (lenAfterOffset * i / (LINE_X - 1));
				}

				int wideStartX, wideLength;
				wideLength = int(gplib::system::WINW * OUTER_WIDTH_SCALE);
				wideStartX = (gplib::system::WINW - wideLength) / 2;
				for (int i = 0; i < LINE_X; ++i) {
					topUp[i] = wideStartX + wideLength * i / (LINE_X - 1);
					bottomDown[i] = wideStartX + wideLength * i / (LINE_X - 1);
				}

				//Draw
				for (int i = 0; i < LINE_X; ++i) {
					//Upper X
					gplib::draw::LineNC(topDown[i], (gplib::system::WINH / 2) - HALF_SPACE_HEIGHT, topUp[i], 0, 1.0f, ARGB(255, 0, 255, 0), 1);
					//Lower X
					gplib::draw::LineNC(bottomUp[i], (gplib::system::WINH / 2) + HALF_SPACE_HEIGHT, bottomDown[i], gplib::system::WINH, 1.0f, ARGB(255, 0, 255, 0), 1);

				}
				for (int i = 0; i < LINE_Y; ++i) {
					//Fix Line
					int up = (gplib::system::WINH / 2) - HALF_SPACE_HEIGHT;
					int down = (gplib::system::WINH / 2) + HALF_SPACE_HEIGHT;
					gplib::draw::LineNC(0, up, gplib::system::WINW, up, 1.0f, ARGB(255, 0, 255, 0), 1);
					gplib::draw::LineNC(0, down, gplib::system::WINW, down, 1.0f, ARGB(255, 0, 255, 0), 1);

					//Moving Upper Y
					gplib::draw::LineNC(0, (int)yUp[i].posy, gplib::system::WINW, (int)yUp[i].posy, 1.0f, ARGB(255, 0, 255, 0), 1);
					//Moving Lower Y
					gplib::draw::LineNC(0, (int)yDown[i].posy, gplib::system::WINW, (int)yDown[i].posy, 1.0f, ARGB(255, 0, 255, 0), 1);

				}

				delete[] topUp;
				delete[] topDown;
				delete[] bottomUp;
				delete[] bottomDown;
			}
		};
	}
}