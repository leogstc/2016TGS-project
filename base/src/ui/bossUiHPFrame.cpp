#include "bossUiHPFrame.h"


namespace BOMB
{
	bossUiHPFrame::bossUiHPFrame(std::string resname)
		:
		uiBase("boss_hp_frame"),
		frame_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			resname,										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))
		,
		avatar_
		(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			"boss_avatar",									//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))

	{}

	//SCALEÇÃèâä˙âª
	float uiBase::BOSS_GAUGE_SCALE_ = 1.0f;


	void bossUiHPFrame::init()
	{
		frame_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X, BOSS_HP_FRAME_POS_Y, FRAME_POSZ));
		frame_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, BOSS_FRAME_HIGHT));
		frame_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE(), BOSS_GAUGE_SCALE()));

		avatar_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X - BOSS_AVATAR_WIDTH , BOSS_HP_FRAME_POS_Y, AVATAR_POSZ));
		avatar_.setSrcSize(ci_ext::Vec2f(BOSS_AVATAR_WIDTH, BOSS_AVATAR_HEIGHT));

		FRAME_ALPHA = 0;
		nowWidth = 0;
		anim_.setAnim(20, 2);
	}

	void bossUiHPFrame::update()
	{

#ifdef _DEBUG

#endif


		anim_.step();
		if (player.expired()) getPlayerPtr();



		if (FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			nowWidth = (BOSS_FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;
			frame_.setSrcSize(ci_ext::Vec2f(nowWidth , BOSS_FRAME_HIGHT));
		}
		frame_.setPos(ci_ext::Vec3f(BOSS_HP_FRAME_POS_X, BOSS_HP_FRAME_POS_Y, FRAME_POSZ));
		frame_.setScale(ci_ext::Vec2f(BOSS_GAUGE_SCALE() * -1.0f, BOSS_GAUGE_SCALE()));

	}

	void bossUiHPFrame::render()
	{
		frame_.renderLeftTopNC();
		avatar_.renderLeftTopNC();
	}

	void bossUiHPFrame::renderLater()
	{
	
	}

	inline void bossUiHPFrame::getPlayerPtr()
	{
		player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
	}

}