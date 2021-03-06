#pragma once
#include "effectBase.h"

namespace effect
{
	class Effect_FadeInBlack:public EffectBase
	{
		int alpha_;
		POINT camera_; //カメラ座標
	public:
		Effect_FadeInBlack(int x,int y);
		void init() override;
		void update() override;
		void renderLater() override;
	};
}
