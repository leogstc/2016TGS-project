#pragma once

#include "effectBase.h"

namespace effect {
	class EffectReady : public EffectBase {
		struct Star {
			float posx, posy;
			float offsetx, offsety;
			int pic;
			float deg;
			float degStep;
			float scale;
		};

		const std::string resname_;
		const std::string starRes_;
		const int sizex_, sizey_;
		float scale_;
		float alpha_;
		bool flipScale_;
		bool out_;
		Star stars_[6];
		const int starSize_;

	public:
		EffectReady(int x, int y);
		void init() override;
		void update() override;
		//void render() override;
		void renderLater() override;
	};
}