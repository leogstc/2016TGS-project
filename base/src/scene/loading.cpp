#include "loading.h"

#include "../lib/gplib.h"
#include "../ui/ui_marquee.hpp"
#include "../ui/ui_loading.h"
#include "../io/stageReader.hpp"

//Next Scene
#include "Title.hpp"
//#include "stageSelect.h"
#include "stage.hpp"
#include "Logo.hpp"
#include "gameOver.hpp"
#include "end.h"


//-------------------- サンプルスレッド --------------------
//DWORD WINAPI threadFunctionName(LPVOID* param)
//{
//	while (ci_ext::Window::runThread) {
//		//処理
//	}
//	//終了する -> WM_DESTROYのループを抜ける (@ lib_Window.hpp -> OnDestroy_default())
//	ExitThread(0);
//}
//--------------------------------------------------


namespace scene {
	//スレッド
	DWORD WINAPI Loading::LoadNextScene(SendData* pSendData)
	{
		loadResource(*pSendData);
		ExitThread(0);
	}

	//実体
	map::StageInfo Loading::stageInfo_;

	//オブジェクト
	Loading::Loading(const Scene& nextScene, const std::string& command)
		:
		Object("scene_loading")
	{
		sendData_.nextScene = nextScene;
		sendData_.cmd = command;
	}

	Loading::~Loading()
	{
	}

	void Loading::init()
	{
		releaseResource();

		gplib::se::Init();
		//HANDLE CreateThread(
		//	LPSECURITY_ATTRIBUTES lpThreadAttributes, // セキュリティ記述子
		//	DWORD dwStackSize, // 初期のスタックサイズ
		//	LPTHREAD_START_ROUTINE lpStartAddress, // スレッドの機能 | (LPTHREAD_START_ROUTINE)キャストが必須
		//	LPVOID lpParameter, // スレッドの引数
		//	DWORD dwCreationFlags, // 作成オプション
		//	LPDWORD lpThreadId // スレッド識別子
		//);
		ci_ext::Window::hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loading::LoadNextScene, (void*)&sendData_, 0, NULL);

