/*
----- Player�̏�� -----
�v���C���[�EUI �Ȃǂ͂���PlayerInfo�N���X��������擾�E�X�V
*/

#pragma once

#include "../lib/ci_ext/object.hpp"

namespace player {
	class PlayerInfo : public ci_ext::Object {
		//HP
		float hp_;
		float maxHp_ = 100.0f;

		//MP
		float mp_;
		float maxMp_ = 100.0f;

		//�c�@
		int life_;
		int startLife_ = 3;
		int maxLife_ = 3;

		//���݂̃��C�t�Ŏ�����A�C�e��
		//std::vector<int> itemFlag_;

	public:
		PlayerInfo()
			:
			Object("player_info")
		{}

		void init() override
		{
			hp_ = maxHp_;
			mp_ = maxMp_;
			life_ = startLife_;
		}

		//�p�����[�^�֌W
		void offsetHp(float val) { hp_ += val; }
		void offsetMp(float val) { mp_ += val; }
		void offsetLife(int val) { life_ += val; }

		void setHP(float val) { hp_ = val; }
		void setMP(float val) { mp_ = val; }

		float getHp() const { return hp_; }
		float getMp() const { return mp_; }

		float getMaxHp() const { return maxHp_; }
		float getMaxMp() const { return maxMp_; }


		int getLife() const { return life_; }
		//�����̎�
		void resetLife() {
			hp_ = maxHp_;
			mp_ = maxMp_;

			//�t���O�����Z�b�g
			//itemFlag_.clear();
		}

		void resetContinue()
		{
			life_ = startLife_;
		}

		//�A�C�e���֌W
		//void addItemFlag(int id) { itemFlag_.push_back(id); }
		//bool isItemFlagOn(int id) const {
		//	if (std::find(itemFlag_.begin(), itemFlag_.end(), id) != itemFlag_.end())
		//		return true;
		//	return false;
		//}
	};
}