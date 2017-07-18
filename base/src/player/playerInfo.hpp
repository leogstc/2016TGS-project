/*
----- Playerの情報 -----
プレイヤー・UI などはこのPlayerInfoクラスから情報を取得・更新
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

		//残機
		int life_;
		int startLife_ = 3;
		int maxLife_ = 3;

		//現在のライフで取ったアイテム
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

		//パラメータ関係
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
		//復活の時
		void resetLife() {
			hp_ = maxHp_;
			mp_ = maxMp_;

			//フラグをリセット
			//itemFlag_.clear();
		}

		void resetContinue()
		{
			life_ = startLife_;
		}

		//アイテム関係
		//void addItemFlag(int id) { itemFlag_.push_back(id); }
		//bool isItemFlagOn(int id) const {
		//	if (std::find(itemFlag_.begin(), itemFlag_.end(), id) != itemFlag_.end())
		//		return true;
		//	return false;
		//}
	};
}