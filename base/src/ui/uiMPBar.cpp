#include "uiMPBar.h"
#include "../stage/map.h"



/////////////////////////////////////////////////////////////////////////////////////
//
// Can fly for 3 seconds  
// MP cooldown in 1 second
// MP full recovery in 3 seconds
// ATK_M use 25 MP 
//
////////////////////////////////////////////////////////////////////////////////////




namespace BOMB
{
	uiMPBar::uiMPBar(std::string resname)
		:
		uiBase("mp_bar"),
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
		barSmall_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			resname,										//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))
	{}

	void uiMPBar::init()
	{
		isInitBAR = false;
		isnowFlying = false;
		isMPRecovery = false;
		isnowAttack = false;

		FRAME_WIDTH_PERCENT = 0.0f;
		bar_.setPos(ci_ext::Vec3f(MP_FRAME_POS_X, HP_FRAME_POS_Y + (FRAME_DISTANT * GAUGE_SCALE()), BAR_POSZ));
		bar_.setSrcSize(ci_ext::Vec2f(FRAME_WIDTH_PERCENT, FRAME_HIGHT));
		bar_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

	}

	void uiMPBar::update()
	{
		if (player.expired()) getPlayerPtr();
		if (playerInfo.expired()) getPlayerInfoPtr();
		if (map_.expired()) getMapPtr();
		auto map = ci_ext::weak_cast<map::Map>(map_);

		currentMp = (playerInfo.lock()->getMp() * BAR_WIDTH) / playerInfo.lock()->getMaxMp();

		bar_.setScale(ci_ext::Vec2f(GAUGE_SCALE(), GAUGE_SCALE()));

		if (!isInitBAR && FRAME_WIDTH_PERCENT < 100.f)
		{
			FRAME_WIDTH_PERCENT++;
			float WIDTH = (FRAME_WIDTH * FRAME_WIDTH_PERCENT) / 100;
			bar_.setPos(ci_ext::Vec3f((MP_FRAME_POS_X + (BAR_OFFSET_X * GAUGE_SCALE())) - BAR_OFFSET_X * GAUGE_SCALE(), HP_FRAME_POS_Y + (FRAME_DISTANT * GAUGE_SCALE()), BAR_POSZ));
			bar_.setSrcSize(ci_ext::Vec2f(WIDTH, FRAME_HIGHT));
		}
		else
		{
			isInitBAR = true;
			bar_.setSrc(ci_ext::Vec2f(BAR_OFFSET_X));
		}
		if (isInitBAR )
		{	
			bar_.setColor(ci_ext::Color(BAR_ALPHA, 255, 255, 255));
			bar_.setPos(ci_ext::Vec3f(MP_FRAME_POS_X + (BAR_OFFSET_X * GAUGE_SCALE()), HP_FRAME_POS_Y + (FRAME_DISTANT * GAUGE_SCALE()), BAR_POSZ));
			bar_.setSrcSize(ci_ext::Vec2f(currentMp, FRAME_HIGHT));

			if (gplib::input::CheckPush(gplib::input::KEY_BTN2) && !isnowAttack)
			{
				offsetMP = playerInfo.lock()->getMp() - config.ATK_M_MP_USE;
			}
			if(!map.lock()->isChangingMap() && !player.expired()) checkMPBar();

			if (!player.expired() )
			{
				ci_ext::Vec2f  pos = player.lock()->getPos();
				barSmall_.setSrc(ci_ext::Vec2f(BAR_OFFSET_X));
				barSmall_.setPos(ci_ext::Vec3f(pos.x()  - 32.0f, pos.y() + 64.0f, 0.0f));
				barSmall_.setSrcSize(ci_ext::Vec2f(currentMp, FRAME_HIGHT));
				barSmall_.setScale(ci_ext::Vec2f(0.2f, 0.2f));

				if (player.lock()->getPlayerState() != player::State::FLY)
				{	
					barSmall_.setPos(ci_ext::Vec3f(pos.x() - 32.0f, pos.y() + 45.0f, 0.0f));
				}
				else
				{
					barSmall_.setPos(ci_ext::Vec3f(pos.x() - 32.0f, pos.y() + 32.0f, 0.0f));
				}
			}

		}
	}

