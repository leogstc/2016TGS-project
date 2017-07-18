#pragma once

#include "ui_loading.h"
extern LPDIRECT3DDEVICE9 pD3DDevice;
extern LPD3DXSPRITE pSprite;

namespace ui {

	NowLoading::NowLoading()
		:
		Object("ui_now_loading")
	{
		//Load Graphic from File
		createTexture();
	}
	NowLoading::~NowLoading()
	{
		//Safe Release
		tex_->Release();
		tex_ = nullptr;
	}

	void NowLoading::init()
	{
		//First time ONLY
		for (int i = 0; i < STR_LEN; ++i) {
			auto& c = ch_[i];
			c.posx = static_cast<float>(CENTER_X + ((i - STR_LEN / 2) * (CHAR_WIDTH + SPACE_EACH_CHAR) - OFFSET_LEFT) * SCALE);
			c.posy = static_cast<float>(POS_Y);
			c.flag = Flag::START;
			c.t = 0 - (i * T_EACH_CHAR);
			c.angle = 0;
		}

		for (int i = 0; i < 2; ++i) {
			auto& p = pic_[i];
			p.posx = static_cast<float>(CENTER_X + (((STR_LEN / 2) * (CHAR_WIDTH + SPACE_EACH_CHAR)) + (PIC_WIDTH / 2) - OFFSET_LEFT) * SCALE);
			p.posy = static_cast<float>(POS_Y);
			p.srcx = STR_LEN * CHAR_WIDTH + (i * PIC_WIDTH * 2);
			p.angle = 0.0f + (i * 180.0f);
			p.scalex = -sinf(p.angle * (float)M_PI / 180.0f) * SCALE;
		}
	}

	void NowLoading::initText()
	{
		//2nd time & goes on
		for (int i = 0; i < STR_LEN; ++i) {
			auto& c = ch_[i];
			c.posx = static_cast<float>(CENTER_X + ((i - STR_LEN / 2) * (CHAR_WIDTH + SPACE_EACH_CHAR) - OFFSET_LEFT) * SCALE);
			c.posy = static_cast<float>(POS_Y);
			c.flag = Flag::START;
			c.t = -T_EACH_CHAR - (i * T_EACH_CHAR);
			c.angle = 0;
		}
	}

	void NowLoading::update()
	{
		//---------- Text ----------
		for (auto& c : ch_) {
			if (c.angle >= 180) continue;

			c.t++;
			//Move
			if (c.t > 0) {
				c.angle += 6;
				if (c.angle >= 180) {
					c.angle = 180;
					c.flag = Flag::END;
				}
				c.posy = MOVE_HEIGHT * SCALE * -sinf(c.angle * (float)M_PI / 180.0f) + POS_Y;
			}
		}

		//End iterator = after last one
		if (std::all_of(&ch_[0], &ch_[STR_LEN], [](const Char& c) { return (c.flag == Flag::END); })) {
			initText();
		}

		//---------- Picture ----------
		for (int i = 0; i < 2; ++i) {
			auto& p = pic_[i];
			p.angle += 3 + (i * 2);
			p.scalex = -sinf(p.angle * (float)M_PI / 180.0f) * SCALE;
		}
	}

	void NowLoading::render()
	{
		for (int i = 0; i < STR_LEN; ++i) {
			auto& c = ch_[i];
			drawPic(c.posx, c.posy, 0.0f, i * CHAR_WIDTH, 0, CHAR_WIDTH, CHAR_HEIGHT, 0.0f, SCALE, SCALE);
		}
		for (auto& p : pic_) {
			drawPic(p.posx, p.posy, 0.0f, p.srcx, 0, PIC_WIDTH, CHAR_HEIGHT, 0.0f, p.scalex, SCALE);
		}
	}

	void NowLoading::createTexture()
	{
		std::string filename = "res/gra/nowloading.png";
		D3DCOLOR Transparent = 0xFF000000;
		D3DXIMAGE_INFO imgInfo_;

		if (FAILED(D3DXCreateTextureFromFileEx(
			pD3DDevice,
			filename.c_str(),	//�p�X
			D3DX_DEFAULT,	    //��
			D3DX_DEFAULT,		//����
			D3DX_DEFAULT,		//�~�b�v�}�b�v
			0,					//�����_�����O�^�[�Q�b�g�Ƃ��Ă������ǂ���
			D3DFMT_UNKNOWN,		//�e�N�X�`���t�@�C���t�H�[�}�b�g
			D3DPOOL_MANAGED,	//�������̎���
			D3DX_FILTER_NONE,	//�t�B���^�����O���@�i�g��k���̉��Z���@�j
			D3DX_DEFAULT,		//mip���̃t�B���^�����O���@
			Transparent,		//�����F�i0�w��Ŕ����F�����j
			&imgInfo_,			//�ǂݍ��񂾉摜�̏��
			nullptr,			//�p���b�g�ւ̃|�C���^�i�g���ۂɖʓ|�Ȃ̂ŁA�����j
			&tex_)				//�e�N�X�`���I�u�W�F�N�g�ւ̃|�C���^
			))
		{
			gplib::debug::BToMR("%s�̓ǂݍ��݂Ɏ��s���܂���", filename.c_str());
			assert(0);
			return;
		}
	}

	void NowLoading::drawPic(float x, float y, float z,
		int srcX, int srcY, int sizeX, int sizeY,
		float degree, float scaleX, float scaleY,
		u_char a, u_char r, u_char g, u_char b)
	{
		//�`���`�v�Z
		RECT rt = { srcX, srcY, srcX + sizeX, srcY + sizeY };

		D3DXVECTOR2 centerPos(static_cast<float>(sizeX) / 2.0f, static_cast<float>(sizeY) / 2.0f);

		D3DXMATRIX matDraw;	//���W�ϊ��}�g���b�N�X�̊i�[��
		D3DXVECTOR3 ptCenter(centerPos.x, centerPos.y, 0.0f);	//�`��̊�l�̐ݒ�
		D3DXVECTOR3 position(0.0f, 0.0f, z);	//�\������ʒu���w��
		D3DXVECTOR2 draw(x, y);	//�`�����W�i���Z�O�j
		D3DXVECTOR2 scale(scaleX, scaleY);	//�X�P�[�����O�W���i�{�����w��j

		D3DXVECTOR2 sCenter;
		sCenter = D3DXVECTOR2(0, 0);

		D3DXMatrixTransformation2D(&matDraw, &sCenter, 0.0f, &scale, NULL, D3DXToRadian(-degree), &draw);
		pSprite->SetTransform(&matDraw);
		//�`��
		pSprite->Draw(tex_, &rt, &ptCenter, &position, D3DCOLOR_ARGB(a, r, g, b));
	}
}