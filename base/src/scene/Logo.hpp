#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/ci_ext/Counter.hpp"

#include "loading.h"
//#include "title.hpp"
#include "../object/root.h"

#include <iostream>

namespace scene
{

	class Logo : public ci_ext::Object
	{
		enum class State//シーン切り替え用
		{
			up,    //透明から非透明へ
			stop,  //ロゴ表示
			down,  //非透明から透明へ
			end    //終了処理
		};
		State state_;  //現在のシーン
		int alpha_;    //非透明度
		std::weak_ptr<ci_ext::Object> timer_; //経過時間を管理
		std::string resname_;

		//表示させる時間を秒で設定
		const float waitSecond_ = 1.0f;     //非表示からフェードイン開始までの秒数
		const float chhangeAlphaSec_ = 0.6f;//フェードインフェードアウトの秒数
		const float drawLogoSec_ = 2.0f;    //ロゴを表示している時間

		void nextscene()
		{
			//getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::GAME_OVER));
			
			//insertToParent(new Title());
			//getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::STAGE, "demo"));
			getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::TITLE));

			kill();
		}

		void changeState(State nextState)
		{
			switch (nextState)
			{
			case State::up:
				alpha_ = alpha_ < 0 ? 0 : alpha_;
				break;

			case State::stop:
				alpha_ = 255;
				break;

			case State::down:
				alpha_ = alpha_ > 255 ? 255 : alpha_;
				break;

			case State::end:
				//シーン遷移
				nextscene();
				break;
			}
			state_ = nextState;
		}
	public:
		Logo()
			:
			Object("scene_logo"),
			state_(State::up),
			alpha_(0),
			resname_("logo")
		{
			//gplib::draw::LoadObject(resname_, "res/gra/logo.png", 0);
		}

		//コンストラクタでは親子関係が構築できない仕様なので、
		//生成時に子供を登録したい場合はinit()で行う
		//init()はオブジェクトを一括追加するときに自動で呼ばれる
		void init() override
		{
			//カウンタを生成し、検索取得しなくて良いようにweak_ptrを保持しておく
			timer_ = insertAsChild(new ci_ext::TimeCounter<float>(name() + "timecounter_"));
			//ルートのオブジェクトにあるwindownに対して、背景色を白にする命令を送る
			auto root = ci_ext::weak_to_shared<Root>(getRootObject());
			root->changeBkColor(0xFFFFFF);
		}

		//このオブジェクトがメモリから破棄されるときに呼ばれる、デストラクタ
		~Logo() override
		{
			gplib::draw::DeleteObject(resname_);
		}

		//１フレームで行う表示処理、ロゴのみ表示する
		void render() override
		{
			auto w = gplib::draw::GetImageWidth(resname_);
			auto h = gplib::draw::GetImageHeight(resname_);
			gplib::draw::Graph(
				gplib::system::WINW / 2.0f, gplib::system::WINH / 2.0f, 0.5f,
				resname_,
				0, 0,
				w, h,
				0.0f,
				1.0f, 1.0f,
				//gplib::system::WINW / static_cast<float>(w), gplib::system::WINH / static_cast<float>(h),
				alpha_);
		}

		void renderPause() override
		{
			render();
			DOUT << "Logo::pause" << std::endl;
		}

		//１フレームで行う処理
		void update() override
		{
			float ms = 0;
			{
				//オブジェクトをロックして使用可能にし、タイマー生成からのミリ秒を取得する
				auto timer = ci_ext::weak_to_shared<ci_ext::TimeCounter<float>>(timer_);
				float temp = (timer->get() - waitSecond_);//フェードインに入るまでの時間は換算しない
				ms = temp < 0 ? 0 : temp;
			}
			//現在のステートによって処理を変更
			switch (state_)
			{
				//非透明度を加算
			case State::up:
				alpha_ = static_cast<int>(255.0f * (ms / chhangeAlphaSec_));
				if (ms > chhangeAlphaSec_)
					changeState(State::stop);
				break;

				//停止中
			case State::stop:
				//規定秒たったら、透明化の処理へ移行
				if (ms > chhangeAlphaSec_ + drawLogoSec_)
					changeState(State::down);
				break;

				//透明へ
			case State::down:
			{
				//フェードアウトに入る時間から規定の秒数でフェードアウトを終了させる
				auto temp = (chhangeAlphaSec_ + chhangeAlphaSec_ + drawLogoSec_) - ms;
				alpha_ = static_cast<int>(255.0f * (temp / chhangeAlphaSec_));
				if (temp < 0.0f)
					changeState(State::end);
			}
			break;
			}
		}

	};

}