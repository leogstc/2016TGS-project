#pragma once

namespace menu {
	class OptionBase {
	protected:
		int now_;
		int maxItems_;

		int cnt_; //速度のステップのカウンター
		int step_; //速度のステップ
		int interval_; //ステップ移動と連動 | 入力できるの間

	public:
		OptionBase(int maxOption, int now = 0) : now_(now), maxItems_(maxOption), interval_(0) {}
	};
}