#pragma once
#include "uiBase.h"

namespace BOMB
{
	class uiLife : public uiBase
	{
		ci_ext::DrawObjf avatar_;

		float AVATAR_WIDTH = 150.0f;
		float AVATAR_HEIGHT = 150.0f;
		float nowScale;
		float AVATAR_MAX_SCALE = 0.3f;
		int AVATAR_ALPHA = 255;
		int ALPHA_PERCENT = 0;
		const float AVATAR_POSZ = 0.5f;
		bool initFlag;

	public:
		uiLife(std::string resname, float posX, float posY);

		void init() override;
		void render() override;
		void update() override;

	};

}