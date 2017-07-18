#pragma once

#include "optionBase.hpp"
#include "../lib/ci_ext/object.hpp"

namespace menu {
	class MenuLeftRightBase : public OptionBase, public ci_ext::Object {
	protected:
		bool skipUp_;
		bool skipDown_;

	public:
		MenuLeftRightBase(const std::string& objectName, int maxOption, int now = 0);

		void init() override;
		void update() override;
		void render() override;

	protected:
		virtual void input();
		virtual void CursorSpeedControl(int interval);
	};
}