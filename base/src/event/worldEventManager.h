#pragma once

#include "../lib/ci_ext/object.hpp"
#include "eventBase.h"
#include <sstream>
#include <unordered_map>

namespace gameevent {

	struct DIALOG
	{
		std::string srcname_jp;
		std::string srcname_en;
		float srcsize_jp_x, srcsize_jp_y;
		float srcsize_en_x, srcsize_en_y;
	};

	template <typename T>
	EventBase* createInstance(const std::istringstream& command)
	{
		return new T(command);
	}

	class WorldEventManager : public ci_ext::Object {
		//std::unordered_map<std::string, std::function<void(const std::istringstream&)>> cmdLists_;
		std::unordered_map<std::string, EventBase*(*)(const std::istringstream&)> cmdLists_;

	private:
		bool nowShowingTutorial_ ;
	public:
		WorldEventManager();
		void doEvent(const std::string& command);
		void init() override;
		bool isNowShowingTutorial() { return nowShowingTutorial_; };
		void setTutorialOff() { nowShowingTutorial_ = false; };
	};
}