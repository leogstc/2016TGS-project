#include "player.h"
#include "../config.h"
#include "../stage/map.h" //マップ名を取得ため
#include "shot.h"
#include "../event/stageEventArea.hpp"
#include "../scene/loading.h"
//Debug
#include "../effect/effectManager.h"

//Record
#include <fstream>
#include <sstream>



//FLY_ATK Loop
//#define PLAYER_FLY_ATK_LOOP 1
//#define PLAYER_FLY_ATK_DISTANCE (300 * config.GAME_SCALE)
//#define PLAYER_FLY_ATK_END_LOOP 5
//#define PLAYER_FLY_ATK_END_DISTANCE (100 * config.GAME_SCALE)


//無敵
#define PLAYER_INVINCIBLE_CHANGE_STATE 60
#define PLAYER_INVINCIBLE_TIME 180


/*
//-------------------- 重要！ --------------------
update系()
自動的に状態変更は手動状態変更の前に入れる
そうしないと、1/60の可能性にバグが出る
*/

namespace player {
	Player::Player(int x, int y, DIR dir, bool isDemo)
		:
		MapCollisionObj(
			//ObjectName
			"player",
			//Body
			ci_ext::DrawObjf(
				ci_ext::Vec3f(x, y, 0.7f),	//pos
				ci_ext::Vec2f::one(),	//scale
				"Tplayer",				//resname
				//"res/gra/player.png",	//filename
				ci_ext::Vec2f::zero(),	//src
				ci_ext::Vec2f::zero(),	//src size
				ci_ext::Color(255, 255, 255, 255),	//color
				0.0f					//degree
			),
			//Velocity
			ci_ext::Vec3f::zero(),
			//isPlayer
			true
		),
		state_(State::STAND),
		dir_(dir),
		controlable_(true),
		cnt_(0),
		shotPow_(3.f),
		isDemo_(isDemo)
	{}

	Player::~Player()
	{}


#ifdef RECORD_DEMO
	void Player::saveRec()
	{
		std::ofstream file("res/rec.txt");
		if (file.is_open()) {
			std::stringstream ss;
			for (auto& rec : recordBtn) {
				ss << rec << std::endl;
			}
			file << ss.str();
			file.close();
		}
		else {
			assert("Error file open");
		}
	}
#else
	void Player::loadRec()
	{
		std::ifstream file("res/rec.txt");
		if (file.is_open()) {
			readRec.clear();

			while (!file.eof()) {
				std::string line;
				std::getline(file, line);

				readRec.push_back((WORD)atoi(line.c_str()));
			}

			file.close();
		}
		else {
			assert("Error file open");
		}
	}
#endif

	void Player::init()
	{
		if (isDemo_ == true) {
			mover_ = std::make_shared<AutoMover>();
			keyMover_ = std::make_shared<KeyMover>();
#ifndef RECORD_DEMO
			loadRec();
#endif
		}
		else {
			mover_ = std::make_shared<KeyMover>();
		}

		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}
		info_ = ci_ext::weak_cast<PlayerInfo>(getObjectFromRoot("player_info"));
		//info_.lock()->resetLife(); //現在のライフで立てたフラグをリセット

		body_.setSrcSize(ci_ext::Vec2f(sprite::SPRITE_SIZE, sprite::SPRITE_SIZE));
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE, config.GAME_SCALE));

		//アニメーション
		anim_.setAnimReset(6, 6);

		//Adjust Y Position when create character
		setObjCollisionMapRect(getBodyCollisionRect());
		mapCheckCollisionY(true);

		deadcnt = 0;
		//Debug
		//body_.setPos(ci_ext::Vec3f(350, 400));
		//body_.setPos(ci_ext::Vec3f(80 * config.GAME_SCALE, 100 * config.GAME_SCALE));
		//body_.setPos(ci_ext::Vec3f(300 * config.GAME_SCALE, -800 * config.GAME_SCALE)); //posy=-20
		//body_.setPos(ci_ext::Vec3f(300 * config.GAME_SCALE, 400 * config.GAME_SCALE, body_.pos().z())); //posy=2
		//setState(State::STAND);
		isShot_ = false;


#ifdef RECORD_DEMO
		recordBtn.clear();
