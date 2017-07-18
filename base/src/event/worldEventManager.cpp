#include "worldEventManager.h"

#include "eventTalk.hpp"
#include "eventTutorial.h"

namespace gameevent {
	WorldEventManager::WorldEventManager()
		:
		Object("world_event_manager")
	{
		//Create Table
		//cmdLists_["talk"] = std::bind(&WorldEventManager::createTalk, this, std::placeholders::_1);
		cmdLists_["talk"] = createInstance<EventTalk>;
		cmdLists_["tutorial"] = createInstance<EventTutorial>;
		
		//cmdLists_["switch"] = createInstance<EventTalk>;
	}

	void WorldEventManager::init()
	{
		nowShowingTutorial_ = false;
	}

	void WorldEventManager::doEvent(const std::string& command)
	{
		//All Command set
		std::istringstream cmd(command);

		//Main Command set
		std::string mainCmd;
		cmd >> mainCmd;
		

		//Tutorial‚ÍPAUSE‚ð‚µ‚È‚¢B
		if (mainCmd == "tutorial" && !nowShowingTutorial_)
		{
			nowShowingTutorial_ = true;
			insertAsChild(cmdLists_[mainCmd](cmd));
		}

		else {
			//Insert as sleep for 0 frame -> debug of changing kill() state to run/pause
			//new instance MUST use resume() instead of init()
			if (cmdLists_.find(mainCmd) != cmdLists_.end())
				insertAsChildSleep(cmdLists_[mainCmd](cmd), 0);
		}
	}

}