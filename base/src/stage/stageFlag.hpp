#pragma once

#include "../lib/ci_ext/object.hpp"

namespace map {
	class StageFlag : public ci_ext::Object {
		//�v���C���[�̕����ꏊ�t���O
		bool continueFlag_;

		//�A�C�e���̕����t���O
		std::vector<int> stageItemFlag_;

		//�G�̕������Ȃ��t���O
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

		//----- Player �����ꏊ
		void setContinue(bool flag = true) { continueFlag_ = flag; }

		bool isContinueFlagOn() const { return continueFlag_; }
		//----- End Player �����ꏊ

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