#endif
	}

	

	void Player::render()
	{
		if ((invincibleCnt_ % 2) == 0)
			body_.render();

#ifdef _DEBUG
		//-------------------- Debug --------------------
		{
			//ダメージを受ける・マップに対して判定
			auto rt = getBodyCollisionRect();
			gplib::draw::CkRect(rt);
			gplib::font::TextNC(250, 20, 0.0f, "l:" + std::to_string(rt.left), ARGB(255, 255, 255, 0), 0);
			gplib::font::TextNC(250, 40, 0.0f, "t:" + std::to_string(rt.top), ARGB(255, 255, 255, 0), 0);
			gplib::font::TextNC(250, 60, 0.0f, "r:" + std::to_string(rt.right), ARGB(255, 255, 255, 0), 0);
			gplib::font::TextNC(250, 80, 0.0f, "b:" + std::to_string(rt.bottom), ARGB(255, 255, 255, 0), 0);

			//攻撃判定
			//auto atkRt = getAtkCollision();
			//if (!isRectValueEqual(rt, -1))
			//	gplib::draw::CkRect(atkRt, ARGB(255, 255, 255, 0));

			//状態
			std::string s;
			switch (state_) {
			default: s = "Unknown"; break;
			case State::STAND: s = "Stand"; break;
			case State::WALK: s = "Walk"; break;
			case State::JUMP: s = "Jump"; break;
			case State::FALL: s = "Fall"; break;
			//case State::ATK_C: s = "Atk C"; break;
			case State::ATK_M: s = "Atk M"; break;
			//case State::START_FLY: s = "Start Fly"; break;
			case State::FLY: s = "Fly"; break;
			//case State::FLY_ATK: s = "Fly Atk"; break;
			//case State::FLY_ATK_END: s = "Fly Atk End (Break)"; break;
			case State::HIT: s = "Hit"; break;
			}
			gplib::font::TextNC(20, 20, 0.0f, s, ARGB(255, 255, 255, 255), 0);

			//座標
			gplib::font::TextNC(20, 40, 0.0f, std::to_string(body_.pos().x()) + "," + std::to_string(body_.pos().y()), ARGB(255, 255, 255, 255), 0);
			//現在にいるマップ名
			gplib::font::TextNC(20, 60, 0.0f, "Map:" + nowInMapName_, ARGB(255, 255, 0, 0), 0);
			//無敵フラグ
			gplib::font::TextNC(350, 20, 0.0f, "Invincible:" + std::to_string(invincible_), ARGB(255, 255, 0, 0), 0);\
		}
		//-------------------- End Debug --------------------
#endif
	}

	void player::Player::resume()
	{
		//getParentPtr().lock()->getParentPtr().lock()->insertAsChild(new scene::GameOver());
		getParentPtr().lock()->getParentPtr().lock()->insertAsChild(new scene::Loading(scene::Scene::GAME_OVER));
		getParentPtr().lock()->getParentPtr().lock()->kill();
	}

	void Player::update()
	{
#ifndef RECORD_DEMO
		if (isDemo_ == true) {
			if (readRec.size() > 0) {
				mover_->CheckKey(readRec.front());
				readRec.erase(readRec.begin());

				keyMover_->CheckKey();
				if (keyMover_->CheckAnyPress()) {
					auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));	
					if (!map.expired())
						map.lock()->nextScene();					
					kill();
				}
			}
			else {
				auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));		
				if (!map.expired())
					map.lock()->nextScene();
				kill();
			}
		}
		else {
			mover_->CheckKey();
		}
#else
		mover_->CheckKey();

		recordBtn.push_back(mover_->GetData());

		if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
			saveRec();
		}
