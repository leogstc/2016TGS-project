#pragma once

#include "uiBase.h"

namespace BOMB
{
	class uiMPBar : public uiBase
	{
		float FRAME_DISTANT = 120.f;									 //HPÇ∆MPÉQÅ[ÉWÅ@ÇÃãóó£
		float MP_FRAME_POS_X = HP_FRAME_POS_X;
		float MP_FRAME_POS_Y = HP_FRAME_POS_Y + FRAME_DISTANT;
		int BAR_ALPHA = 255;

		float currentMp;
		ci_ext::DrawObjf bar_,barSmall_;

		float FLY_MP_UP_DOWN_SPEED;										//= (maxMp * 0.33f / 60.f);	
		float ATK_M_DOWN_SPEED;
		bool isnowFlying;
		bool isMPRecovery;
		bool isnowAttack;
		float counter = 0.0f;
		float offsetMP = 0.0f;

		float MP_RECOVERY_COOLDOWN_SEC = 1.0f;
		float FLY_DURATION_SEC = 3.0f;

	private:

		inline void getPlayerPtr();
		inline void getPlayerInfoPtr();
		inline void getMapPtr();
		void checkMPBar();


	public:
		uiMPBar(std::string resname);
		void init() override;
		void update() override;
		void render() override;




	};

}