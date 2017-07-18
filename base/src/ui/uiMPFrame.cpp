#include  "uiMPFrame.h"


namespace BOMB
{
	uiMPFrame::uiMPFrame(std::string resname)
		:
		uiBase("mp_frame"),
		frame_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),								//scale
			resname,														//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),	//color
			0.0f																//degree
			))

	{}


	void uiMPFrame::init()
	{
		frame_.setPos(ci_ext::Vec3f(MP_FRAME_POS_X, HP_FRAME_POS_Y + (FRAME_DISTANT * GAUGE_SCALE()), FRAME_POSZ));
		frame_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		frame_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

	}

	void uiMPFrame::update()
	{
		if (FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			float WIDTH = (FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;
			frame_.setSrcSize(ci_ext::Vec2f(WIDTH, FRAME_HIGHT));
		}

		frame_.setPos(ci_ext::Vec3f(MP_FRAME_POS_X, HP_FRAME_POS_Y + (FRAME_DISTANT * GAUGE_SCALE()), FRAME_POSZ));
		frame_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));
	}

	void uiMPFrame::render()
	{
		frame_.renderLeftTopNC();
	}
}
