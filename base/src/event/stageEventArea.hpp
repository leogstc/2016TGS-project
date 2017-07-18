#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../gameFlag.hpp"
#include "../player/player.h"
#include "worldEventManager.h"
#include <sstream>
#include <Windows.h>

//Debug
#include "../lib/gplib.h"

namespace gameevent {
	class EventArea : public ci_ext::Object {
		std::string name_;
		std::string cmd_;
		RECT rect_;
	public:
		EventArea(const std::string& name, const std::string& command, RECT area)
			:
			Object("event_area_" + name), name_(name), cmd_(command), rect_(area)
		{}
		virtual ~EventArea() {}

		void render() override
		{
			gplib::draw::CkRect(rect_);
		}

		void createEvent()
		{
			//Create Event
			auto worldEvent = ci_ext::weak_cast<WorldEventManager>(getObjectFromRoot("world_event_manager"));
			worldEvent.lock()->doEvent(cmd_);

			//Delete This Obj ‚µ‚È‚¢
			//kill();
		}

		bool checkCollision(RECT rt)
		{
			RECT inRt; //Intersect Rect
			IntersectRect(&inRt, &rt, &rect_);
			//Has an intersect
			if (inRt.right - inRt.left > 0)
				return true;

			return false;
		}
	};
}