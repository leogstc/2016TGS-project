#include "bossUiHPBar.h"
#include "../stage/map.h"


/////////////////////////////////////////////////////////////////////////
//
// For now, took 20 damages for being hit by the enemy.
//
/////////////////////////////////////////////////////////////////////////
namespace BOMB
{
	bossUiHPBar::bossUiHPBar(std::string resname)
		:
		uiBase("boss_hp_bar"),
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
			"boss_bar_mask",								//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 0, 0),					//color
			0.0f											//degree
			))
	{}
	void bossUiHPBar::init()
	{
		isInitBAR = false;
		FRAME_WIDTH_PERCENT = 0.0f;

		bar_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X, BOSS_HP_FRAME_POS_Y, BAR_POSZ));
		bar_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		bar_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE() * -1.0f, BOSS_GAUGE_SCALE()));

		mask_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X, BOSS_HP_FRAME_POS_Y, MASK_POSZ));
		mask_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		mask_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE() - 1.0f, BOSS_GAUGE_SCALE()));

		currentMask = BOSS_BAR_WIDTH;
		mask_flag = false;

	}

	void bossUiHPBar::update()
	{
#ifdef _DEBUG



#endif


		if (boss.expired()) getBossPtr();
		if (map_.expired()) getMapPtr();
		auto map = ci_ext::weak_cast<map::Map>(map_);

		if (!boss.expired())
		{
			currentLife = (boss.lock()->getHp() * BOSS_BAR_WIDTH) / boss.lock()->getMaxHp();
		}
		bar_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE() * -1.0f, BOSS_GAUGE_SCALE()));
		mask_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE() * -1.0f, BOSS_GAUGE_SCALE()));

		if (!isInitBAR && FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			float WIDTH = (BOSS_FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;

			bar_.setSrcSize(ci_ext::Vec2f(WIDTH, BOSS_FRAME_HIGHT));
			mask_.setSrcSize(ci_ext::Vec2f(WIDTH, BOSS_FRAME_HIGHT));
		}
		else
		{
			isInitBAR = true;
			bar_.setSrc(ci_ext::Vec2f(BOSS_BAR_OFFSET_X));
			mask_.setSrc(ci_ext::Vec2f(BOSS_BAR_OFFSET_X));
		}

		if (isInitBAR)
		{
			bar_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X - (BOSS_BAR_OFFSET_X * BOSS_GAUGE_SCALE()), BOSS_HP_FRAME_POS_Y, BAR_POSZ));
			bar_.setSrcSize(ci_ext::Vec2f(currentLife, BOSS_FRAME_HIGHT));

			mask_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X - (BOSS_BAR_OFFSET_X * BOSS_GAUGE_SCALE()), BOSS_HP_FRAME_POS_Y, MASK_POSZ));
			mask_.setSrcSize(ci_ext::Vec2f(currentMask, BOSS_FRAME_HIGHT));

			mask_.setColor(mask_alpha, 255, 0, 0);

			if (boss.expired())
			{
				currentLife = 0.0f;
			}
			checkHPMask();

		}

	}

	void bossUiHPBar::render()
	{
		bar_.renderLeftTopNC();
		mask_.renderLeftTopNC();
	}

	inline void bossUiHPBar::getBossPtr()
	{
		boss = ci_ext::weak_cast<enemy::Enemy_Boss>(getObjectFromRoot("enemy_boss"));
	}

	inline void bossUiHPBar::getMapPtr()
	{
		map_ = getObjectFromRoot("map");
	}


	void bossUiHPBar::checkHPMask()
	{

		if(currentMask > currentLife) 
		{
			mask_flag = true;
		}

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
					currentMask -= 5.0f;		
				}
				else
				{
					currentMask = currentLife;
					mask_count = 0.0f;
					mask_flag = false;
				}
			}
		}

	}


}