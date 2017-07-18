#pragma once

//#include "menuUpDownBase.h"
#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

#include "../player/playerInfo.hpp"

namespace menu {
	namespace item {
		class MenuItemPlayerStatus : public ci_ext::Object {
			//BG
			float bgX_, bgY_;
			float bgScaleX_, bgScaleY_;

			//Intro & Outro + Animation
			bool intro_;
			bool outro_;
			int t_;
			float bgMoveX_, bgMoveY_;

			//CONST
			const float BG_MAX_X = 300.0f;

		public:
			MenuItemPlayerStatus();

			void init() override;
			void update() override;
			void render() override;
		};
	}
}