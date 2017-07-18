#include "uiAvatar.h"
#include "uiLife.h"

namespace BOMB
{
	
	uiAvatar::uiAvatar(std::string resname)
		:
		uiBase("avatar"),
		avatar_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			resname,										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))

	{}

	void uiAvatar::init()
	{
		ALPHA_PERCENT = 0;
		avatar_.setSrcSize(ci_ext::Vec2f(AVATAR_WIDTH, AVATAR_HEIGHT));
		avatar_.setScale(ci_ext::Vec2f(AVATAR_SCALE, AVATAR_SCALE));
		avatar_.setPos(ci_ext::Vec3f(60, 50, AVATAR_POSZ));
		avatar_.setColor(AVATAR_ALPHA, 255, 255, 255);
		isInitAvatar = false;
		isInitUiLife = false;
		isDecreasTotalLife = false;
	}

	void uiAvatar::update()
	{
		if (ALPHA_PERCENT < 100)
		{
			ALPHA_PERCENT++;
			int ALPHA = (AVATAR_ALPHA * ALPHA_PERCENT) / 100;
			avatar_.setColor(ALPHA, 255, 255, 255);
			
		}
		else
		{
			isInitAvatar = true;
		}

		avatar_.setScale(ci_ext::Vec2f(GAUGE_SCALE() * AVATAR_SCALE, GAUGE_SCALE() * AVATAR_SCALE));
		avatar_.setPos(ci_ext::Vec3f(HP_FRAME_POS_X + ((25 * GAUGE_SCALE()) *AVATAR_SCALE),
				HP_FRAME_POS_Y + ((25 * GAUGE_SCALE()) * AVATAR_SCALE), AVATAR_POSZ));


		if (isInitAvatar)
		{

			if (playerInfo.expired())
			{
				getPlayerInfoPtr();
			}

			else
			{
				lifeManager();

				// AVATAR will change based on HP 
				if (playerInfo.lock()->getHp() > 50 && playerInfo.lock()->getHp() <= 100)
				{
					avatar_.setSrc(ci_ext::Vec2f(AVATAR_WIDTH * FACE::FULL));
				}
				/*		if (playerInfo.lock()->getHp() <= 50 && playerInfo.lock()->getHp() >= 20)
				{
				avatar_.setSrc(ci_ext::Vec2f(AVATAR_WIDTH * FACE::HALF));
				}*/
				if (playerInfo.lock()->getHp() <= 30 && playerInfo.lock()->getHp() > 0)
				{
					avatar_.setSrc(ci_ext::Vec2f(AVATAR_WIDTH * FACE::LOW));
				}
				if (playerInfo.lock()->getHp() <= 0)
				{
					avatar_.setSrc(ci_ext::Vec2f(AVATAR_WIDTH * FACE::DEAD));
				}
			}
		}

		if (player.expired())
		{
			getPlayerPtr();
		}
		else 
		{
			if ((player.lock()->getPlayerState() == player::State::DEAD  && !isDecreasTotalLife && life_.size() >= 1))
			{
				life_.pop_back();
				isDecreasTotalLife = true;
			}
		}

		if (isInitUiLife)
		{
			for (size_t i = 0; i < life_.size(); ++i)
			{
				life_[i].update();
			}
		}

	}
	void uiAvatar::render()
	{
		avatar_.renderLeftTopNC();
	}
	void uiAvatar::renderLater()
	{
		for (size_t i = 0; i < life_.size(); ++i)
		{
			life_[i].render();	
		}
	}

	inline void uiAvatar::getPlayerInfoPtr()
	{
		playerInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
	}

	inline void uiAvatar::getPlayerPtr()
	{
		player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
	}

	void uiAvatar::lifeManager()
	{
		if (!isInitUiLife)
		{
			int totalLife = playerInfo.lock()->getLife();
			float posX = avatar_.pos().x() + 90.0f;
			float posY = avatar_.pos().y() + FRAME_HIGHT + 20.0f ;


			if (totalLife > 0)
			{
				for (int i = 0; i < totalLife - 1; ++i)
				{
				auto life = new uiLife("avatar", posX + (i * 55.0f), posY);
				life_.push_back(*life);
				life_[i].init();
				}
			}
			isInitUiLife = true;
		}

	}
}