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
			filename.c_str(),	//パス
			D3DX_DEFAULT,	    //幅
			D3DX_DEFAULT,		//高さ
			D3DX_DEFAULT,		//ミップマップ
			0,					//レンダリングターゲットとしてつかうかどうか
			D3DFMT_UNKNOWN,		//テクスチャファイルフォーマット
			D3DPOOL_MANAGED,	//メモリの取り方
			D3DX_FILTER_NONE,	//フィルタリング方法（拡大縮小の演算方法）
			D3DX_DEFAULT,		//mip時のフィルタリング方法
			Transparent,		//抜け色（0指定で抜け色無効）
			&imgInfo_,			//読み込んだ画像の情報
			nullptr,			//パレットへのポインタ（使う際に面倒なので、無効）
			&tex_)				//テクスチャオブジェクトへのポインタ
			))
		{
			gplib::debug::BToMR("%sの読み込みに失敗しました", filename.c_str());
			assert(0);
			return;
		}
	}

	void NowLoading::drawPic(float x, float y, float z,
		int srcX, int srcY, int sizeX, int sizeY,
		float degree, float scaleX, float scaleY,
		u_char a, u_char r, u_char g, u_char b)
	{
		//描画矩形計算
		RECT rt = { srcX, srcY, srcX + sizeX, srcY + sizeY };

		D3DXVECTOR2 centerPos(static_cast<float>(sizeX) / 2.0f, static_cast<float>(sizeY) / 2.0f);

		D3DXMATRIX matDraw;	//座標変換マトリックスの格納先
		D3DXVECTOR3 ptCenter(centerPos.x, centerPos.y, 0.0f);	//描画の基準値の設定
		D3DXVECTOR3 position(0.0f, 0.0f, z);	//表示する位置を指定
		D3DXVECTOR2 draw(x, y);	//描画先座標（演算前）
		D3DXVECTOR2 scale(scaleX, scaleY);	//スケーリング係数（倍率を指定）

		D3DXVECTOR2 sCenter;
		sCenter = D3DXVECTOR2(0, 0);

		D3DXMatrixTransformation2D(&matDraw, &sCenter, 0.0f, &scale, NULL, D3DXToRadian(-degree), &draw);
		pSprite->SetTransform(&matDraw);
		//描画
		pSprite->Draw(tex_, &rt, &ptCenter, &position, D3DCOLOR_ARGB(a, r, g, b));
	}
}