		//insertAsChild(new ui::MarqueeText("Now Loading"));
		insertAsChild(new ui::NowLoading());
		{
			auto root = ci_ext::weak_to_shared<Root>(getRootObject());
			root->changeBkColor(0x000000);
		}
	}

	void Loading::update()
	{
		//スレッドが終わったかをチェック
		DWORD result;
		GetExitCodeThread(ci_ext::Window::hThread, &result);
		//終わったらハンドルを閉じる。 | Add && hThread != NULL to debug if not kill() this object immediately
		if (result != STILL_ACTIVE && ci_ext::Window::hThread != NULL) {
			//CloseHandleで閉じる。 | hThreadが閉じている場合はプログラムが落ちる
			CloseHandle(ci_ext::Window::hThread);

			//Debug if not kill() this object immediately
			ci_ext::Window::hThread = NULL;

			nextScene(sendData_);
		}
		/*else if (result == STILL_ACTIVE && ci_ext::Window::hThread != NULL) {
			gplib::font::TextNC(100, 100, 0.0f, "Thread running:", ARGB(255, 255, 0, 0), 0);
			static int i = 0;
			++i;
			gplib::font::TextNC(270, 100, 0.0f, std::to_string(i), ARGB(255, 255, 0, 0), 0);
		}*/
	}

	void Loading::nextScene(const SendData& sendData)
	{
		switch (sendData.nextScene) {
		case Scene::LOGO:
			insertToParent(new Logo()); break;
		case Scene::TITLE:
			insertToParent(new Title()); break;
		//case Scene::STAGE_SELECT:
			//insertToParent(new StageSelect()); break;
		case Scene::STAGE:
			insertToParent(new Stage(sendData.cmd, stageInfo_)); break;
		case Scene::GAME_OVER:
			insertToParent(new GameOver()); break;
		case Scene::END:
			insertToParent(new End()); break;
		default:
			break;
		}
		kill();
	}

	void Loading::loadResource(const SendData& sendData)
	{
		switch (sendData.nextScene) {
		case Scene::LOGO:
			loadResLogo(); break;
		case Scene::TITLE:
			loadResTitle(); break;
		//case Scene::STAGE_SELECT:
			//loadResStageSelect(); break;
		case Scene::STAGE:
			loadResStage(sendData.cmd); break;
		case Scene::GAME_OVER:
			loadResGameOver(); break;
		case Scene::END:
			loadResEnd(); break;
		default:
			break;
		}
	}

	void Loading::loadResLogo()
	{
		gplib::draw::LoadObject("logo", "res/gra/logo.png", 0);
	}

	void Loading::loadResTitle()
	{
		gplib::draw::LoadObject("bg", "res/gra/bg/BG 1280.png");
		gplib::draw::LoadObject("block", "res/gra/title/block.png");
		gplib::draw::LoadObject("tree1", "res/gra/title/tree1.png");
		gplib::draw::LoadObject("backTree", "res/gra/title/tree2.png");
		gplib::draw::LoadObject("titlelogo", "res/gra/title/logo.png");
		gplib::draw::LoadObject("please", "res/gra/title/pleaseA.png");
		gplib::draw::LoadObject("cloud1", "res/gra/title/cloud1.png");
		gplib::draw::LoadObject("easy", "res/gra/title/easy.png");
		gplib::draw::LoadObject("nomal", "res/gra/title/normal.png");
		//gplib::draw::LoadObject("normal", "res/gra/title/normal.png");
		gplib::draw::LoadObject("hard", "res/gra/title/hard.png");
		gplib::draw::LoadObject("cursol", "res/gra/title/star.png");
		gplib::se::LoadFile("decision", "res/snd/decision.wav");
		gplib::bgm::LoadFile("title", "res/snd/title.mp3");
	}

	//void Loading::loadResStageSelect()
	//{
	//}

	void Loading::loadResStage(const std::string& stageName)
	{
		//Stage
		{
			{
				io::StageReader reader(stageName, stageInfo_);
			}

			gplib::draw::LoadObject("map", "res/gra/stage/" + stageName + ".png");
			gplib::draw::LoadObject("joint_door", "res/gra/stage/joint_door.png");

			//BG
			//gplib::draw::LoadObject("bg_falling_leaf", "res/gra/bg/falling_leaf.png");
			gplib::draw::LoadObject("bg", "res/gra/bg/BG 1280.png");
		}
		
		//Player
		{
			//gplib::draw::LoadObject("player", "res/gra/player.png", 0xFF2F5F73);
			gplib::draw::LoadObject("Tplayer", "res/gra/player00.png", 0xFF2F5F73);
			gplib::draw::LoadObject("shot", "res/gra/shot.png");
		}

		//Item
		{
			gplib::draw::LoadObject("item", "res/gra/item2.png");
		}

		//Effect
		{
			gplib::draw::LoadObject("effect_mahoujin", "res/gra/effect/mahoujin.png");
			gplib::draw::LoadObject("effect_clear", "res/gra/effect/clear.png");
			gplib::draw::LoadObject("effect_ready", "res/gra/effect/ready.png");
			gplib::draw::LoadObject("effect_ready_star", "res/gra/effect/star.png");
			gplib::draw::LoadObject("effect_pop_star", "res/gra/effect/popstar.png");
			
			gplib::draw::LoadObject("effect_gilgamesh", "res/gra/effect/muzzle.png");
		}

		//Enemy
		{
			//gplib::draw::LoadObject("enemy001", "res/gra/enemy/enemy001.png");
			//gplib::draw::LoadObject("enemy002", "res/gra/enemy/enemy002.png");
			//gplib::draw::LoadObject("enemy_orangeSlime", "res/gra/enemy/enemy_orangeSlime.png");
			//gplib::draw::LoadObject("enemy_koh", "res/gra/player.png");
			gplib::draw::LoadObject("enemy_bat", "res/gra/enemy/enemy_bat.png");
			gplib::draw::LoadObject("enemy_monster", "res/gra/enemy/enemy_monster.png");
			gplib::draw::LoadObject("enemy_boss", "res/gra/enemy/boss.png");
			gplib::draw::LoadObject("enemyshot", "res/gra/shot00.png");
			gplib::draw::LoadObject("enemy_slime", "res/gra/enemy/enemy_slime.png");
			gplib::draw::LoadObject("enemy_archer", "res/gra/enemy/enemy_archer.png");
			gplib::draw::LoadObject("enemy_archer_arrow", "res/gra/enemy/arrow.png");
			gplib::draw::LoadObject("enemymine", "res/gra/spike.png");
			gplib::draw::LoadObject("boss_shot", "res/gra/boss_shot.png");
		}

		//EnemyMask
		{
			gplib::draw::LoadObject("mask_enemy_bat", "res/gra/enemy/enemy_bat_mask.png");
			gplib::draw::LoadObject("mask_enemy_fly", "res/gra/enemy/enemy_bat_mask.png");
			gplib::draw::LoadObject("mask_enemy_slime", "res/gra/enemy/enemy_slime_mask.png");
			gplib::draw::LoadObject("mask_enemy_archer", "res/gra/enemy/enemy_archer_mask.png");
			gplib::draw::LoadObject("mask_enemy_monster", "res/gra/enemy/enemy_monster_mask.png");
			gplib::draw::LoadObject("mask_enemy_boss", "res/gra/enemy/boss_mask.png");
		}

		//Object
		{
			gplib::draw::LoadObject("wind", "res/gra/wind.png");
		}

		//UI
		{
			//Player
			gplib::draw::LoadObject("hp_frame", "res/gra/ui/hp_frame.png");
			gplib::draw::LoadObject("hp_bar", "res/gra/ui/hp_bar.png");
			gplib::draw::LoadObject("mp_frame", "res/gra/ui/mp_frame.png");
			gplib::draw::LoadObject("mp_bar", "res/gra/ui/mp_bar.png");
			gplib::draw::LoadObject("avatar", "res/gra/ui/avatar2.png");
			gplib::draw::LoadObject("effect", "res/gra/ui/effect.png");
			gplib::draw::LoadObject("effectStar", "res/gra/ui/star2.png");

			gplib::draw::LoadObject("bar_mask", "res/gra/ui/bar_mask.png");
			gplib::draw::LoadObject("frame_mask", "res/gra/ui/frame_mask.png");
			gplib::draw::LoadObject("effect", "res/gra/ui/frame_mask.png");
		
			//Boss
			gplib::draw::LoadObject("boss_hp_frame", "res/gra/ui/boss_frame.png");
			gplib::draw::LoadObject("boss_hp_bar", "res/gra/ui/boss_bar.png");
			gplib::draw::LoadObject("boss_avatar", "res/gra/ui/boss_avatar.png");

			gplib::draw::LoadObject("boss_frame_mask", "res/gra/ui/boss_frame_mask.png");
			gplib::draw::LoadObject("boss_bar_mask", "res/gra/ui/boss_bar_mask.png");
		}
		//Event
		{
			gplib::draw::LoadObject("dialog_box", "res/gra/event/Dialogue_Box.png");
		//	gplib::draw::LoadObject("button", "res/gra/event/Button.png");
		}
		//Tutorial
		{
			//移動
			gplib::draw::LoadObject("tutorial_movement_jp", "res/gra/tutorial/movement_jp.png");
			gplib::draw::LoadObject("tutorial_movement_en", "res/gra/tutorial/movement_en.png");
			//攻撃
			gplib::draw::LoadObject("tutorial_melee_atk_jp", "res/gra/tutorial/melee_atk_jp.png");
			gplib::draw::LoadObject("tutorial_melee_atk_en", "res/gra/tutorial/melee_atk_en.png");
			//魔法攻撃
			gplib::draw::LoadObject("tutorial_matk_jp", "res/gra/tutorial/matk_jp.png");
			gplib::draw::LoadObject("tutorial_matk_en", "res/gra/tutorial/matk_en.png");
			//ジャンプ
			gplib::draw::LoadObject("tutorial_jump_jp", "res/gra/tutorial/jump_jp.png");
			gplib::draw::LoadObject("tutorial_jump_en", "res/gra/tutorial/jump_en.png");
			//飛ぶモード
			gplib::draw::LoadObject("tutorial_fly_jp", "res/gra/tutorial/fly_jp.png");
			gplib::draw::LoadObject("tutorial_fly_en", "res/gra/tutorial/fly_en.png");
			//吹き飛ぶ
			gplib::draw::LoadObject("tutorial_enemy_jp", "res/gra/tutorial/enemy_jp.png");
			gplib::draw::LoadObject("tutorial_enemy_en", "res/gra/tutorial/enemy_en.png");
		}

		//BGM SE
		{
			gplib::se::LoadFile("onground", "res/snd/landing.wav");
			gplib::se::LoadFile("shot", "res/snd/shot.wav");
			gplib::se::LoadFile("jump", "res/snd/jump.wav");
			gplib::se::LoadFile("flychange", "res/snd/change1.wav");
			gplib::se::LoadFile("damage", "res/snd/damage.wav");
			gplib::se::LoadFile("hit", "res/snd/shot_hit.wav");
			gplib::se::LoadFile("bound", "res/snd/bound.wav");
			gplib::se::LoadFile("down", "res/snd/down.wav");
			gplib::se::LoadFile("slimejump", "res/snd/puyon1.wav");
			gplib::se::LoadFile("bosswalk", "res/snd/bosswalk.wav");
			gplib::se::LoadFile("boss_shot", "res/snd/magic_worp.wav");
			gplib::se::LoadFile("appear", "res/snd/appear.wav");
			gplib::se::LoadFile("ice", "res/snd/magic_ice.wav");
			gplib::se::LoadFile("arrow", "res/snd/arrow.wav");
			gplib::bgm::LoadFile("stagebgm", "res/snd/stagebgm.mp3");
			gplib::bgm::LoadFile("bossbgm", "res/snd/boss.mp3");
		}

	}

	void Loading::loadResGameOver()
	{
		gplib::draw::LoadObject("effect_gameover", "res/gra/effect/gameOver.png");
	}

	void Loading::loadResEnd()
	{
		gplib::draw::LoadObject("endText1", "res/gra/end/endText1.png");
		gplib::draw::LoadObject("endText2", "res/gra/end/endText2.png");
		gplib::draw::LoadObject("programmer1", "res/gra/end/programmer1.png");
		gplib::draw::LoadObject("programmer2", "res/gra/end/programmer2.png");
		gplib::draw::LoadObject("programmer3", "res/gra/end/programmer3.png");
		gplib::draw::LoadObject("programmer4", "res/gra/end/programmer4.png");
		gplib::draw::LoadObject("designer1", "res/gra/end/designer1.png");
		gplib::draw::LoadObject("designer2", "res/gra/end/designer2.png");
	}

	void Loading::releaseResource()
	{
		//Images
		gplib::draw::DeleteAll();
		//BGM
		gplib::bgm::DeleteAll();
		//SE
		gplib::se::DeleteAll();
	}

}