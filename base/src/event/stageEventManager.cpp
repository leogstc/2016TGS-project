#include "stageEventManager.h"
#include "stageEventArea.hpp"
#include "stageAutoEventArea.hpp"
#include "stageTutorialEventArea.hpp"
#include <sstream> //istringstream
#include <Windows.h> //RECT

namespace gameevent {
	StageEventManager::StageEventManager()
		:
		Object("stage_event_manager")
	{}

	void StageEventManager::createEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey)
	{
		int offset = 2;
		RECT area = { posx + offset, posy + offset, sizex - offset, sizey - offset };
		insertAsChild(new EventArea(name, command, area));
	}

	void StageEventManager::createAutoEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey)
	{
		int offset = 2;
		RECT area = { posx + offset, posy + offset, sizex - offset, sizey - offset };
		insertAsChild(new AutoEventArea(name, command, area));
	}

	void StageEventManager::createTutorialEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey)
	{
		int offset = 2;
		RECT area = { posx + offset, posy + offset, sizex - offset, sizey - offset };
		insertAsChild(new TutorialEventArea(name, command, area));
	}

	void StageEventManager::destroyAllEventAreas()
	{
		auto events = getChildren();
		for (auto& event : events) {
			event.lock()->kill();
		}
	}
}