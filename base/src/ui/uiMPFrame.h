#pragma once


#include "uiBase.h"

namespace BOMB
{
	class uiMPFrame : public uiBase
	{
		float FRAME_DISTANT = 120.f;									 //HPÇ∆MPÉQÅ[ÉWÅ@ÇÃãóó£
		float MP_FRAME_POS_X = HP_FRAME_POS_X;
		float MP_FRAME_POS_Y = HP_FRAME_POS_Y + FRAME_DISTANT;

		ci_ext::DrawObjf frame_;

	public:
		uiMPFrame(std::string resname);
		void init() override;
		void update() override;
		void render() override;

	};

}