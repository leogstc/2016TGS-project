#pragma once

#include "uiBase.h"

namespace BOMB
{
	class bossUiHPBar : public uiBase
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



	private:

		inline void getBossPtr();
		inline void getMapPtr();
		void checkHPBar();
		void checkHPMask();


	public:
		bossUiHPBar(std::string resname);
		void init() override;
		void update() override;
		void render() override;




	};

}