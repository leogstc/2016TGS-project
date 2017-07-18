#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../gameFlag.hpp"
#include "../stage/map.h"
#include "../player/player.h"
#include "worldEventManager.h"
#include <sstream>
#include <Windows.h>

//Debug RECT
#include "../lib/gplib.h"

namespace gameevent {
	class AutoEventArea : public ci_ext::Object {
		std::string name_;
		std::string cmd_;
		RECT rect_;
		std::weak_ptr<map::Map> map_;
	public:
		AutoEventArea(const std::string& name, const std::string& command, RECT area)
			:
			Object("auto_event_area_" + name), name_(name), cmd_(command), rect_(area)
		{}
		virtual ~AutoEventArea() {}

		void init() override
		{
			map_ = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		}

		void update() override
		{
			if (map_.expired())
				return;
			else {
				if (map_.lock()->isChangingMap())
					return;
			}

			//Player‚Æ‚Ì”»’è
			auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
			auto worldEvent = ci_ext::weak_cast<WorldEventManager>(getObjectFromRoot("world_event_manager"));
			if (!player.expired()) {
				RECT pRt = player.lock()->getBodyCollisionRect();
				RECT inRt; //Intersect Rect
				IntersectRect(&inRt, &pRt, &rect_);
				//Has an intersect
				if (inRt.right - inRt.left > 0) {
					//Set flag get this object
					auto gameFlag = ci_ext::weak_cast<game::GameFlag>(getObjectFromRoot("game_flag"));
					if (!gameFlag.lock()->isEventFlagOn(name_))
						gameFlag.lock()->addEventFlag(name_);

					//Create Event
					auto worldEvent = ci_ext::weak_cast<WorldEventManager>(getObjectFromRoot("world_event_manager"));
					worldEvent.lock()->doEvent(cmd_);

					//Delete This Obj
					kill();
				}
			}
			//gplib::draw::CkRect(rect_);
		}
	};
}