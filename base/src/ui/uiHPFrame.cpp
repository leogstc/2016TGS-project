#include "uiHPFrame.h"


namespace BOMB
{
	uiHPFrame::uiHPFrame(std::string resname)
		:
		uiBase("hp_frame"),
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
		mask_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			"frame_mask",									//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(FRAME_ALPHA, 255, 255, 255),		//color
			0.0f											//degree
			))
		,
		effect_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			"effectStar",									//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),		//color
			0.0f											//degree
			))
		
	{}

	//SCALEÇÃèâä˙âª
	float uiBase::GAUGE_SCALE_ = 0.6f;


	void uiHPFrame::init()
	{
	  frame_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, FRAME_POSZ));
		frame_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		frame_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		mask_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, FRAME_POSZ));
		mask_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH, FRAME_HIGHT));
		mask_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		effect_.setPos(ci_ext::Vec3f(nowWidth * GAUGE_SCALE(), ((HP_FRAME_POS_Y + (FRAME_HIGHT * GAUGE_SCALE()) * GAUGE_SCALE()) / 2), FRAME_POSZ ));
		effect_.setSrcSize(ci_ext::Vec2f(EFFECT_SIZE, EFFECT_SIZE));
		effect_.setScale(ci_ext::Vec2f(EFFECT_SCALE, EFFECT_SCALE));

		isDamage = false;
		mask_flag = false;
		FRAME_ALPHA = 0;
		nowWidth = 0;
		anim_.setAnim(20, 2);
		

	}

	void uiHPFrame::update()
	{

#ifdef _DEBUG


		//gplib::font::TextNC(10, 500, 0.0f, "Frame alpha : " + std::to_string(FRAME_ALPHA), ARGB(255, 255, 255, 255), 0);
		//gplib::font::TextNC(250, 500, 0.0f, "Effect Scake  : " + std::to_string(EFFECT_SCALE), ARGB(255, 255, 0, 0), 0);

#endif

		//if (gplib::input::CheckPress(gplib::input::KEY_F3))
		//{
		//	offSetScale(-0.01f);
		//}

		//if (gplib::input::CheckPress(gplib::input::KEY_F4))
		//{
		//	offSetScale(+0.01f);
		//}

		anim_.step();
		if (player.expired()) getPlayerPtr();

		mask_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, FRAME_POSZ));
		mask_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH, FRAME_HIGHT));
		mask_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));
		
		//Debug (for now) default for gauge is 0.6f effect = 1.0f  
		EFFECT_SCALE = GAUGE_SCALE() + 0.4f;

		if (FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			nowWidth = (FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;
		    frame_.setSrcSize(ci_ext::Vec2f(nowWidth, FRAME_HIGHT));

			effect_.setPos(ci_ext::Vec3f((nowWidth * GAUGE_SCALE()) -  (EFFECT_SIZE * GAUGE_SCALE())/2, ( (FRAME_HIGHT * GAUGE_SCALE()) /2)  - (EFFECT_SIZE * GAUGE_SCALE())/2, FRAME_POSZ - 0.01f));
			effect_.setScale(ci_ext::Vec2f(EFFECT_SCALE, EFFECT_SCALE));
			effect_.setSrc(anim_.getAnimNo() * EFFECT_SIZE);

		}
		frame_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X, HP_FRAME_POS_Y, FRAME_POSZ));
		frame_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));
		mask_.setColor(FRAME_ALPHA, 247, 177, 0);

		if (!player.expired() && player.lock()->getPlayerState() == player::State::HIT)
		{
			isDamage = true;
			if (!mask_flag)
			{
				rannum_ = 32;
				FRAME_ALPHA = 100;
				mask_flag = true;
			}
			doDamageMask();
		}
		else
		{
			mask_flag = false;
		}

	}

	void uiHPFrame::render()
	{
		frame_.renderLeftTopNC();
		mask_.renderLeftTopNC();
	}

	void uiHPFrame::renderLater()
	{
		if (FRAME_WIDTH_PERCENT < 100)effect_.renderLeftTopNC();
	}

	void uiHPFrame::doDamageMask()
	{
		if (isDamage)
		{
			int x = gplib::math::GetRandom(-rannum_, rannum_);
			int y = gplib::math::GetRandom(-rannum_, rannum_);

			mask_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X + x, HP_FRAME_POS_Y + y, FRAME_POSZ));		
			--rannum_;
			--FRAME_ALPHA;

			if (rannum_ <= 0)
			{
				isDamage = false;
				rannum_ = 0;
				FRAME_ALPHA = 0;
			}
		}
	}

	inline void uiHPFrame::getPlayerPtr()
	{
		player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
	}

}