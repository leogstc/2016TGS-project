#pragma once
#include "effectBase.h"

namespace effect
{
	class EffectGameOver :public EffectBase
	{
	public:
		EffectGameOver(int x,int y);
		void update() override;
		void render() override;
	};
}
