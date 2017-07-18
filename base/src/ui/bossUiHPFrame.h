#pragma once

#include "uiBase.h"

namespace BOMB
{
	class bossUiHPFrame : public uiBase
	{
		int FRAME_ALPHA;
		float EFFECT_SCALE = 1.0f;
		float nowWidth;
		ci_ext::DrawObjf frame_;
		ci_ext::DrawObjf avatar_;
		const float BOSS_AVATAR_WIDTH = 77.0f;
		const float BOSS_AVATAR_HEIGHT = 90.0f;
	
	private:
		inline void getPlayerPtr();

	public:
		bossUiHPFrame(std::string resname);
		void init() override;
		void update() override;
		void render() override;
		void renderLater() override;


	};

}