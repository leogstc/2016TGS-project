#pragma once

#include "optionBase.hpp"
#include "../lib/ci_ext/object.hpp"
#include <Windows.h>

namespace menu {
	class MenuUpDownBase : public OptionBase, public ci_ext::Object {
	protected:
		//選択肢を表示できるエリア
		RECT limit_;

	public:
		MenuUpDownBase(const std::string& objectName, int maxOption, int now = 0);
		~MenuUpDownBase();
		void init() override;
		void update() override;
		void render() override;

	protected:
		void input();
		void CursorSpeedControl(int interval);
	};
}