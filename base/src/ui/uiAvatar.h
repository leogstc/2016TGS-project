#pragma once
#include "uiBase.h"
#include "uiLife.h"

namespace BOMB
{
	class uiAvatar : public uiBase
	{
		ci_ext::DrawObjf avatar_;
		//enum  FACE { FULL, HALF, LOW, DEAD, MAX_FACE };
		enum  FACE { FULL,LOW, DEAD, MAX_FACE };
		float AVATAR_WIDTH = 150.0f;
		float AVATAR_HEIGHT = 150.0f;
		float AVATAR_SCALE = 0.6f;
		int AVATAR_ALPHA = 255;
		int ALPHA_PERCENT = 0;
		int nowFace = FACE::FULL;
		const float AVATAR_POSZ = 0.5f;
		bool isInitAvatar;
		bool isInitUiLife;
		std::vector<uiLife> life_;
		bool isDecreasTotalLife;

	private:
		inline void getPlayerInfoPtr();
		inline void getPlayerPtr();
	public:
		uiAvatar(std::string resname);
	
		void init() override;
		void render() override;
		void renderLater() override;
		void update() override;
		void lifeManager();

	};

}