#include "eventTutorial.h"
#include "../stage/map.h"
#include "worldEventManager.h"

namespace gameevent
{

	EventTutorial::EventTutorial(const std::istringstream& command)
		:
		EventBase("event_tutorial", command),
		release_(false),
		isgetText_(false),
		tutorial_box_(ci_ext::DrawObjf(
			ci_ext::Vec3f::zero(),							//pos
			ci_ext::Vec2f::one(),							//scale
			"dialog_box",									//resname
			ci_ext::Vec2f::zero(),							//src
			ci_ext::Vec2f::zero(),							//src size
			ci_ext::Color(255, 255, 255, 255),				//color
			0.0f											//degree
			))
	{}

	void EventTutorial::init()
	{
		alpha_ = 0;
		text_scale_ = 0.7f;
		tutorial_box_scale_x = gplib::system::WINW / DIALOG_BOX_WIDTH;
		tutorial_box_scale_y = 0.65f;
		tutorial_box_.setSrcSize(ci_ext::Vec2f(DIALOG_BOX_WIDTH, DIALOG_BOX_HEIGHT));
		tutorial_box_.setScale(ci_ext::Vec2f(tutorial_box_scale_x, tutorial_box_scale_y));
		tutorial_box_.setPos(0, gplib::system::WINH - DIALOG_BOX_HEIGHT *tutorial_box_scale_y, 0.0f);
		tutorial_box_.setColor(ci_ext::Color(alpha_, 0, 0, 0));

		if (map_.expired()) getMapPtr();
		auto map = ci_ext::weak_cast<map::Map>(map_);
		mapName_ = map.lock()->getNowMapName();
		loadFile(fileName_, mapName_);

	}

	//Resume from Sleep
	void EventTutorial::resume()
	{
		//Stop All
		getRootObject().lock()->pauseAll();

		{
			//Set controlable of player
			auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
			if (!player.expired()) {
				player.lock()->run();
				if (player.lock()->getPlayerState() != player::State::STAND)
					player.lock()->setPlayerState(player::State::STAND);
				player.lock()->setVelocity(0.0f, 0.0f);
				player.lock()->setControlable(false);
			}

			//Set map bg to run till the end
			auto map = getObjectFromRoot("map");
			if (!map.expired()) {
				map.lock()->run();
				auto bgs = map.lock()->getObjectsFromChildren({ "bg_" });
				for (auto& bg : bgs) {
					bg.lock()->run();
				}
			}

			//Set effect to run till the end
			auto effect = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effect.expired()) {
				effect.lock()->runAll();
			}
		}
		//Run this obj
		run();
	}

	void EventTutorial::update()
	{
		setTutorialText();
		if (!release_) {

			if (alpha_ < 240)
			{
				alpha_ += 20;
			}
			else
			{
				alpha_ = 255;
			}
			auto worldEvent = ci_ext::weak_cast<WorldEventManager>(getObjectFromRoot("world_event_manager"));
			if (!worldEvent.lock()->isNowShowingTutorial()) {
				release_ = true;
			}
		}
		else {
			alpha_ -= 20;
			if (alpha_ < 0)
			{
				alpha_ = 0;
				isgetText_ = false;
				kill();
			}
		}
	}

	void EventTutorial::render()
	{
#ifdef _DEBUG

	    //gplib::font::TextNC(20, 450, 0.0f, "Event Talk running, press A to exit", ARGB(255, 255, 0, 0), 0);
		//gplib::font::TextNC(20, 200, 0.0f, "Now mapname :" + mapName_, ARGB(255, 255, 0, 0), 0);
		//gplib::font::TextNC(20, 220, 0.0f, "Alpha :" + std::to_string(alpha_), ARGB(255, 255, 0, 0), 0);
		
#endif


	}
	void EventTutorial::renderLater()
	{
		if (isgetText_)
		{
			tutorial_box_.setColor(ci_ext::Color(alpha_, 255, 255, 255));
			tutorial_box_.renderLeftTopNC();
			gplib::draw::GraphLeftTopNC(text_jp_pos_x_, text_jp_pos_y_, 0.0f, resName_jp_, 0, 0, srcsize_jp_x_, srcsize_jp_y_, 0.0f, nullptr, text_scale_, text_scale_, alpha_);
			gplib::draw::GraphLeftTopNC(text_en_pos_x_, text_en_pos_y_, 0.0f, resName_en_, 0, 0, srcsize_en_x_, srcsize_en_y_, 0.0f, nullptr, text_scale_, text_scale_, alpha_);
		}
	}

	void EventTutorial::loadFile(std::string fileName, std::string mapName)
	{
		std::ifstream f(fileName);
		if (f.is_open())
		{
			while (!f.eof())
			{
				std::string label;
				f >> label;
				if (label == "mapName")
				{
					f >> label;
					if (label == mapName)
					{
						f >> label;
						if (label == "{")
						{
							while (label != "}")
							{
								f >> label;
								if (label == "resName_JP")
								{
									f >> dialogs_.resName_jp;
								}
								else if (label == "resName_EN")
								{
									f >> dialogs_.resName_en;
								}
								else if (label == "src_jp_x")
								{
									f >> dialogs_.srcsize_jp_x;
								}
								else if (label == "src_jp_y")
								{
									f >> dialogs_.srcsize_jp_y;
								}
								else if (label == "src_en_x")
								{
									f >> dialogs_.srcsize_en_x;
								}
								else if (label == "src_en_y")
								{
									f >> dialogs_.srcsize_en_y;
								}
							}
							break;
						}

					}
				}
			}
		}
		f.close();

	}

	void EventTutorial::setTutorialText()
	{
		//“ú–{Œê
		resName_jp_ = dialogs_.resName_jp;
		text_jp_pos_x_ = gplib::system::WINW / 2 - ((dialogs_.srcsize_jp_x / 2) * text_scale_);
		text_jp_pos_y_ = gplib::system::WINH - ((DIALOG_BOX_HEIGHT / 2) * tutorial_box_scale_y) - ((dialogs_.srcsize_jp_y / 2) * text_scale_) - ((dialogs_.srcsize_en_y / 2) * text_scale_);
		srcsize_jp_x_ = dialogs_.srcsize_jp_x;
		srcsize_jp_y_ = dialogs_.srcsize_jp_y;

		//‰pŒê
		resName_en_ = dialogs_.resName_en;
		text_en_pos_x_ = gplib::system::WINW / 2 - ((dialogs_.srcsize_en_x / 2) * text_scale_);
		text_en_pos_y_ = gplib::system::WINH - ((DIALOG_BOX_HEIGHT / 2) * tutorial_box_scale_y) + ((dialogs_.srcsize_en_y / 2) *text_scale_);
		srcsize_en_x_ = dialogs_.srcsize_en_x;
		srcsize_en_y_ = dialogs_.srcsize_en_y;

		isgetText_ = true;
	}

	inline void EventTutorial::getMapPtr()
	{
		map_ = getObjectFromRoot("map");
	}


}