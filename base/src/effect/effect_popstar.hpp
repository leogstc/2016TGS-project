#pragma once

#include "effectBase.h"
#include "../config.h"
namespace effect
{
	class EffectPopStar : public EffectBase 
	{
		const std::string resname_;
		const int sizex_, sizey_;
	public:
		EffectPopStar(int x, int y)
			:
			EffectBase("effect_popstar",x,y),
			resname_("effect_pop_star"),
			sizex_(240),
			sizey_(240)
			{
				anim_.setAnim(10, 3);
			}

		void EffectPopStar::update() override
		{
			anim_.step();
			if (anim_.isLastFrame())
				kill();
		}

		void EffectPopStar::renderLater() override
		{
			gplib::draw::Graph(posx_, posy_, posz_, resname_, sizex_*(anim_.getAnimNo() % 5), sizey_*(anim_.getAnimNo() / 5),sizex_,sizey_ );
		}
	};
}