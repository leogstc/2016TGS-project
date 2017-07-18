#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"
#include "../object/animation.hpp"

namespace effect {
	class EffectBase : public ci_ext::Object {
	protected:
		int posx_, posy_;
		const float posz_;
		anim::Anim anim_;
		int cnt_;

	public:
		EffectBase(const std::string& objectName, int x, int y)
			:
			Object(objectName), posx_(x), posy_(y), posz_(0.56f)
		{}
		virtual ~EffectBase() {}

		void init() override {}
		void update() override {}
		void render() override {}
		void renderLater() override {}
	};
}