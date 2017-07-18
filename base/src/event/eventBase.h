#pragma once

#include "../lib/ci_ext/object.hpp"
#include <sstream>
#include "../lib/ci_ext/DrawObj.hpp"

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

		const float DIALOG_BOX_WIDTH = 1290.f;
		const float DIALOG_BOX_HEIGHT = 308.f;
	};


}