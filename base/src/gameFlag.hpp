#pragma once

#include "lib/ci_ext/object.hpp"

namespace game {
	/*enum UniqueItem {
		UNIQUE_ITEM01 = 1,
		UNIQUE_ITEM02 = (1 << 1),
		UNIQUE_ITEM03 = (1 << 2),
		UNIQUE_ITEM04 = (1 << 3),
	};*/


	class GameFlag : public ci_ext::Object {
		std::vector<int> uniqueItems_; //Id start from 1

		//‹N‚±‚Á‚½ event name ‚ð•Û‘¶
		std::vector<std::string> eventLists_;

		//Stage Cleared Flag
		std::vector<std::string> clearedLists_;

	public:
		GameFlag()
			:
			Object("game_flag")
		{
			uniqueItems_.clear();
			eventLists_.clear();
		}

		void addUniqueItemFlag(int id) { uniqueItems_.push_back(id); }
		//int getUniqueItemFlag() { return uniqueItem_; }
		bool isUniqueItemFlagOn(int id)
		{
			if (std::find(uniqueItems_.begin(), uniqueItems_.end(), id) != uniqueItems_.end())
				return true;
			return false;

			/*if ((uniqueItem_ & flag) != 0)
				return true;
			return false;*/
		}

		void addEventFlag(const std::string& name) { eventLists_.push_back(name); }
		bool isEventFlagOn(const std::string& name)
		{
			if (std::find(eventLists_.begin(), eventLists_.end(), name) != eventLists_.end())
				return true;
			return false;
		}

		void addClearedFlag(const std::string& name) { clearedLists_.push_back(name); }
		bool isClearedFlagOn(const std::string& name)
		{
			if (std::find(clearedLists_.begin(), clearedLists_.end(), name) != clearedLists_.end())
				return true;
			return false;
		}
	};
}