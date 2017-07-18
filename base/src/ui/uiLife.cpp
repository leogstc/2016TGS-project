#include "uiLife.h"

namespace BOMB
{
	uiLife::uiLife(std::string resname, float posX, float posY)
		:
		uiBase("uiLife"),
		avatar_(ci_ext::DrawObjf(
			ci_ext::Vec3f(posX,posY, AVATAR_POSZ),			//pos
			ci_ext::Vec2f::one(),							//scale
			resname,										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(0, 255, 255, 255),				//color
			0.0f											//degree
			))

	{}

	void uiLife::init()
	{
		ALPHA_PERCENT = 0;
		nowScale = 0.0f;
		avatar_.setSrcSize(ci_ext::Vec2f(AVATAR_WIDTH, AVATAR_HEIGHT));
		avatar_.setScale(ci_ext::Vec2f(nowScale, nowScale));
		avatar_.setColor(0, 255, 255, 255);
		initFlag = false;
	}

	void uiLife::update()
	{
		if (nowScale < 0.6f && !initFlag)
		{
			nowScale += 0.02f;
		}
		else
		{
			initFlag = true;
			if (nowScale > AVATAR_MAX_SCALE)
			{
				nowScale -= 0.02f;
			}
		}
		avatar_.setScale(ci_ext::Vec2f(nowScale, nowScale));

		if (ALPHA_PERCENT < 100)
		{
			ALPHA_PERCENT++;
			int ALPHA = (AVATAR_ALPHA * ALPHA_PERCENT) / 100;
			avatar_.setColor(ALPHA, 255, 255, 255);

		}

		//avatar_.setScale(ci_ext::Vec2f(GAUGE_SCALE() * AVATAR_SCALE, GAUGE_SCALE() * AVATAR_SCALE));
	/*	avatar_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X + ((25 * GAUGE_SCALE()) *AVATAR_SCALE),
			HP_FRAME_POS_Y + ((25 * GAUGE_SCALE()) * AVATAR_SCALE) + 300.0f, AVATAR_POSZ));*/

	}
	void uiLife::render()
	{
		avatar_.renderNC();
	}


}