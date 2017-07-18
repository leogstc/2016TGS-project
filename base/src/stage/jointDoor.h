#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/ci_ext/DrawObj.hpp"
#include "../object/animation.hpp"
#include "../stage/stageInfo.h" //JointType

namespace map {
	class JointDoor : public ci_ext::Object {
		bool close_; //閉じるだけの扉か？
		bool finishCloseFlag_; //閉じるだけの扉の最終処理フラグ
		info::JointType type_;
		ci_ext::DrawObjf body_; //render()のため
		anim::Anim anim_;
		bool useAnim_; //アニメーションを使うか？　使わない->既に閉じている扉
	public:
		JointDoor(int id, info::JointType type, RECT rt, bool close = false, bool anim = true);

		void init() override;
		void update() override;
		void render() override;

		void resumeAnim(); //Use by Opening Door
	};
}