#pragma once

#include "uiBase.h"

namespace BOMB 
{
	class uiHPFrame : public uiBase
	{

		float HP_FRAME_POS_X = 10.0f;
		float HP_FRAME_POS_Y = 10.0f;
		bool isDamage;
		bool mask_flag;
		int FRAME_ALPHA;
		float EFFECT_SCALE = 1.0f;
		float nowWidth;

		ci_ext::DrawObjf frame_;
		ci_ext::DrawObjf mask_;
		ci_ext::DrawObjf effect_;


	private:
		void doDamageMask();
		inline void getPlayerPtr();
	
	public: 
		uiHPFrame( std::string resname);
		void init() override;
		void update() override;
		void render() override;
		void renderLater() override;


	};

}