#endif



		//Debug
		//if (gplib::input::CheckPush(gplib::input::KEY_F2)) {
		//	auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
		//	if (!effMngr.expired()) {
		//		effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
		//	}
		//}

		//auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		auto map = ci_ext::weak_cast<map::Map>(getParentPtr());


		//Debug
		//if (!map.lock()->isChangingMap() && controlable_) {
		//	if (gplib::input::CheckPush(gplib::input::KEY_SPACE)) {
		//		initDead();
		//	}
		//}
		//----- End Debug
		

		//次のマップへ移動中
		if (map.lock()->isChangingMap()) {
			//アニメーションしない
			if (state_ == State::JUMP || state_ == State::FLY) {
				return;
			}
		}
		//----- 以降　次のマップへ移動していない


		//アニメーション
		anim_.step();

		//状態のアニメーション画像
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), (int)state_ * sprite::SPRITE_SIZE));

		//現在のマップ名を取得
		nowInMapName_ = map.lock()->getNowMapName();
		
		//無敵状態
		if (invincible_) {
			invincibleCnt_++;
			if (invincibleCnt_ >= PLAYER_INVINCIBLE_TIME) {
				invincibleCnt_ = 0;
				invincible_ = false; //普通状態へ
			}
		}
		
		//プレイヤーの状態によりあたり判定を生成
		setObjCollisionMapRect(getSpriteMapCollisionRect());
		//プレイヤーの状態により処理を選択
		switch (state_) {
		case State::STAND: updateStand(); break;
		case State::WALK: updateWalk(); break;
		case State::JUMP: updateJump(); break;
		case State::FALL: updateFall(); break;
		//case State::ATK_C: updateAtkC(); break;
		case State::ATK_M: updateAtkM(); break;
		//case State::START_FLY: updateStartFly(); break;
		case State::FLY: updateFly(); break;
		//case State::FLY_ATK: updateFlyAtk(); break;
		//case State::FLY_ATK_END: updateFlyAtkEnd(); break;
		case State::HIT: updateHit(); break;
		case State::DEAD: updateDead(); break;
		}

		//Map Collision Check
		mapCheckMoveXY();

		//画像の回転
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE, body_.scale().y()));

	}

	void Player::updateStand()
	{
		gplib::se::EndCheck();
		if (controlable_) {
			//X動き
			MoveKeyX();

			//動きあり
			if (velocity_.x() != 0) {
				setState(State::WALK);
			}

			//If Event trigger on -> can't jump
			bool jumpOk = true;

			//Check Event Triggers
			auto eventArea = getObjectsFromRoot({ "event_area_" }, { "auto_" });
			for (auto& eArea : eventArea) {
				auto& e = ci_ext::weak_cast<gameevent::EventArea>(eArea);
				if (e.lock()->checkCollision(getBodyCollisionRect())) {
					if (mover_->CheckPush(Mover::Key::A)/* ||gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
						e.lock()->createEvent();
						jumpOk = false;
						break;
					}
				}
			}

			if (jumpOk) {
				if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
					setState(State::JUMP);
					velocity_.y(-config.PLAYER_JUMP_POW);
					gplib::se::Play("jump");
				}
			}
			//if (mover_->CheckPush(Mover::Key::Y)/*gplib::input::CheckPush(gplib::input::KEY_BTN1)*/) {
			//	setState(State::ATK_C);
			//}
			if (mover_->CheckPush(Mover::Key::X)/*gplib::input::CheckPush(gplib::input::KEY_BTN2) && info_.lock()->getMp() >= config.ATK_M_MP_USE*/) {
				setState(State::ATK_M);
			}
		}
		
		//重力
		velocity_.offset(0, config.GRAVITY);

		//床をチェック
		if (!checkUnderFloor()) {
			setState(State::FALL);
		}

		isShot_ = false;
	}

	void Player::updateWalk()
	{
		if (controlable_) {
			//X動き
			MoveKeyX();

			//動きなし
			if (velocity_.x() == 0) {
				setState(State::STAND);
			}

			//重力
			velocity_.offset(0, config.GRAVITY);

			//床をチェック
			if(!checkUnderFloor()) {
				checkUnderFloor();
				setState(State::FALL);
			}


			//If Event trigger on -> can't jump
			bool jumpOk = true;

			//Check Event Triggers
			auto eventArea = getObjectsFromRoot({ "event_area_" }, { "auto_" });
			for (auto& eArea : eventArea) {
				auto& e = ci_ext::weak_cast<gameevent::EventArea>(eArea);
				if (e.lock()->checkCollision(getBodyCollisionRect())) {
					if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
						e.lock()->createEvent();
						jumpOk = false;
						break;
					}
				}
			}

			if (jumpOk) {
				if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
					setState(State::JUMP);
					velocity_.y(-config.PLAYER_JUMP_POW);
					gplib::se::Play("jump");
				}
			}
			//if (mover_->CheckPush(Mover::Key::Y)/*gplib::input::CheckPush(gplib::input::KEY_BTN1)*/) {
			//	setState(State::ATK_C);
			//}
			if (mover_->CheckPush(Mover::Key::X)/*gplib::input::CheckPush(gplib::input::KEY_BTN2) && info_.lock()->getMp() >= config.ATK_M_MP_USE */) {
				setState(State::ATK_M);
			}
		}
		else {
			//重力
			velocity_.offset(0, config.GRAVITY);

			//床をチェック
			if(!checkUnderFloor()) {
				setState(State::FALL);
			}

		}
	}

	void Player::updateJump()
	{
		if (controlable_) {
			//X動き
			MoveKeyX();
		}

		//重力
		velocity_.offset(0, config.GRAVITY);

		//Reach top of jump power and start to fall
		if (velocity_.y() > 0) {
			setState(State::FALL);
		}

		//このフレームでにジャンプボタンを押す
		if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
			velocity_.y(0);
			setState(State::FLY);
			gplib::se::Play("flychange");
		}
		//元のフレームからジャンプボタンはまだ押されている
		else if (mover_->CheckPress(Mover::Key::A)/*gplib::input::CheckPress(gplib::input::KEY_BTN0)*/) {
			if (velocity_.y() < 0) {
				velocity_.offset(0, -config.PLAYER_JUMP_INC);
			}
		}

		//アニメーション
		if (anim_.getAnimNo() == sprite::ANIM_JUMP - 1) {
			anim_.pause();
		}
	}

	void Player::updateFall()
	{
		//If changing map by passing through door -> No Gravity
		auto map = ci_ext::weak_cast<map::Map>(getParentPtr());
		if (!map.lock()->isChangingMap()) {
			//落下
			velocity_.offset(0, config.GRAVITY);
		}

		if (controlable_) {
			//X動き
			MoveKeyX();

			//先に確認
			if (checkUnderFloor()) {
				//if (anim_.isPause())
				////着地アニメーション
				//	anim_.resume();
				//
				//if (anim_.isLastFrame()) {
				//if (!invincible_) //<-- ダメージを受けている状態に状態変更しないよう　この条件を追加
				setState(State::STAND);
				gplib::se::Play("onground");
				//}
			}

			//後に確認
			if (anim_.isPause()) {
				if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
					velocity_.y(0);
					setState(State::FLY);
					gplib::se::Play("flychange");
				}
			}
			//if (anim_.isPlay()) {
			//else {
			//	if (gplib::input::CheckPush(gplib::input::KEY_BTN1)) {
			//		setState(State::ATK_C);
			//	}
			//}
		}
		else {
			if (checkUnderFloor()) {
				setState(State::STAND);
				gplib::se::Play("onground");
			}
		}
		isShot_ = false;

		auto create_timer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
		if (create_timer != nullptr)
		{
			if (create_timer->isRunning())
			{
				create_timer_.lock()->kill();
			}
		}
	}

	//void Player::updateStartFly()
	//{
	//	if (controlable_) {
	//		//Flyをキャンセル
	//		if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
	//			setState(State::FALL);
	//		}

	//		//アニメーション
	//		if (anim_.getAnimNo() == sprite::ANIM_START_FLY - 1) {
	//			setState(State::FLY);
	//		}
	//	}
	//}

	void Player::updateFly()
	{
		if (controlable_) {
			//XY動き
			MoveKeyX();
			MoveKeyY();

			auto create_timer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
			if (create_timer != nullptr)
			{
				if (create_timer->isPause())
				{
					create_timer->run();
				}
				if (create_timer->isTimeOver())
				{
					isShot_ = false;
					create_timer_.lock()->kill();
				}
			}

			if (mover_->CheckPush(Mover::Key::A)/*gplib::input::CheckPush(gplib::input::KEY_BTN0)*/) {
				velocity_.y(0);
				setState(State::FALL);

			}

			//攻撃
			if (mover_->CheckPush(Mover::Key::X)/*gplib::input::CheckPush(gplib::input::KEY_BTN1)*/) {
				if (!isShot_)
				{
					create_timer_ = insertAsChild(new game::Timer("generator_timer", 0.5f));
					isShot_ = true;
					insertAsChild(new shot::PlayerShot(shotPow_, "nomal"));
					insertAsChild(new shot::PlayerShot(shotPow_, "up"));
					insertAsChild(new shot::PlayerShot(shotPow_, "down"));
					gplib::se::Play("shot");
				}
				
			}
		}
	}

	//void Player::updateFlyAtk()
	//{
	//	if (anim_.isFirstFrameStep() && cnt_ == 0) {
	//		//Do in this frame to debug map collision when state change

	//		//Animation Frame * Animation Frame Spd * Loop
	//		float frame = sprite::ANIM_FLY_ATK * sprite::ANIM_SPD_FLY_ATK * PLAYER_FLY_ATK_LOOP;
	//		float v = PLAYER_FLY_ATK_DISTANCE / frame;
	//		velocity_.x((int)dir_ * v);
	//		velocity_.y(0);
	//	}

	//	if (anim_.isLastFrame()) {
	//		cnt_++;

	//		if (cnt_ >= PLAYER_FLY_ATK_LOOP) {
	//			cnt_ = 0;

	//			//Animation Frame * Animation Frame Spd * Loop
	//			float frame = sprite::ANIM_FLY_ATK_END * sprite::ANIM_SPD_FLY_ATK_END * PLAYER_FLY_ATK_END_LOOP;
	//			float v = PLAYER_FLY_ATK_END_DISTANCE / frame;
	//			velocity_.x((int)dir_ * v);
	//			velocity_.y(0);
	//			setState(State::FLY_ATK_END);
	//		}
	//	}
	//}

	//void Player::updateFlyAtkEnd()
	//{
	//	if (anim_.isLastFrame()) {
	//		cnt_++;

	//		if (cnt_ >= PLAYER_FLY_ATK_END_LOOP) {
	//			velocity_.x(0);
	//			velocity_.y(0);
	//			setState(State::FLY);
	//		}
	//	}
	//}

	//void Player::updateAtkC()
	//{
	//	velocity_.x(0);

	//	if (anim_.isLastFrame()) {
	//		setState(State::STAND);
	//	}
	//}

	void Player::updateAtkM()
	{
		if (anim_.isFirstFrameNo(3)) {
			//Create Shot 魔法弾
			insertAsChild(new shot::PlayerShot(shotPow_, "nomal"));
			insertAsChild(new shot::PlayerShot(shotPow_, "up"));
			insertAsChild(new shot::PlayerShot(shotPow_, "down"));	
			gplib::se::Play("shot");
		}

		velocity_.x(0);

		if (anim_.isLastFrame()) {
			setState(State::STAND);
		}
	}

	void Player::updateHit()
	{
		
		//落下
		velocity_.offset(0, config.GRAVITY);

		//一定時間通過したら、普通状態へ
		if (invincibleCnt_ >= PLAYER_INVINCIBLE_CHANGE_STATE) {
			setState(State::STAND);
		}

		//Debug
		//if (gplib::input::CheckPush(gplib::input::KEY_BTN0)) {
		//	invincibleCnt_ = 0;
		//	invincible_ = false;
		//	setState(State::STAND);
		//}
	}

	void Player::updateDead() 
	{
		velocity_.offset(0, config.GRAVITY);
		auto map = ci_ext::weak_cast<map::Map>(getParentPtr());

		//時間をずらすための処理
		if (anim_.isFirstFrameNo(4))
		{
			anim_.pause();
			if (checkUnderFloor())
			{
				auto create_timer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
				if (create_timer != nullptr)
				{
					if (create_timer->isPause())
					{
						create_timer->run();
					}
					if (create_timer->isTimeOver())
					{
						create_timer_.lock()->kill();
						if (info_.lock()->getLife() > 0)
						{
							kill();
							map.lock()->playerRebirth();
							return;
						}
						else if (info_.lock()->getLife() == 0)
						{
							gplib::bgm::Stop("stagebgm");

							auto efMana = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
							if (!efMana.expired())
								efMana.lock()->createEffect("fadeinblack", 0, 0);

							map.lock()->getParentPtr().lock()->sleep(45);
							//map.lock()->nextScene();
						}
					}
				}
			}
		}
	}

	void Player::doOnceHitByEnemy(float dmg)
	{
		info_.lock()->offsetHp(-dmg);
		if (info_.lock()->getHp() > 0.0f)
		{
			if (dir_ == DIR::LEFT)
				velocity_.x(config.PLAYER_HIT_X_SPD);
			else// if (dir_ == DIR::RIGHT)
				velocity_.x(-config.PLAYER_HIT_X_SPD);

			velocity_.y(-config.PLAYER_HIT_Y_SPD);
			gplib::se::Play("damage");
			setState(State::HIT);
			invincible_ = true;
			invincibleCnt_ = 0;
		}
	}

	void Player::doOnceHitDamageTile()
	{
		initDead();
	}

	void Player::doOnceHitDeadTile()
	{
		initDead();
	}

	void Player::initDead()
	{
		//Dead処理
		info_.lock()->offsetLife(-1);
		velocity_.x(0);
		invincible_ = true;
		create_timer_ = insertAsChild(new game::Timer("generator_timer", 1.f));
		setDeadState(State::DEAD);
		gplib::se::Play("down");
	}

	void Player::MoveKeyX()
	{
		velocity_.x(0);
		//動き
		if (mover_->CheckPress(Mover::Key::LEFT) || mover_->LeftAnalogX() < -0.6/*gplib::input::CheckPress(gplib::input::KEY_LEFT)*/)
		{
			//方向セット
			dir_ = DIR::LEFT;
			//Adjust
			mapCheckCollisionX(true);

			//移動量
			if (state_ == State::FLY) velocity_.x(-config.PLAYER_FLY_SPD);
			else if (state_ == State::JUMP || state_ == State::FALL) velocity_.x(-config.PLAYER_JUMP_MOVEX_SPD);
			else velocity_.x(-config.PLAYER_MOVE_SPD);

			//移動によってStateを変える
			//if (state != State::NONE)
			//	setState(state);
		}
		else if (mover_->CheckPress(Mover::Key::RIGHT) || mover_->LeftAnalogX() > 0.6/*gplib::input::CheckPress(gplib::input::KEY_RIGHT)*/)
		{
			//方向セット
			dir_ = DIR::RIGHT;
			//Adjust
			mapCheckCollisionX(true);

			//移動量
			if (state_ == State::FLY) velocity_.x(+config.PLAYER_FLY_SPD);
			else if (state_ == State::JUMP || state_ == State::FALL) velocity_.x(+config.PLAYER_JUMP_MOVEX_SPD);
			else velocity_.x(+config.PLAYER_MOVE_SPD);

			//移動によってStateを変える
			//if (state != State::NONE)
			//	setState(state);
		}
	}

	void Player::MoveKeyY()
	{
		velocity_.y(0);
		//動き
		if (mover_->CheckPress(Mover::Key::UP) || mover_->LeftAnalogY() > 0.6)/*gplib::input::CheckPress(gplib::input::KEY_UP)*/
		{
			if (state_ == State::FLY) velocity_.y(-config.PLAYER_FLY_SPD);
			else velocity_.y(-config.PLAYER_MOVE_SPD);
		}
		else if (mover_->CheckPress(Mover::Key::DOWN) || mover_->LeftAnalogY() < -0.6)/*gplib::input::CheckPress(gplib::input::KEY_DOWN)*/
		{
			if (state_ == State::FLY) velocity_.y(+config.PLAYER_FLY_SPD);
			else velocity_.y(+config.PLAYER_MOVE_SPD);
		}
	}

	void Player::setState(State state)
	{
		//突然判定RECTが変更したら -> Adjust
		{
			bool bad = false;
			//引数 stateは「次のstate」;
			//classが持っているstate_は「現在のstate」

			//New State Check Rect
			int nowAnimNo_ = anim_.getAnimNo();
			anim_.resetNo();
			setObjCollisionMapRect(getSpriteMapCollisionRect(state));


			if (state_ == State::FLY && (state == State::FALL || state == State::FLY_ATK)) {
				if (mapCheckCollisionY(true)) {
#ifdef _DEBUG
					gplib::font::TextNC(20, 80, 0.0f, "Error Change State Collision Y", ARGB(255, 255, 0, 0), 0);
#endif
					bad = true;
				}
			}
			/*else if (state == State::START_FLY) {
				if (mapCheckCollisionX(true)) {
					gplib::font::TextNC(20, 80, 0.0f, "SX", ARGB(255, 255, 0, 0), 0);
					bad = true;
				}
			}*/
			else {
				if (mapCheckCollisionX(true)) {
#ifdef _DEBUG
					gplib::font::TextNC(20, 80, 0.0f, "Error Change State Collision X", ARGB(255, 255, 0, 0), 0);
#endif
					bad = true;
				}
			}

			//Both side are collision
			if (bad) {
				//Back to Old State
				anim_.setAnimNo(nowAnimNo_);
				setObjCollisionMapRect(getSpriteMapCollisionRect(state_));
				return;
			}
		}

		//Change state OK
		setStateForce(state);
	}

	void Player::setStateForce(State state)
	{
		//Change state
		state_ = state;
		
		//アニメーション設定
		int spd = 6;
		switch (state_) {
		case State::STAND: anim_.setAnimReset(sprite::ANIM_STAND, sprite::ANIM_SPD_STAND); break;
		case State::WALK: anim_.setAnimReset(sprite::ANIM_WALK, sprite::ANIM_SPD_WALK); break;
		case State::JUMP: anim_.setAnimReset(sprite::ANIM_JUMP, sprite::ANIM_SPD_JUMP); break;
		case State::FALL: {
			anim_.setAnimReset(sprite::ANIM_FALL, sprite::ANIM_SPD_FALL);
			anim_.pause();
			break;
		}
		//case State::ATK_C: anim_.setAnimReset(sprite::ANIM_ATK_C, sprite::ANIM_SPD_ATK_C); break;
		case State::ATK_M: anim_.setAnimReset(sprite::ANIM_ATK_M, sprite::ANIM_SPD_ATK_M); break;
		//case State::START_FLY: anim_.setAnimReset(sprite::ANIM_START_FLY, sprite::ANIM_SPD_START_FLY); break;
		case State::FLY: anim_.setAnimReset(sprite::ANIM_FLY, sprite::ANIM_SPD_FLY); break;
		//case State::FLY_ATK: anim_.setAnimReset(sprite::ANIM_FLY_ATK, sprite::ANIM_SPD_FLY_ATK); break;
		//case State::FLY_ATK_END: anim_.setAnimReset(sprite::ANIM_FLY_ATK_END, sprite::ANIM_SPD_FLY_ATK_END); break;
		case State::HIT: anim_.setAnimReset(sprite::ANIM_HIT, sprite::ANIM_SPD_HIT); break;
		case State::DEAD:anim_.setAnimReset(sprite::ANIM_DEAD, sprite::ANIM_SPD_DEAD); break;
		}
	}

	RECT Player::getBodyOffset(State state) {
		RECT rt = { 0, 0, 0, 0 };

		if (state == State::NONE)
			state = state_;

		switch (state) {
		default:
		case State::STAND:
		case State::WALK:
			rt = {
				sprite::OFFSET_STAND_LEFT,
				sprite::OFFSET_STAND_TOP,
				sprite::OFFSET_STAND_RIGHT,
				sprite::OFFSET_STAND_BOTTOM
			};
			break;
		case State::JUMP: {
			switch (anim_.getAnimNo()) {
			default:
			case 0:
				rt = {
					sprite::OFFSET_JUMP_LEFT1,
					sprite::OFFSET_JUMP_TOP1,
					sprite::OFFSET_JUMP_RIGHT1,
					sprite::OFFSET_JUMP_BOTTOM1
				};
				break;
			case 1:
				rt = {
					sprite::OFFSET_JUMP_LEFT2,
					sprite::OFFSET_JUMP_TOP2,
					sprite::OFFSET_JUMP_RIGHT2,
					sprite::OFFSET_JUMP_BOTTOM2
				};
				break;
			case 2:
				rt = {
					sprite::OFFSET_JUMP_LEFT3,
					sprite::OFFSET_JUMP_TOP3,
					sprite::OFFSET_JUMP_RIGHT3,
					sprite::OFFSET_JUMP_BOTTOM3
				};
				break;
			case 3:
				rt = {
					sprite::OFFSET_JUMP_LEFT4,
					sprite::OFFSET_JUMP_TOP4,
					sprite::OFFSET_JUMP_RIGHT4,
					sprite::OFFSET_JUMP_BOTTOM4
				};
				break;
			case 4:
				rt = {
					sprite::OFFSET_JUMP_LEFT5,
					sprite::OFFSET_JUMP_TOP5,
					sprite::OFFSET_JUMP_RIGHT5,
					sprite::OFFSET_JUMP_BOTTOM5
				};
				break;
			}
			break;
		} //----- End JUMP
		case State::FALL: 
			rt = {
				sprite::OFFSET_FALL_LEFT,
				sprite::OFFSET_FALL_TOP,
				sprite::OFFSET_FALL_RIGHT,
				sprite::OFFSET_FALL_BOTTOM
			};
			break;
		 //----- End FALL
		//case State::ATK_C: 
		//{
		//	switch (anim_.getAnimNo()) {
		//	default:
		//	case 0:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT1,
		//			sprite::OFFSET_ATK_C_TOP1,
		//			sprite::OFFSET_ATK_C_RIGHT1,
		//			sprite::OFFSET_ATK_C_BOTTOM1
		//		};
		//		break;
		//	case 1:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT2,
		//			sprite::OFFSET_ATK_C_TOP2,
		//			sprite::OFFSET_ATK_C_RIGHT2,
		//			sprite::OFFSET_ATK_C_BOTTOM2
		//		};
		//		break;
		//	case 2:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT3,
		//			sprite::OFFSET_ATK_C_TOP3,
		//			sprite::OFFSET_ATK_C_RIGHT3,
		//			sprite::OFFSET_ATK_C_BOTTOM3
		//		};
		//		break;
		//	case 3:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT4,
		//			sprite::OFFSET_ATK_C_TOP4,
		//			sprite::OFFSET_ATK_C_RIGHT4,
		//			sprite::OFFSET_ATK_C_BOTTOM4
		//		};
		//		break;
		//	case 4:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT5,
		//			sprite::OFFSET_ATK_C_TOP5,
		//			sprite::OFFSET_ATK_C_RIGHT5,
		//			sprite::OFFSET_ATK_C_BOTTOM5
		//		};
		//		break;
		//	case 5:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT6,
		//			sprite::OFFSET_ATK_C_TOP6,
		//			sprite::OFFSET_ATK_C_RIGHT6,
		//			sprite::OFFSET_ATK_C_BOTTOM6
		//		};
		//		break;
		//	case 6:
		//		rt = {
		//			sprite::OFFSET_ATK_C_LEFT7,
		//			sprite::OFFSET_ATK_C_TOP7,
		//			sprite::OFFSET_ATK_C_RIGHT7,
		//			sprite::OFFSET_ATK_C_BOTTOM7
		//		};
		//		break;
		//	}
		//	break;
		//} //----- End ATK_C
		//case State::START_FLY:
		case State::FLY:
			rt = {
				sprite::OFFSET_FLY_LEFT,
				sprite::OFFSET_FLY_TOP,
				sprite::OFFSET_FLY_RIGHT,
				sprite::OFFSET_FLY_BOTTOM
			};
			break;
		//case State::FLY_ATK:
		//	rt = {
		//		sprite::OFFSET_FLY_ATK_LEFT,
		//		sprite::OFFSET_FLY_ATK_TOP,
		//		sprite::OFFSET_FLY_ATK_RIGHT,
		//		sprite::OFFSET_FLY_ATK_BOTTOM
		//	};
		//	break;
		//case State::FLY_ATK_END:
		//	rt = {
		//		sprite::OFFSET_FLY_ATK_END_LEFT,
		//		sprite::OFFSET_FLY_ATK_END_TOP,
		//		sprite::OFFSET_FLY_ATK_END_RIGHT,
		//		sprite::OFFSET_FLY_ATK_END_BOTTOM
		//	};
		//	break;
		case State::HIT://Debug
			rt = {
				sprite::OFFSET_FALL_LEFT,//Debug
				sprite::OFFSET_FALL_TOP,//Debug
				sprite::OFFSET_FALL_RIGHT,//Debug
				sprite::OFFSET_FALL_BOTTOM//Debug
			};
			break;
		case State::DEAD:
			rt = {
				sprite::OFFSET_DEAD_LEFT,
				sprite::OFFSET_DEAD_TOP,
				sprite::OFFSET_DEAD_RIGHT,
				sprite::OFFSET_DEAD_BOTTOM
			};
		}
		//----- End Switch State -----

		//Finally -> 方向により値を入れ替える
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT Player::getSpriteMapOffset(State state)
	{
		RECT rt = { 0, 0, 0, 0 };

		if (state == State::NONE)
			state = state_;

		switch (state) {
		default:
		case State::STAND:
		case State::WALK:
			rt = {
				sprite::MAP_COL_OFFSET_STAND_LEFT,
				sprite::MAP_COL_OFFSET_STAND_TOP,
				sprite::MAP_COL_OFFSET_STAND_RIGHT,
				sprite::MAP_COL_OFFSET_STAND_BOTTOM
			};
			break;
		case State::JUMP:
			rt = {
				sprite::MAP_COL_OFFSET_JUMP_LEFT,
				sprite::MAP_COL_OFFSET_JUMP_TOP,
				sprite::MAP_COL_OFFSET_JUMP_RIGHT,
				sprite::MAP_COL_OFFSET_JUMP_BOTTOM
			};
			break;
		case State::FALL:
			rt = {
				sprite::MAP_COL_OFFSET_FALL_LEFT,
				sprite::MAP_COL_OFFSET_FALL_TOP,
				sprite::MAP_COL_OFFSET_FALL_RIGHT,
				sprite::MAP_COL_OFFSET_FALL_BOTTOM
			};
			break;
		//case State::START_FLY:
		case State::FLY:
			rt = {
				sprite::MAP_COL_OFFSET_FLY_LEFT,
				sprite::MAP_COL_OFFSET_FLY_TOP,
				sprite::MAP_COL_OFFSET_FLY_RIGHT,
				sprite::MAP_COL_OFFSET_FLY_BOTTOM
			};
			break;
		//case State::FLY_ATK:
		//	rt = {
		//		sprite::MAP_COL_OFFSET_FLY_ATK_LEFT,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_TOP,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_RIGHT,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_BOTTOM
		//	};
		//	break;
		//case State::FLY_ATK_END:
		//	rt = {
		//		sprite::MAP_COL_OFFSET_FLY_ATK_END_LEFT,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_END_TOP,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_END_RIGHT,
		//		sprite::MAP_COL_OFFSET_FLY_ATK_END_BOTTOM
		//	};
		//	break;
		case State::DEAD:
			rt = { 
				sprite::MAP_COL_OFFSET_DEAD_LEFT,
				sprite::MAP_COL_OFFSET_DEAD_TOP,
				sprite::MAP_COL_OFFSET_DEAD_RIGHT,
				sprite::MAP_COL_OFFSET_DEAD_BOTTOM
			};
			break;
		}
		//----- End Switch State -----

		//Finally -> 方向により値を入れ替える
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT Player::getBodyCollisionRect(State state)
	{
		RECT offset = getBodyOffset(state);
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

	RECT Player::getSpriteMapCollisionRect(State state)
	{
		RECT offset = getSpriteMapOffset(state);
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

	//RECT Player::getAtkCollision()
	//{
	//	RECT rt = { -1, -1, -1, -1 }; //Offset

	//	switch (state_) {
	//	case State::ATK_C: {
	//		if (((1 << anim_.getAnimNo()) & sprite::ATK_C_AVAILABLE) != 0) {
	//			rt = {
	//				72, 67, 0, 40
	//			};
	//		}
	//		break;
	//	}
	//	case State::FLY_ATK: {
	//		if (((1 << anim_.getAnimNo()) & sprite::FLY_ATK_AVAILABLE) != 0) {
	//			rt = {
	//				8, 34, 4, 8
	//			};
	//		}
	//		break;
	//	}
	//	} //----- End Switch State -----

	//	//Finally -> 方向により値を入れ替える
	//	swapLRbyScale(rt.left, rt.right, rt.left, rt.right);

	//	//No Collision
	//	if (isRectValueEqual(rt, -1))
	//		return RECT{ -1, -1, -1, -1 };

	//	//Has Collision
	//	return makeRectScale(rt.left, rt.top, rt.right, rt.bottom);
	//}

	ci_ext::Vec2f Player::getPos() const
	{
		return { body_.pos().x(), body_.pos().y() };
	}

	void Player::offsetPos(const ci_ext::Vec3f& offset)
	{
		body_.offsetPos(offset);
	}

	void Player::setVelocity(float x, float y)
	{
		velocity_.set(x, y);
	}

	DIR Player::getDir() const
	{
		return dir_;
	}

	State Player::getPlayerState() const
	{
		return state_;
	}

	void Player::setPlayerState(State state)
	{
		setState(state);
	}

	void Player::setPlayerStateJoint(State state)
	{
		setStateForce(state);

		if (state == State::STAND) {
			velocity_.x(0);
		}

		//外部から設定された場合は画像を切替しなければならない
		body_.setSrc(ci_ext::Vec2f(0, (int)state_ * sprite::SPRITE_SIZE));
	}

	void Player::setDeadState(State state)
	{
		state_ = state;
		anim_.setAnimReset(sprite::ANIM_DEAD, sprite::ANIM_SPD_DEAD);
	}

	bool Player::isInvincible() const
	{
		return invincible_;
	}

	void Player::setControlable(bool flag)
	{
		controlable_ = flag;
	}
	
}