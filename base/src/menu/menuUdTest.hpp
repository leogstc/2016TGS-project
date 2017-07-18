#pragma once

#include "menuUpDownBase.h"
#include "../lib/gplib.h"
#include <unordered_map>

#define BG_MAX_X 300.0f
//Initial value -> Position of first item
#define FIRST_ITEM_X 30
#define FIRST_ITEM_Y 70
//Font padding
#define SPACE_BETWEEN_ITEM_X 5
#define SPACE_BETWEEN_ITEM_Y 20
//Limit
#define ITEM_LIMIT_START (FIRST_ITEM_Y - 10)
#define ITEM_LIMIT_END (gplib::system::WINH - 160)

namespace menu {
	class MenuUpDownTest : public MenuUpDownBase {
		std::vector<std::string> items_;

		//BG
		float bgX_, bgY_;
		float bgScaleX_, bgScaleY_;

		//Intro & Outro + Animation
		bool intro_;
		bool outro_;
		int t_;
		float bgMoveX_, bgMoveY_;
		
		//Position of First Item
		float firstItemPosX_;
		float firstItemPosY_;

		//Cursor
		float cursorPosX_;
		float cursorPosY_;

		//Debug Rect
		float selectingBoxMidY_;
		RECT selectingRt_;
		//Debug String
		std::string debugStr_;

	public:
		MenuUpDownTest()
			:
			MenuUpDownBase("menu_status", 0),
			intro_(true), outro_(false), t_(0),
			bgX_(0), bgY_(0), bgScaleX_(1), bgScaleY_(1), bgMoveX_(0), bgMoveY_(0),
			firstItemPosX_(FIRST_ITEM_X), firstItemPosY_(FIRST_ITEM_Y), cursorPosX_(0), cursorPosY_(0),
			selectingBoxMidY_(FIRST_ITEM_Y + 10)
		{
			//BG
			bgScaleX_ = BG_MAX_X / gplib::draw::GetImageWidth("submenu_left");
			bgScaleY_ = 544.f / gplib::draw::GetImageHeight("submenu_left");
			bgX_ = -BG_MAX_X / 2;
			bgY_ = float(gplib::system::WINH / 2);

			//Limit box
			//When over this limit box, all items move.
			limit_.top = ITEM_LIMIT_START;
			limit_.bottom = ITEM_LIMIT_END;
			
			//Items
			for (int i = 0; i < 30; ++i) {
				items_.push_back("item" + std::to_string(i));
			}

			//Selectable Item
			maxItems_ = items_.size();
		}

		void init() override
		{
			getParentPtr().lock()->pauseAll();
			run();
		}

		void update() override
		{
			//Intro/Outro Animation
			if (intro_) {
				const float FRAME = 15;
				if (t_ == 0) {
					bgMoveX_ = BG_MAX_X / FRAME;
				}
				t_++;
				bgX_ += bgMoveX_;
				if (t_ >= FRAME) {
					t_ = 0;
					intro_ = false;
				}
				return;
			}
			if (outro_) {
				const float FRAME = 15;
				if (t_ == 0) {
					bgMoveX_ = BG_MAX_X / FRAME;
				}
				t_++;
				bgX_ -= bgMoveX_;
				if (t_ >= FRAME) {
					getParentPtr().lock()->runAll();
					kill();
				}
				return;
			}

			input();

			//Calculate cursor position
			calcCursorPos();
			
			//Input
			if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
				selectItem();
			}

			if (gplib::input::CheckPush(gplib::input::KEY_BTN1)) {
				outro_ = true;
			}

