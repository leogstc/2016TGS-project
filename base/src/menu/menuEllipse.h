#pragma once

#include "menuLeftRightBase.h"
#include "../object/animation.hpp"
#include <Windows.h>

namespace menu {
	class MenuEllipse : public MenuLeftRightBase {
		//Selecting
		int skipItem_;
		std::vector<bool> available_;

		//Circle
		POINT midPos_;
		float radiusX_;
		float radiusY_;
		float nowAngle_; //åªç›ÇÃäpìx
		float firstItemPosAngle_; //Angle of First Item

		//Intro & Outro & Item Selectable Flag
		bool intro_;
		bool outro_;
		bool selectable_;
		int t_; //Frame time counter
		int lastFrameNow_; //last frame selected item
		
		//Animation
		float incAngle_; //increment of angle | when choosing item
		float incRadiusX_; //intro / outro
		float incRadiusY_; //intro / outro
		float alpha_; //Alpha of Graphic's Color | intro / outro
		float incAlpha_; //intro / outro
		anim::Anim anim_;

		//Debug string
		std::string debugStr_;

	public:
		MenuEllipse();
		~MenuEllipse();

		void init() override;
		void update() override;
		void render() override;

		void selectItem();
		void CursorSpeedControl(int interval) override;
	};
}