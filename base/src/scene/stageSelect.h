#pragma once

#include "../lib/ci_ext/object.hpp"

namespace scene {
	class StageSelect : public ci_ext::Object {
		template<typename T>
		void nextscene(T* p)
		{
			kill();
			insertToParent(p);
		}

	public:
		StageSelect();
		void init() override;

		void update() override;

		void render() override;

	};

}