#pragma once
#include "effectBase.h"

namespace effect
{
	class EffectGilgamesh :public EffectBase
	{
		const std::string resname_;
		const int sizex_, sizey_;
	public:
		EffectGilgamesh(int x, int y)
			:
			EffectBase("effect_gilgamesh", x, y), resname_("effect_gilgamesh"),
			sizex_(18), sizey_(228)
		{
			anim_.setAnim(4, 4);
		}

		void update() override
		{
			anim_.step();
			if (anim_.isLastFrame())
			{
				kill();
			}
		}

		void render() override
		{
			gplib::draw::Graph(posx_, posy_, posz_, resname_, sizex_*anim_.getAnimNo(), 0, sizex_, sizey_);
		}
	};
}