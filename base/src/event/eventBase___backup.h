#pragma once

#include "../lib/ci_ext/object.hpp"
#include <sstream>

namespace gameevent {
	class EventBase : public ci_ext::Object {
	protected:
		std::istringstream cmd_;
	public:
		EventBase(const std::string& objectName, const std::istringstream& command)
			:
			Object(objectName), cmd_(command.str())
		{}
		virtual ~EventBase() {}
	};
}