#pragma once

#include <vector>

namespace anim {
	class Anim {
		int no_;	//���݂̉摜�ԍ�
		int startNo_; //�ŏ��̉摜�ԍ�

		int maxFrame_;
		int cnt_;

		int spd_; //�����X�s�[�h
		std::vector<int> diffSpd_; //�Ⴄ�X�s�[�h

		bool pause_; //��~
		bool backward_; //�t�]����

		int loop_;
	public:
		Anim()
			:
			no_(0), startNo_(0),
			maxFrame_(0), cnt_(0), spd_(0),
			pause_(false), backward_(false),
			loop_(0)
		{
			diffSpd_.clear();
		}
		void init() {
			no_ = 0;
			startNo_ = 0;
			maxFrame_ = 0;
			cnt_ = 0;
			spd_ = 0;
			diffSpd_.clear();
			pause_ = false;
			backward_ = false;
			loop_ = 0;
		}
		void resetStartNo() {
			startNo_ = 0;
		}
		void resetNo() {
			no_ = startNo_;
		}
		void resetCnt() {
			cnt_ = 0;
		}
		void resetLoop() {
			loop_ = 0;
		}
		void setAnim(int maxFrame, int framePerNo) {
			maxFrame_ = maxFrame;
			spd_ = framePerNo;
		}
		void setAnim(int maxFrame, const std::vector<int>& framePerNo) {
			maxFrame_ = maxFrame;
			diffSpd_ = framePerNo;
		}
		void setAnimReset(int maxFrame, int framePerNo, int startNo = 0) {
			no_ = 0;
			startNo_ = startNo;
			cnt_ = 0;
			maxFrame_ = maxFrame;
			spd_ = framePerNo;
			pause_ = false;
			backward_ = false;
			loop_ = 0;
		}
		void setAnimReset(int maxFrame, const std::vector<int>& framePerNo, int startNo = 0) {
			no_ = 0;
			startNo_ = startNo;
			cnt_ = 0;
			maxFrame_ = maxFrame;
			diffSpd_ = framePerNo;
			pause_ = false;
			backward_ = false;
			loop_ = 0;
		}
		void step() {
			if (pause_) return;

			//�����X�s�[�h
			if (diffSpd_.size() == 0) {
				cnt_++;
				if (cnt_ >= spd_) {
					//Forward
					if (!backward_) {
						no_++;
						if (no_ >= maxFrame_) {
							no_ = startNo_;
							loop_++;
						}
					}
					//Backward
					else {
						no_--;
						if (no_ < startNo_) {
							no_ = maxFrame_ - 1;
							loop_++;
						}
					}
					cnt_ = 0;
				}
			}
			//�Ⴄ�X�s�[�h
			else {
				cnt_++;
				if (cnt_ >= diffSpd_[no_]) {
					//Forward
					if (!backward_) {
						no_++;
						if (no_ >= maxFrame_) {
							no_ = startNo_;
							loop_++;
						}
					}
					//Backward
					else {
						no_--;
						if (no_ < startNo_) {
							no_ = maxFrame_ - 1;
							loop_++;
						}
					}
					cnt_ = 0;
				}
			}
		}
		void pause() {
			pause_ = true;
		}
		void resume() {
			pause_ = false;
		}
		void setAnimNo(int no) {
			no_ = no;
		}
		void setForward() {
			backward_ = false;
		}
		void setBackward() {
			backward_ = true;
		}
		void reverse() {
			backward_ = !backward_;
		}

		int getAnimNo() {
			return no_;
		}
		int getMaxAnimNo() {
			return maxFrame_ - 1 + startNo_;
		}
		int getLoop() {
			return loop_;
		}
		bool isPlay() {
			return !pause_;
		}
		bool isPause() {
			return pause_;
		}
		bool isFirstFrame() {
			if (backward_ == true) {
				if ((no_ == maxFrame_ - 1 + startNo_) && (cnt_ == 0))
					return true;
				return false;
			}
			else {
				if ((no_ == startNo_) && (cnt_ == 0))
					return true;
				return false;
			}
		}
		bool isFirstFrameStep() {
			if (backward_ == true) {
				if ((no_ == maxFrame_ - 1 + startNo_) && (cnt_ == 0))
					return true;
				return false;
			}
			else {
				if ((no_ == startNo_) && (cnt_ == 1))
					return true;
				return false;
			}
		}
		bool isLastFrame() {
			if (backward_ == false) {
				if ((no_ == maxFrame_ - 1 + startNo_) && (cnt_ == spd_ - 1))
					return true;
				return false;
			}
			else {
				if ((no_ == startNo_) && (cnt_ == spd_ - 1))
					return true;
				return false;
			}
		}
		bool isFirstFrameNo(int no)
		{
			if ((no_ == no) && (cnt_ == 0))
				return true;
			return false;
		}
		bool isFirstFrameStepNo(int no)
		{
			if ((no_ == no) && (cnt_ == 1))
				return true;
			return false;
		}
		bool isLastFrameNo(int no)
		{
			if ((no_ == no) && (cnt_ == spd_ - 1))
				return true;
			return false;
		}
		
	};
}