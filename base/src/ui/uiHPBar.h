#pragma once

#include "uiBase.h"

namespace BOMB
{
	class uiHPBar : public uiBase
	{


		float currentLife;
		float currentMask;

		ci_ext::DrawObjf bar_;
		ci_ext::DrawObjf mask_;

		//MASK
		bool mask_flag;
		const float mask_delay = 50.0f;
		float mask_count = 0;
		int mask_alpha = 255;

		bool isdead_;

	private:
		
		inline void getPlayerPtr();
		inline void getPlayerInfoPtr();
		inline void getMapPtr();
		void checkHPBar();
		void checkHPMask();


	public:
		uiHPBar(std::string resname);
		void init() override;
		void update() override;
		void render() override;
		void doHpRecovery();

	
	};

}