			//When cross the limit line -> shift items
			if ((now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_ > limit_.bottom - SPACE_BETWEEN_ITEM_Y) {
				firstItemPosX_ -= SPACE_BETWEEN_ITEM_X;
				firstItemPosY_ -= SPACE_BETWEEN_ITEM_Y;
				//Re-calculate (no frame lacking in render())
				calcCursorPos();
			}
			else if ((now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_ < limit_.top) {
				firstItemPosX_ += SPACE_BETWEEN_ITEM_X;
				firstItemPosY_ += SPACE_BETWEEN_ITEM_Y;
				//Re-calculate (no frame lacking in render())
				calcCursorPos();
			}


			//gplib::draw::CkRectNC(selectingRt_);

			//gplib::debug::TToM("now*space+fP=%d, fP=%d, t=%d, b=%d", now_ * SPACE_BETWEEN_ITEM_Y + (int)firstItemPosY_, (int)firstItemPosY_, limit_.top, limit_.bottom);
		}

		void render() override
		{
			gplib::draw::GraphNC(bgX_, bgY_, 0.04f, "submenu_left",
				0, 0, gplib::draw::GetImageWidth("submenu_left"), gplib::draw::GetImageHeight("submenu_left"),
				0.0f, bgScaleX_, bgScaleY_);

			int i = 0;
			for (auto& item : items_) {
				float posx = bgX_ - (BG_MAX_X / 2) + (i * SPACE_BETWEEN_ITEM_X) + firstItemPosX_;
				float posy = (i * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_;

				if (posx >= FIRST_ITEM_X && posy >= ITEM_LIMIT_START && posy <= ITEM_LIMIT_END - SPACE_BETWEEN_ITEM_Y)
					gplib::font::TextNC((int)posx, (int)posy, 0.03f, item, ARGB(255, 255, 0, 0), 0);
				i++;
			}

			gplib::draw::BoxNC(390, 10, 700, 50, 0.2f, ARGB(255, 0, 0, 0), ARGB(0, 0, 0, 0), 0, 1);
			gplib::font::TextNC(400, 20, 0.0f, debugStr_, ARGB(255, 255, 255, 0), 0);

			if (cursorPosY_ >= ITEM_LIMIT_START && cursorPosY_ <= ITEM_LIMIT_END - SPACE_BETWEEN_ITEM_Y)
				gplib::draw::GraphNC((int)cursorPosX_, (int)cursorPosY_, 0.01f, "cursor", 0, 0, 20, 20);

			//debug line
			//x
			gplib::draw::LineNC(FIRST_ITEM_X, 0, FIRST_ITEM_X, gplib::system::WINH, 0.0f, ARGB(255, 255, 0, 0), 1);
			//y
			gplib::draw::LineNC(0, limit_.top, gplib::system::WINW, limit_.top, 0.0f, ARGB(255, 255, 0, 0), 1);
			gplib::draw::LineNC(0, limit_.bottom, gplib::system::WINW, limit_.bottom, 0.0f, ARGB(255, 255, 0, 0), 1);
		}

		void calcCursorPos()
		{
			//Cursor
			float offsetFront = 20.0f;
			//選択肢は画像、Graphで表示 | 選択肢はフォント、GraphLeftTopで表示
			//cursorPosX_ = (now_ * SPACE_BETWEEN_ITEM_X) + firstItemPosX_ - offsetFront;
			//cursorPosY_ = (now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_;
			//選択肢はフォント、Graphで表示
			cursorPosX_ = (now_ * SPACE_BETWEEN_ITEM_X) + firstItemPosX_ + (SPACE_BETWEEN_ITEM_X / 2) - offsetFront;
			cursorPosY_ = (now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_ + (SPACE_BETWEEN_ITEM_Y / 2);

			//Debug Rect
			int offsetRectFront = 10;
			int sizeX = 100;
			sizeX -= offsetRectFront;
			selectingRt_ = {
				LONG((now_ * SPACE_BETWEEN_ITEM_X) + firstItemPosX_ - offsetRectFront),
				LONG((now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_),
				LONG((now_ * SPACE_BETWEEN_ITEM_X) + firstItemPosX_ + sizeX),
				LONG((now_ * SPACE_BETWEEN_ITEM_Y) + firstItemPosY_ + (SPACE_BETWEEN_ITEM_Y / 2) * 2)
			};
		}

		void selectItem()
		{
			if (items_[now_] == "item1") {
				debugStr_ = "item 1 is selected";
			}
			else if (items_[now_] == "item2") {
				debugStr_ = "item 2 is selected";
			}
			else if (items_[now_] == "item3") {
				debugStr_ = "item 3 is selected";
			}
			else {
				debugStr_ = std::to_string(now_);
			}

			std::unordered_map<std::string, std::function<void()>> map = {
				{ "item1", std::bind(&MenuUpDownTest::item1, this)},
				{ "item2", std::bind(&MenuUpDownTest::item2, this)}
			};
			try {
				map[items_[now_]]();
			}
			catch (std::exception e) {
				std::cout << e.what() << std::endl;
			}
		}

		void item1()
		{
			gplib::debug::TToM("item1() called");
		}

		void item2()
		{
			gplib::debug::TToM("item2() called");
		}
	};
}