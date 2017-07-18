#pragma once

#include "../lib/ci_ext/object.hpp"
#include <iostream>
#include <map>

#include "../object/MenuButton.hpp"
#include "Stage.hpp"
#include "../title/tree.h"
#include "../title/backTree.h"
#include "../title/cloud.h"
#include "../scene/loading.h"
//#include "../scene/stageSelect.h"
#include "../title/pleaseText.h"
#include "../player/mover.h"
#include "../menu/menuStageSelect.hpp"

namespace scene
{

	class Title : public ci_ext::Object
	{
		struct BK { float x, y; }bk1, bk2;
		float bkW;
		float ground;

		std::shared_ptr<Mover> mover_;
		std::weak_ptr<Object> timer_;

		template<typename T>
		void nextscene(T* p)
		{
			kill();
			insertToParent(p);
		}

	public:
		Title()
			:
			Object("scene_title")
		{
			//releaseResource();

			/*gplib::se::Init();
			gplib::bgm::Init();*/
			//gplib::draw::LoadObject("bg", "res/gra/bg/BG 1280.png");
			//gplib::draw::LoadObject("block", "res/gra/title/block.png");
			//gplib::draw::LoadObject("tree1", "res/gra/title/tree1.png");
			//gplib::draw::LoadObject("backTree", "res/gra/title/tree2.png");
			//gplib::draw::LoadObject("titlelogo", "res/gra/title/logo.png");
			//gplib::draw::LoadObject("please", "res/gra/title/pleaseA.png");
			//gplib::draw::LoadObject("cloud1", "res/gra/title/cloud1.png");
			//gplib::draw::LoadObject("easy", "res/gra/title/easy.png");
			//gplib::draw::LoadObject("nomal", "res/gra/title/normal.png");
			////gplib::draw::LoadObject("normal", "res/gra/title/normal.png");
			//gplib::draw::LoadObject("hard", "res/gra/title/hard.png");
			//gplib::draw::LoadObject("cursol", "res/gra/title/star.png");
			//gplib::se::LoadFile("decision", "res/snd/decision.wav");
			//gplib::bgm::LoadFile("title", "res/snd/title.mp3");
		}

		~Title()
		{
			//gplib::se::Delete("decision");
			//gplib::bgm::Delete("title");
			//gplib::draw::DeleteAll();
		}

		void init() override
		{
			gplib::bgm::Play("title");
			//ルートのオブジェクトにあるwindownに対して、背景色を黒にする命令を送る
			auto root = ci_ext::weak_to_shared<Root>(getRootObject());
			root->changeBkColor(0x000000);

			bk1.x = 0.f;
			bkW = 1364.f;
			bk1.y = (float)gplib::system::WINH - 62.f;
			bk2.x = bk1.x + bkW - 1.f;
			bk2.y = (float)gplib::system::WINH - 62.f;
			ground = (float)gplib::system::WINH - 62.f;

			//オブジェクトを追加
			insertAsChild(new titleObj::Tree("tree1", ground));
			insertAsChild(new titleObj::BackTree("backTree", ground, 200.f));
			insertAsChild(new titleObj::BackTree("backTree", ground, 600.f));
			insertAsChild(new titleObj::Cloud("cloud1", ground, 100.f));
			insertAsChild(new titleObj::Cloud("cloud1", ground, 700.f));
			insertAsChild(new titleObj::TitleText(10));

			mover_ = std::make_shared<KeyMover>();

			timer_ = insertAsChild(new game::Timer("demo_timer", 10.f));
		}

		void update() override
		{
			mover_->CheckKey();

			bk1.x -= 5.f;
			bk2.x -= 5.f;
			if (bk1.x + bkW <= 0.f)
				bk1.x = (float)gplib::system::WINW;
			else if (bk2.x + bkW <= 0.f)
				bk2.x = (float)gplib::system::WINW;

			using namespace gplib;
			using namespace input;

			auto pleaseA = ci_ext::weak_cast<titleObj::TitleText>(getObjectFromChildren("titleText"));
			if (!pleaseA.expired()) {
				if (CheckPush(KEY_BTN0) || mover_->CheckPush(Mover::Key::A))
				{
					gplib::se::Play("decision");
					//auto pleaseA = ci_ext::weak_cast<titleObj::TitleText>(getObjectFromChildren("titleText"));
					//if (!pleaseA.expired())
						pleaseA.lock()->kill();

					auto gameFlag = getObjectFromRoot("game_flag");
					if (gameFlag.expired())
						getRootObject().lock()->insertAsChild(new game::GameFlag());

					//insertAsChild(new map::bg::BgStar());
					//insertAsChild(new map::bg::BgDigitalLine());
					insertAsChild(new menu::MenuStageSelect());
					//insertAsChild(new effect::EffectManager());
					//insertAsChild(new StageSelect());
					//getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::STAGE, "demo"));
					//kill();
				}
			}

			//Demo
			auto create_timer = ci_ext::weak_to_shared<game::Timer>(timer_);
			if (create_timer->isPause())
			{
				create_timer->run();
			}
			if (create_timer->isTimeOver())
			{
				gplib::bgm::Stop("title");
				getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::STAGE, "easy_demo"));
				timer_.lock()->kill();
				kill();
			}
			if (mover_->CheckPush(Mover::Key::A))
			{
				timer_.lock()->kill();
				timer_ = insertAsChild(new game::Timer("demo_timer", 30.f));
			}
			//if (!timer_.expired() && timer_.lock()->isDestroy()) {
			//	gplib::bgm::Stop("title");
			//	getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::STAGE, "easy_demo"));
			//	kill();
			//}

			gplib::bgm::EndCheck();
		}

		void render() override
		{
			gplib::draw::GraphLeftTop(0.f, 0.f, 1.f, "bg", 0, 0, 1280, 720);
			gplib::draw::GraphLeftTop(bk1.x, ground, 0.1f, "block", 0, 0, (int)bkW, 62);
			gplib::draw::GraphLeftTop(bk2.x, ground, 0.1f, "block", 0, 0, (int)bkW, 62);
			gplib::draw::Graph((float)gplib::system::WINW / 2.f, 342.f, 0.f, "titlelogo", 0, 0, 524, 359);
		}

		//void releaseResource()
		//{
		//	//Images
		//	gplib::draw::DeleteAll();
		//	//BGM
		//	gplib::bgm::DeleteAll();
		//	//SE
		//	gplib::se::DeleteAll();
		//}

	};
}