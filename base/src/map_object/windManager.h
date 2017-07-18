#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../stage/stageInfo.h" //WindDIR

namespace object {
	class WindManager : public ci_ext::Object {
	public:
		WindManager();
		void createWind(int x, int y, map::info::WindDIR dir, std::string move, float speed, int range);
		void destroyWinds();
	};
}