#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"

namespace ui {
	//--------------------------------------------------
	// y = x*x*x�̃O���t�𗘗p���ĕ�����𓮂���
	// See graph -> https://www.google.co.jp/search?q=y%3Dx^3
	//
	// x�̓O���t��x����position�Ƃ���y���v�Z����
	// �v�Z����y�̒l�͌��݂�x���W�Ɏg��
	//--------------------------------------------------
	class MarqueeText : public ci_ext::Object {
		enum class Flag {
			GOIN,
			CENTER,
			GOOUT,
			END,
		};
		struct Ease {
			std::string s;
			Flag flag;
			float x; //x-axis position (graph)
			float yCenter; //y-axis position (graph) when moving in the center
		};

		const float INIT_SPEED = 8.0f;
		const float SPACE_BETWEEN_TEXT = 1.2f;
		//���_����@���ƉE�@���ꂼ��̋��� �i���_����̃I�t�Z�b�g�j
		//(���_-��)����(���_+�E)�܂ł̊Ԃ�CENTER
		const float OFFSET = 100.0f;

		Ease* master_;
		Ease* ease_;
		size_t stringSize_;
		int xCenter_;

	public:
		MarqueeText(const std::string& s, int centerPosX = gplib::system::WINW / 2)
			:
			Object("ui_marquee"), xCenter_(centerPosX)
		{
			//std::string s("Now Loading");
			stringSize_ = s.size();
			master_ = new Ease[stringSize_];
			ease_ = new Ease[stringSize_];

			//Master���Z�b�g
			for (size_t i = 0; i < stringSize_; ++i) {
				master_[i].s = s[i];
				master_[i].flag = Flag::GOIN;

				//x-axis position (graph) are adjusted by character's width
				master_[i].x = INIT_SPEED + ((float)i * SPACE_BETWEEN_TEXT);
			}
			initText();
		}

		~MarqueeText()
		{
			delete[] master_;
			delete[] ease_;
		}

		void initText()
		{
			//Copy master to ease
			for (size_t i = 0; i < stringSize_; ++i) {
				ease_[i].s = master_[i].s;
				ease_[i].flag = master_[i].flag;
				ease_[i].x = master_[i].x;
			}
		}

		void update() override
		{
			float speed = 0.3f; //Step of decreasing x-axis (graph)
			float centerSpeed = 4.0f; //Speed @ ���S

			for (size_t i = 0; i < stringSize_; ++i) {
				//���_����E��(+x)�̌v�Z
				if (ease_[i].flag == Flag::GOIN) {
					//Before move
					int yPrev = static_cast<int>(ease_[i].x * ease_[i].x * ease_[i].x + OFFSET);
					ease_[i].x -= speed;
					//After move
					int yNow = static_cast<int>(ease_[i].x * ease_[i].x * ease_[i].x + OFFSET);
					//Due to OFFSET, if speed decreased to speed at center
					//--> Text is moved nearly to the OFFSET as close as it can w/ speed very close to centerSpeed
					if (yPrev - yNow <= (int)centerSpeed) {
						ease_[i].flag = Flag::CENTER;
						ease_[i].yCenter = (float)yNow; //pos on y-axis (graph)
					}
				}
				//���S�̌v�Z
				else if (ease_[i].flag == Flag::CENTER) {
					//Moving until out of OFFSET at other side
					ease_[i].yCenter -= centerSpeed;
					if (ease_[i].yCenter < -OFFSET) {
						ease_[i].flag = Flag::GOOUT;
						//Flip to other side of the graph ((-x)-axis)
						//w/ speed very close to centerSpeed
						ease_[i].x *= -1;
					}
				}
				//���_���獶��(-x)�̌v�Z
				else if (ease_[i].flag == Flag::GOOUT) {
					ease_[i].x -= speed;
					if (ease_[i].x < -INIT_SPEED) {
						ease_[i].flag = Flag::END;
					}
				}
			}

			//���ׂĂ̕�������ʊO�Ȃ珉����Ԃɖ߂�
			//end_iterator�͍Ō�̃f�[�^�̂P�� => ease_[stringSize_ - 1]�ł͂Ȃ� ease_[stringSize_]�ɂȂ�
			if (std::all_of(&ease_[0], &ease_[stringSize_], [](const Ease& e) { return (e.flag == Flag::END); })) {
				initText();
			}
		}

		void render() override
		{
			//���_���w��
			//int xCenter = gplib::system::WINW / 2;

			//�{�b�N�X�\��
			gplib::draw::BoxNC(0, gplib::system::WINH - 94, 1024, gplib::system::WINH - 34, 0, ARGB(255, 64, 64, 64), ARGB(255, 64, 64, 64), 1, 1);

			//�e������\��
			for (size_t i = 0; i < stringSize_; ++i) {
				int xPos;
				if (ease_[i].flag == Flag::GOIN)
					xPos = static_cast<int>(ease_[i].x * ease_[i].x * ease_[i].x + OFFSET); //y (graph) as +
				else if (ease_[i].flag == Flag::CENTER)
					xPos = static_cast<int>(ease_[i].yCenter); //y (graph) travelling from + to -
				else if (ease_[i].flag == Flag::GOOUT)
					xPos = static_cast<int>(ease_[i].x * ease_[i].x * ease_[i].x - OFFSET); //y (graph) as -
				else
					continue; //Skip all ended text

				//���_���痣�ꂽ�����ɑ΂���alpha���v�Z
				//�߂��F�͂����茩���� | ���������`�����F����
				//���̌v�Z���ɂ͌��_�ɋ߂����Ȃ��ƕ����������ɂȂ�
				int alpha = 255 - abs(int((float(xPos) / float(xCenter_)) * 255));
				if (alpha < 0) alpha = 0;
				gplib::font::TextNC(xPos + xCenter_, gplib::system::WINH - 74, 0, ease_[i].s, ARGB(alpha, 255, 64, 64), 1);
			}
		}
	};
}