	void uiMPBar::render()
	{
		bar_.renderLeftTopNC();
		if (!player.expired() && playerInfo.lock()->getMp() < 100)
		{
			barSmall_.renderLeftTop();
		}
#ifdef _DEBUG
		int x = 10;
		int y = 200;

		if (!player.expired())
		{
			gplib::font::TextNC(x, y + 60, 0.0f, "PlayerInfo MP : " + std::to_string(playerInfo.lock()->getMp()), ARGB(255, 255, 255, 255), 0);
			gplib::font::TextNC(x, y + 80, 0.0f, "current MP bar width : " + std::to_string(currentMp), ARGB(255, 255, 255, 255), 0);
			gplib::font::TextNC(x, y + 100, 0.0f, "isnowFlying  : " + std::to_string(isnowFlying), ARGB(255, 255, 255, 255), 0);
			gplib::font::TextNC(x, y + 120, 0.0f, "isnowAttack  : " + std::to_string(isnowAttack), ARGB(255, 255, 255, 255), 0);
			gplib::font::TextNC(x, y + 140, 0.0f, "isRecovery  : " + std::to_string(isMPRecovery), ARGB(255, 255, 255, 255), 0);
		}

#endif

	}

	void uiMPBar::checkMPBar()
	{
		FLY_MP_UP_DOWN_SPEED = ((playerInfo.lock()->getMaxMp() * (gplib::system::ONEFRAME_TIME) / ((gplib::system::REFRESHRATE / FLY_DURATION_SEC) / 8)));
		ATK_M_DOWN_SPEED = config.ATK_M_MP_USE * (gplib::system::ONEFRAME_TIME * 2.0f);

		if (!isnowAttack && !isnowFlying)
		{
			isMPRecovery = true;
		}
		else
		{
			isMPRecovery = false;
			counter = 0.0f;
		}

		if (playerInfo.lock()->getMp() > 0.0f)
		{
			//setGaugeColor();
			if (player.lock()->getPlayerState() == player::State::START_FLY || player.lock()->getPlayerState() == player::State::FLY || player.lock()->getPlayerState() == player::State::FLY_ATK)
			{
				playerInfo.lock()->offsetMp(-FLY_MP_UP_DOWN_SPEED);
				isnowFlying = true;
				(BAR_ALPHA >= 150 ? BAR_ALPHA -= 4 : BAR_ALPHA = 255);
			}
			else
			{
				BAR_ALPHA = 255;
				isnowFlying = false;
			}
		}
		if (playerInfo.lock()->getMp() <= 0.0f)
		{
			if (player.lock()->getPlayerState() == player::State::START_FLY || player.lock()->getPlayerState() == player::State::FLY)
			{
				player.lock()->setVelocity(0, 0);
				player.lock()->setPlayerState(player::State::FALL);
			}
			playerInfo.lock()->setMP(0.0f);
			isnowFlying = false;

		}

		//–‚–@UŒ‚‚Í‚à‚¤MP‚ªŽg‚¦‚È‚¢B
		//if (player.lock()->getPlayerState() == player::State::ATK_M)
		//{
		//	isnowAttack = true;
		//	if (playerInfo.lock()->getMp() > offsetMP)
		//	{
		//		playerInfo.lock()->offsetMp(-ATK_M_DOWN_SPEED);
		//		if (playerInfo.lock()->getMp() <= offsetMP)
		//			playerInfo.lock()->setMP(offsetMP);
		//	}
		//}
		//else
		//{
		//	isnowAttack = false;
		//	//offsetMP = 0.0f;
		//}

		//MP‚Ì‰ñ•œ
		if (isMPRecovery)
		{
			counter++;
			if (counter >= gplib::system::REFRESHRATE * MP_RECOVERY_COOLDOWN_SEC)		// 1•b‚Å‰ñ•œ‚Ì‘O‚É‘Ò‚Â
			{
				if (playerInfo.lock()->getMp() <= playerInfo.lock()->getMaxMp())
				{
					playerInfo.lock()->offsetMp(FLY_MP_UP_DOWN_SPEED);					// MP‚ª3•b‚ÅFULL‚É‚È‚é
				}
				else
				{
					if (playerInfo.lock()->getMp() > playerInfo.lock()->getMaxMp()) playerInfo.lock()->setMP(100.0f);
					counter = 0.0f;
					isMPRecovery = false;
				}
			}
		}

	}



	inline void uiMPBar::getPlayerPtr()
	{
		player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
	}
	inline void uiMPBar::getPlayerInfoPtr()
	{
		playerInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
	}
	inline void uiMPBar::getMapPtr()
	{
		map_ = getObjectFromRoot("map");
	}


}