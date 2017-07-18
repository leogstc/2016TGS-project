#pragma once

#include "../lib/ci_ext/object.hpp"

namespace map {
	class StageFlag : public ci_ext::Object {
		//プレイヤーの復活場所フラグ
		bool continueFlag_;

		//アイテムの復活フラグ
		std::vector<int> stageItemFlag_;

		//敵の復活しないフラグ
		std::vector<int> stageEnemyFlag_;

	public:
		StageFlag()
			:
			Object("stage_flag"),
			continueFlag_(false)
		{
			stageItemFlag_.clear();
			stageEnemyFlag_.clear();
		}

		~StageFlag()
		{
			stageItemFlag_.clear();
			stageEnemyFlag_.clear();
		}

		//----- Player 復活場所
		void setContinue(bool flag = true) { continueFlag_ = flag; }

		bool isContinueFlagOn() const { return continueFlag_; }
		//----- End Player 復活場所

		//----- Item Flag
		void addItemFlag(int id) { stageItemFlag_.push_back(id); }
		bool isItemFlagOn(int id) const {
			if (std::find(stageItemFlag_.begin(), stageItemFlag_.end(), id) != stageItemFlag_.end())
				return true;
			return false;
		}
		//----- End Item Flag

		//----- Enemy Flag
		void addEnemyFlag(int id) { stageEnemyFlag_.push_back(id); }
		bool isEnemyFlagOn(int id) const {
			if (std::find(stageEnemyFlag_.begin(), stageEnemyFlag_.end(), id) != stageEnemyFlag_.end())
				return true;
			return false;
		}
		//----- End Enemy Flag
	};
}