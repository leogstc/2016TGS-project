#pragma once

#include "../lib/ci_ext/object.hpp"

namespace gameevent {
	class StageEventManager : public ci_ext::Object {
	public:
		StageEventManager();

		void createEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey);
		void createAutoEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey);
		void createTutorialEventArea(const std::string& name, const std::string& command, int posx, int posy, int sizex, int sizey);
		void destroyAllEventAreas();
	};
}