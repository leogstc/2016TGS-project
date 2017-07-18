#pragma once

namespace menu {
	class OptionBase {
	protected:
		int now_;
		int maxItems_;

		int cnt_; //���x�̃X�e�b�v�̃J�E���^�[
		int step_; //���x�̃X�e�b�v
		int interval_; //�X�e�b�v�ړ��ƘA�� | ���͂ł���̊�

	public:
		OptionBase(int maxOption, int now = 0) : now_(now), maxItems_(maxOption), interval_(0) {}
	};
}