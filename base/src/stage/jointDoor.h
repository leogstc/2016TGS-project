#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/ci_ext/DrawObj.hpp"
#include "../object/animation.hpp"
#include "../stage/stageInfo.h" //JointType

namespace map {
	class JointDoor : public ci_ext::Object {
		bool close_; //���邾���̔����H
		bool finishCloseFlag_; //���邾���̔��̍ŏI�����t���O
		info::JointType type_;
		ci_ext::DrawObjf body_; //render()�̂���
		anim::Anim anim_;
		bool useAnim_; //�A�j���[�V�������g�����H�@�g��Ȃ�->���ɕ��Ă����
	public:
		JointDoor(int id, info::JointType type, RECT rt, bool close = false, bool anim = true);

		void init() override;
		void update() override;
		void render() override;

		void resumeAnim(); //Use by Opening Door
	};
}