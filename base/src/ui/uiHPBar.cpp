#include "uiHPBar.h"
#include "../stage/map.h"


/////////////////////////////////////////////////////////////////////////
//
// For now, took 20 damages for being hit by the enemy.
//
/////////////////////////////////////////////////////////////////////////
namespace BOMB
{
	uiHPBar::uiHPBar(std::string resname)
		:
		uiBase("hp_bar"),
		bar_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			resname,										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))
		,
		mask_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			"bar_mask",										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 0, 0),					//color
			0.0f											//degree
			))
	{}
	void uiHPBar::init()
	{
		isInitBAR = false;
		FRAME_WIDTH_PERCENT = 0.0f;

		bar_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, BAR_POSZ));
		bar_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		bar_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		mask_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, MASK_POSZ));
		mask_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		mask_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		currentMask = BAR_WIDTH;
		mask_flag = false;
		isdead_ = false;
	}

	void uiHPBar::update()
	{
#ifdef _DEBUG

		//if (!playerInfo.expired())
		//{
		//	int x = 10;
		//	int y = 200;
		//	gplib::font::TextNC(x, y + 20, 0.0f, "PlayerInfo Life : " + std::to_string(playerInfo.lock()->getHp()), ARGB(255, 255, 255, 255), 0);
		//	gplib::font::TextNC(x, y + 40, 0.0f, "current life : " + std::to_string(currentLife), ARGB(255, 255, 255, 255), 0);
		//}


#endif


		if (player.expired()) getPlayerPtr();
		if (playerInfo.expired()) getPlayerInfoPtr();
		if (map_.expired()) getMapPtr();
		auto map = ci_ext::weak_cast<map::Map>(map_);

		currentLife = (playerInfo.lock()->getHp() * BAR_WIDTH) / playerInfo.lock()->getMaxHp();
		bar_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));
		mask_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		if (!isInitBAR && FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			float WIDTH = (FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;

			bar_.setSrcSize(ci_ext::Vec2f(WIDTH, FRAME_HIGHT));
			mask_.setSrcSize(ci_ext::Vec2f(WIDTH, FRAME_HIGHT));
		}
		else
		{
			isInitBAR = true;
			bar_.setSrc(ci_ext::Vec2f(BAR_OFFSET_X));
			mask_.setSrc(ci_ext::Vec2f(BAR_OFFSET_X));
		}

		if (isInitBAR)
		{
			bar_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X + (BAR_OFFSET_X * GAUGE_SCALE()), HP_FRAME_POS_Y, BAR_POSZ));
			bar_.setSrcSize(ci_ext::Vec2f(currentLife, FRAME_HIGHT));
	
			mask_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X + (BAR_OFFSET_X * GAUGE_SCALE()), HP_FRAME_POS_Y, MASK_POSZ));
			mask_.setSrcSize(ci_ext::Vec2f(currentMask, FRAME_HIGHT));

			mask_.setColor(mask_alpha, 255, 0, 0);

			checkHPBar();
			checkHPMask();

			if (!player.expired() && player.lock()->getPlayerState() == player::State::HIT)
			{
				mask_flag = true;
			}

		}

	}

	void uiHPBar::render()
	{
		bar_.renderLeftTopNC();
		mask_.renderLeftTopNC();
	}

	inline void uiHPBar::getPlayerPtr()
	{
		player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
	}
	inline void uiHPBar::getPlayerInfoPtr()
	{
		playerInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
	}
	inline void uiHPBar::getMapPtr()
	{
		map_ = getObjectFromRoot("map");
	}

	void uiHPBar::checkHPBar()
	{
		if (!player.expired())
		{
			if (playerInfo.lock()->getHp() <= 0.0f)
			{
				if (!isdead_)
				{
					player.lock()->initDead();
					isdead_ = true;
				}	
			}
		}
	}

	void uiHPBar::checkHPMask()
	{
		if (mask_flag)
		{
			if (currentLife <= 0)
			{
				currentMask = currentLife;
			}
			else
			{
				mask_count++;
			}

			if (mask_count >= mask_delay)
			{
				if (currentMask > currentLife)
				{
					mask_.setColor(mask_alpha, 255, 0, 0);
					currentMask -= 3.0f;
					mask_alpha -= 10;
				}
				else
				{
					currentMask = currentLife;
					mask_count = 0.0f;
					mask_flag = false;
					mask_alpha = 255;
				}
			}
		}

	}

	void uiHPBar::doHpRecovery()
	{
		float hpRecover = playerInfo.lock()->getMaxHp() * 0.80f;
		float maxHp = playerInfo.lock()->getMaxHp();
		float nowHp = playerInfo.lock()->getHp();
		if (nowHp + hpRecover > maxHp)
		{
			hpRecover = maxHp - nowHp;
		}
		playerInfo.lock()->offsetHp(hpRecover);
		currentLife = (playerInfo.lock()->getHp() * BAR_WIDTH) / playerInfo.lock()->getMaxHp();
		if(currentMask != currentLife)currentMask = currentLife;

	}


}