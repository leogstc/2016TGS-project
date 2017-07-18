#pragma once
#include "effectManager.h"

namespace effect
{
	class Effect_FadeOutBlack :public EffectBase
	{
		POINT camera_;
		int alpha_;
	public:
		Effect_FadeOutBlack(int x, int y);
		void update() override;
		void render() override;
	};
}
