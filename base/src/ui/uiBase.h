#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"
#include "../object/animation.hpp"
#include "../lib/ci_ext//DrawObj.hpp"
#include "../player/player.h"
#include "../player/playerInfo.hpp"
#include "../config.h"
#include "../enemy/enemy_boss.h"

#include <ctime>

namespace BOMB
{
	class uiBase : public ci_ext::Object
	{

	private:
		static float GAUGE_SCALE_;
		static float BOSS_GAUGE_SCALE_;

	protected :
		int posx_ ,posy_,rannum_;
		anim::Anim anim_;

		const float AVATAR_POSZ = 0.5f;

		//プレイヤー
		const float FRAME_WIDTH = 425.0f;		//絵の横	のサイズ		
		const float FRAME_HIGHT = 122.0f;		//絵の経てのサイズ
		const float EFFECT_SIZE = 60.0f;
		const float BAR_WIDTH = 298.0f;
	
		const float FRAME_POSZ = 0.51f;
		const float BAR_POSZ = 0.52f;
		const float MASK_POSZ = 0.53f;

		float FRAME_WIDTH_PERCENT = 0.0f;
		float BAR_OFFSET_X = 113.0f;
		
		float HP_FRAME_POS_X = 10.0f;
		float HP_FRAME_POS_Y = 10.0f;

		//ボース
		const float BOSS_FRAME_WIDTH = 1246.0f;		//絵の横	のサイズ		
		const float BOSS_FRAME_HIGHT = 90.0f;		//絵の経てのサイズ
		const float BOSS_BAR_WIDTH = 1134.0f;
		float BOSS_BAR_OFFSET_X = 97.0f;

		float BOSS_HP_FRAME_POS_X = gplib::system::WINW - 10.0f;
		float BOSS_HP_FRAME_POS_Y = gplib::system::WINH - 100.0f;


		bool isInitBAR;

		//PlayerとPlayerInfo
		std::weak_ptr<player::Player> player;
		std::weak_ptr<player::PlayerInfo> playerInfo;
		std::weak_ptr<ci_ext::Object> map_;
		std::weak_ptr<enemy::Enemy_Boss> boss;
	
	public:
		
		uiBase(const std::string& objectName)
			:
			Object(objectName)
		{}
		virtual ~uiBase() {}	
		void init() override {}
		void update() override {}
		void render() override {}
	  static float GAUGE_SCALE() { return GAUGE_SCALE_; }
		static float BOSS_GAUGE_SCALE() { return BOSS_GAUGE_SCALE_; }
		void offSetScale(float input) { GAUGE_SCALE_ += input; }	
		void offSetBossScale(float input) { BOSS_GAUGE_SCALE_ += input; }
	};

}