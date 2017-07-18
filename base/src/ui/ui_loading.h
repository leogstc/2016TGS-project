#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

namespace ui {
	class NowLoading : public ci_ext::Object {
		enum class Flag {
			START,
			END,
		};
		struct Char {
			float posx, posy;
			Flag flag;
			int t; //Time Counter
			float angle; //Moving from 0~180
		};
		struct Pic {
			float posx, posy;
			int srcx;
			float scalex;
			float angle; //Moving from 0~360 (Use for Flipping)
		};

		enum {
			//Setting
			OFFSET_LEFT = 37, //Shift left from center by ... pixels
			SPACE_EACH_CHAR = 0, //Additional space between each character
			MOVE_HEIGHT = 50, //Maximum pixels that character can move (varies by SCALE below)
			T_EACH_CHAR = 10, //Timing between each character
			CHAR_SPD = 6, //Moving speed
			PIC_SPD = 3, //Flipping speed
			PIC_BORDER_SPD_TIMES = 2, //PIC_SPD‚Ìxx”{

			//Const
			STR_LEN = 14, //"Now Loading..."‚Ìstrlen
			CHAR_WIDTH = 38,
			CHAR_HEIGHT = 82,
			PIC_WIDTH = 74,
		};

		//All Text & Pic Scale Setting
		const float SCALE = 1.0f;
		//Position Setting
		const int CENTER_X = static_cast<int>(gplib::system::WINW - (((STR_LEN / 2 * CHAR_WIDTH) + (PIC_WIDTH / 2) + 15) * SCALE));
		const int POS_Y = static_cast<int>(gplib::system::WINH - ((CHAR_HEIGHT / 2) + 15) * SCALE);
		//const int CENTER_X = gplib::system::WINW / 2;
		//const int POS_Y = gplib::system::WINH / 2;

		LPDIRECT3DTEXTURE9 tex_; //Graphic Texture
		
		Char ch_[STR_LEN];
		Pic pic_[2];

	public:
		NowLoading();
		~NowLoading();

		void init() override;
		void initText();
		void update() override;
		void render() override;
		void createTexture();
		void drawPic(float x, float y, float z,
			int srcX, int srcY, int sizeX, int sizeY,
			float degree = 0.f, float scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255, u_char r = 255, u_char g = 255, u_char b = 255
			);
	};
}