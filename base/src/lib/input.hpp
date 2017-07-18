/*
Logicool Gamepad F310
written by Thossaphon Jungvisitthon

Updated 2016/08/04
Analog Pad can be detected
*In X-Mode it can't detect when both LT & RT pressed
*/

#pragma once
#include <Windows.h>
#include <Mmsystem.h> //JOYINFO
#include <vector>
#pragma comment(lib, "winmm.lib") //joyGetPosなどのライブラリー

//X-Mode を使用したい場合はコメントアウトを消す
//#define USE_JOY_X_MODE

//アナログパッドのHoldとFreeのフレームカウントを使用
//#define USE_ANALOG_HOLD_FREE

//HoldとFreeのフレームをデバッグする
#define DEBUG_HOLD_FREE

class Input {
	enum Setting {
		MAX_PLAYER = 1,
	};

public:
	enum class Key {
		BTN_0,
		BTN_1,
		BTN_2,
		BTN_3,
		BTN_L1,
		BTN_R1,
		BTN_L2,
		BTN_R2,
		BTN_L3,
		BTN_R3,
		BTN_BACK,
		BTN_START,
		UP,
		DOWN,
		LEFT,
		RIGHT,
#ifdef USE_ANALOG_HOLD_FREE
		ANALOG_L_UP,
		ANALOG_L_DOWN,
		ANALOG_L_LEFT,
		ANALOG_L_RIGHT,
		ANALOG_R_UP,
		ANALOG_R_DOWN,
		ANALOG_R_LEFT,
		ANALOG_R_RIGHT,
#endif
		KEY_MAX,
	};

	enum class P {
		P1,
		P2,
		P3,
		P4,
	};

	struct Vec2 {
		float x, y;
		Vec2() {}
		Vec2(float x, float y) : x(x), y(y) {}
	};

#ifdef DEBUG_HOLD_FREE
	struct VecHF {
		UINT hold, free;
		VecHF() {}
		VecHF(UINT h, UINT f) : hold(h), free(f) {}
	};
#endif

private:
	// ---------- Controller Class ----------
	class Controller {

#ifdef USE_JOY_X_MODE
		//Logicool F310 X-Mode
		enum JoyKeyCode {
			JOY_A = 0x0001,
			JOY_B = 0x0002,
			JOY_X = 0x0004,
			JOY_Y = 0x0008,
			JOY_L1 = 0x0010,
			JOY_R1 = 0x0020,
			JOY_BACK = 0x0040,
			JOY_START = 0x0080,
			JOY_L3 = 0x0100,
			JOY_R3 = 0x0200,
		};

		int padCode_[(int)Key::KEY_MAX] = {
			JOY_A,		//BTN_0
			JOY_B,		//BTN_1
			JOY_X,		//BTN_2
			JOY_Y,		//BTN_3
			JOY_L1,		//BTN_L1
			JOY_R1,		//BTN_R1
			0x1000,		//BTN_L2	<-- なし
			0x1000,		//BTN_R2	<-- なし
			JOY_L3,		//BTN_L3
			JOY_R3,		//BTN_R3
			JOY_BACK,	//BTN_BACK
			JOY_START,	//BTN_START
			0x1000,	//JOY_UP,		<-- 使わない
			0x1000,	//JOY_DOWN,		<-- 使わない
			0x1000,	//JOY_LEFT,		<-- 使わない
			0x1000,	//JOY_RIGHT,	<-- 使わない
#ifdef USE_ANALOG_HOLD_FREE
			0x1000,	//ANALOG_L_UP,		<-- 使わない
			0x1000,	//ANALOG_L_DOWN,	<-- 使わない
			0x1000,	//ANALOG_L_LEFT,	<-- 使わない
			0x1000,	//ANALOG_L_RIGHT,	<-- 使わない
			0x1000,	//ANALOG_R_UP,		<-- 使わない
			0x1000,	//ANALOG_R_DOWN,	<-- 使わない
			0x1000,	//ANALOG_R_LEFT,	<-- 使わない
			0x1000,	//ANALOG_R_RIGHT,	<-- 使わない
#endif
		};
#else
		//Logicool F310 D-Mode
		enum JoyKeyCode {
			JOY_X = 0x0001,
			JOY_A = 0x0002,
			JOY_B = 0x0004,
			JOY_Y = 0x0008,
			JOY_L1 = 0x0010,
			JOY_R1 = 0x0020,
			JOY_L2 = 0x0040,
			JOY_R2 = 0x0080,
			JOY_BACK = 0x0100,
			JOY_START = 0x0200,
			JOY_L3 = 0x0400,
			JOY_R3 = 0x0800,
		};

		int padCode_[(int)Key::KEY_MAX] = {
			JOY_A,		//BTN_0
			JOY_B,		//BTN_1
			JOY_X,		//BTN_2
			JOY_Y,		//BTN_3
			JOY_L1,		//BTN_L1
			JOY_R1,		//BTN_R1
			JOY_L2,		//BTN_L2
			JOY_R2,		//BTN_R2
			JOY_L3,		//BTN_L3
			JOY_R3,		//BTN_R3
			JOY_BACK,	//BTN_BACK
			JOY_START,	//BTN_START
			0x1000,	//JOY_UP,		<-- 使わない
			0x1000,	//JOY_DOWN,		<-- 使わない
			0x1000,	//JOY_LEFT,		<-- 使わない
			0x1000,	//JOY_RIGHT,	<-- 使わない
#ifdef USE_ANALOG_HOLD_FREE
			0x1000,	//ANALOG_L_UP,		<-- 使わない
			0x1000,	//ANALOG_L_DOWN,	<-- 使わない
			0x1000,	//ANALOG_L_LEFT,	<-- 使わない
			0x1000,	//ANALOG_L_RIGHT,	<-- 使わない
			0x1000,	//ANALOG_R_UP,		<-- 使わない
			0x1000,	//ANALOG_R_DOWN,	<-- 使わない
			0x1000,	//ANALOG_R_LEFT,	<-- 使わない
			0x1000,	//ANALOG_R_RIGHT,	<-- 使わない
#endif
		};
#endif
		int keyCode_[(int)Key::KEY_MAX] = {
			'A',		//BTN_0
			'S',		//BTN_1
			'D',		//BTN_2
			'F',		//BTN_3
			'Z',		//BTN_L1
			'X',		//BTN_R1
			'Q',		//BTN_L2
			'W',		//BTN_R2
			'E',		//BTN_L3
			'R',		//BTN_R3
			VK_SPACE,		//BTN_BACK
			VK_RETURN,		//BTN_START
			VK_UP,
			VK_DOWN,
			VK_LEFT,
			VK_RIGHT,
#ifdef USE_ANALOG_HOLD_FREE
			NULL,		//ANALOG_L_UP
			NULL,		//ANALOG_L_DOWN
			NULL,		//ANALOG_L_LEFT
			NULL,		//ANALOG_L_RIGHT
			NULL,		//ANALOG_R_UP
			NULL,		//ANALOG_R_DOWN
			NULL,		//ANALOG_R_LEFT
			NULL,		//ANALOG_R_RIGHT
#endif
		};

		struct BtnState {
			UINT holdFrame, freeFrame;
		};

		JOYINFOEX joyInfoEx_;
		bool connected_;	//接続しているか？
		int id_;			//ID
		BtnState keysState_[(int)Key::KEY_MAX];	//各ボタンのState
		Vec2 leftAnalog_, rightAnalog_;	//アナログパッドの情報
		int nowState_;		//現在の情報
		int prevState_;		//前のフレームの情報

	public:
		Controller() = delete;
		Controller(int id)
			:
			id_(static_cast<int>(id)),
			connected_(false),
			nowState_(0), prevState_(0)
		{
			//Set Size & Flag (Initialize)
			joyInfoEx_.dwSize = sizeof(JOYINFOEX);
			joyInfoEx_.dwFlags = JOY_RETURNALL;

			for (auto& key : keysState_) {
				key.holdFrame = key.freeFrame = 0;
			}
		}

		bool update()
		{
			prevState_ = nowState_;
			nowState_ = 0;

			//パッドの情報を取得 & 接続をチェック
			if (joyGetPosEx(id_, &joyInfoEx_) != JOYERR_NOERROR) {
				connected_ = false;
				leftAnalog_ = rightAnalog_ = Vec2(0, 0);
				return false; //Connect Error
			}
			connected_ = true;

			//現在のアナログパッドの情報
			leftAnalog_ = getAnalogXY(joyInfoEx_.dwXpos, joyInfoEx_.dwYpos);
#ifdef USE_JOY_X_MODE
			rightAnalog_ = getAnalogXY(joyInfoEx_.dwUpos, joyInfoEx_.dwRpos);
#else
			rightAnalog_ = getAnalogXY(joyInfoEx_.dwZpos, joyInfoEx_.dwRpos);
#endif

			//各ボタンに対し、押されているフレーム　と　離されているフレームをカウント
			for (int i = 0; i < (int)Key::KEY_MAX; ++i) {

				Key key = static_cast<Key>(i);

				//D-Pad
				if (key == Key::UP || key == Key::DOWN ||
					key == Key::LEFT || key == Key::RIGHT) {
					checkJoyXY(key);
				}
#ifdef USE_ANALOG_HOLD_FREE
				//Analog Check
				else if (key == Key::ANALOG_L_UP || key == Key::ANALOG_L_DOWN ||
					key == Key::ANALOG_L_LEFT || key == Key::ANALOG_L_RIGHT ||
					key == Key::ANALOG_R_UP || key == Key::ANALOG_R_DOWN ||
					key == Key::ANALOG_R_LEFT || key == Key::ANALOG_R_RIGHT){
					checkJoyAnalog(key);
				}
#endif
				//Normal Button
#ifdef USE_JOY_X_MODE
				else if (key == Key::BTN_L2) {
					if (joyInfoEx_.dwZpos > 45000)
						nowState_ |= (1 << i);
				}
				else if (key == Key::BTN_R2) {
					if (joyInfoEx_.dwZpos < 20000)
						nowState_ |= (1 << i);
				}
				else {
					if (joyInfoEx_.dwButtons & padCode_[i])
						nowState_ |= (1 << i);
				}
#else
				else {
					if (joyInfoEx_.dwButtons & padCode_[i])
						nowState_ |= (1 << i);
					//else nowState_ &= ~(1 << i);
				}
#endif

				//Hold & Free Frame Count
				if (checkPush(key) || checkPull(key)) {
					keysState_[i].holdFrame = 0;
					keysState_[i].freeFrame = 0;
				}
				if (checkPress(key)) ++keysState_[i].holdFrame;
				if (checkFree(key)) ++keysState_[i].freeFrame;
			}

			return true; //Connect Success
		}

		void updateKeyboard()
		{
			//各ボタンに対し、押されているフレーム　と　離されているフレームをカウント
			for (int i = 0; i < (int)Key::KEY_MAX; ++i) {
				Key key = static_cast<Key>(i);
				if (GetAsyncKeyState(keyCode_[i]) & 0x8000) //Keyboard (CheckPress)
					nowState_ |= (1 << i);

				//Hold & Free Frame Count
				if (checkPush(key) || checkPull(key)) {
					keysState_[i].holdFrame = 0;
					keysState_[i].freeFrame = 0;
				}
				if (checkPress(key)) ++keysState_[i].holdFrame;
				if (checkFree(key)) ++keysState_[i].freeFrame;
			}
		}

		void checkJoyXY(const Key& btn)
		{
			//Point of View Control (0~360 degree * 100)

			int flag = 1 << static_cast<int>(btn);
			switch (btn) {
			case Key::UP:
				if (joyInfoEx_.dwPOV < JOY_POVRIGHT || (joyInfoEx_.dwPOV > JOY_POVLEFT && joyInfoEx_.dwPOV <= 36000)) nowState_ |= flag;
				break;
			case Key::DOWN:
				if (joyInfoEx_.dwPOV > JOY_POVRIGHT && joyInfoEx_.dwPOV < JOY_POVLEFT) nowState_ |= flag;
				break;
			case Key::LEFT:
				if (joyInfoEx_.dwPOV > JOY_POVBACKWARD && joyInfoEx_.dwPOV < 36000) nowState_ |= flag;
				break;
			case Key::RIGHT:
				if (joyInfoEx_.dwPOV > JOY_POVFORWARD && joyInfoEx_.dwPOV < JOY_POVBACKWARD) nowState_ |= flag;
				break;
			}
		}

#ifdef USE_ANALOG_HOLD_FREE
		void checkJoyAnalog(const Key& btn)
		{
			int flag = 1 << static_cast<int>(btn);
			switch (btn) {
			case Key::ANALOG_L_UP:
				if (leftAnalog_.y < 0) nowState_ |= flag;
				break;
			case Key::ANALOG_L_DOWN:
				if (leftAnalog_.y > 0) nowState_ |= flag;
				break;
			case Key::ANALOG_L_LEFT:
				if (leftAnalog_.x < 0) nowState_ |= flag;
				break;
			case Key::ANALOG_L_RIGHT:
				if (leftAnalog_.x > 0) nowState_ |= flag;
				break;
			case Key::ANALOG_R_UP:
				if (rightAnalog_.y < 0) nowState_ |= flag;
				break;
			case Key::ANALOG_R_DOWN:
				if (rightAnalog_.y > 0) nowState_ |= flag;
				break;
			case Key::ANALOG_R_LEFT:
				if (rightAnalog_.x < 0) nowState_ |= flag;
				break;
			case Key::ANALOG_R_RIGHT:
				if (rightAnalog_.x > 0) nowState_ |= flag;
				break;
			}
		}
#endif

		Vec2 getAnalogXY(DWORD xpos, DWORD ypos)
		{
			//Length 0 ~ 65535 clamp to -1.0~1.0
			const float MID_VALUE = static_cast<float>(0x7FFF); //32767
			const float deadZone = MID_VALUE * 0.15f;
			const float deadMin = MID_VALUE - deadZone;
			const float deadMax = MID_VALUE + deadZone;

			float x, y;
			//X
			if (xpos > deadMin && xpos < deadMax) x = 0;
			else x = static_cast<float>(xpos - MID_VALUE) / (xpos > MID_VALUE ? MID_VALUE + 1 : MID_VALUE);
			//Y
			if (ypos > deadMin && ypos < deadMax) y = 0;
			else y = static_cast<float>(ypos - MID_VALUE) / (ypos > MID_VALUE ? MID_VALUE + 1 : MID_VALUE);

			return Vec2(x, y);
		}

		//「Input」クラスに呼ばれる
		bool checkPush(const Key& btn) const
		{
			int flag = 1 << static_cast<int>(btn);
			return ((prevState_ & flag) == 0) && ((nowState_ & flag) != 0);
		}
		bool checkPull(const Key& btn) const
		{
			int flag = 1 << static_cast<int>(btn);
			return ((prevState_ & flag) != 0) && ((nowState_ & flag) == 0);
		}
		bool checkPress(const Key& btn, int interval = 1) const
		{
			int flag = 1 << static_cast<int>(btn);
			return ((nowState_ & flag) == flag && (keysState_[static_cast<int>(btn)].holdFrame % interval) == 0);
		}
		bool checkFree(const Key& btn, int interval = 1) const
		{
			int flag = 1 << static_cast<int>(btn);
			return ((nowState_ & flag) == 0 && (keysState_[static_cast<int>(btn)].freeFrame % interval) == 0);
		}
		Vec2 getLeftAnalog() const
		{
			return leftAnalog_;
		}
		Vec2 getRightAnalog() const
		{
			return rightAnalog_;
		}

		int getId() const
		{
			return id_;
		}

#ifdef DEBUG_HOLD_FREE
		BtnState getHoldFree(int index) const
		{
			return keysState_[index];
		}
#endif
	};
	// ---------- End Controller Class ----------

	std::vector<Controller> controllers_;
	Input()
	{
		controllers_.clear();

		//Standard -> Detect on joyId = 0 or 1
		controllers_.push_back(Controller(0));
		//controllers_.push_back(Controller(1));

		//Addition (More Controller)
		for (int i = 1; i < MAX_PLAYER; ++i) {
			controllers_.push_back(Controller(i));
		}
	}

	void update()
	{
		//Debug if id starting from 1 (on D-Mode)
		//抜いたらDestructor->Constructor無限ループバッグ発見
		//{
			//JOYINFO tmp; //Detect device is connected or not -> No need to use JOYINFOEX
			//MMRESULT joy0 = joyGetPos(0, &tmp); //ID = 0 のパッドを取得

			////Start from id = 1
			//if (joy0 != JOYERR_NOERROR) {
			//	if (controllers_[0].getId() == 0) {
			//		//Delete index 0 to shift id = 1 as index 0
			//		controllers_.erase(controllers_.begin());

			//		//Less than MAX_PLAYER
			//		if (controllers_.size() < MAX_PLAYER) {
			//			//Add to back
			//			controllers_.push_back(Controller(0));
			//		}
			//		
			//	}
			//}
			////Start from id = 0
			//else {
			//	if (controllers_[0].getId() != 0) {
			//		controllers_.insert(controllers_.begin(), Controller(0));

			//		//More than MAX_PLAYER
			//		if (controllers_.size() > MAX_PLAYER) {
			//			//Delete Back
			//			controllers_.pop_back();
			//		}
			//	}
			//}
		//}

		for (auto& controller : controllers_) {
			if (!controller.update()) {
				//Error on Joy-Stick
				//Do on Keyboard
				controller.updateKeyboard();
			}
		}
	}

	static Input* getInstance()
	{
		static Input instance;
		return &instance;
	}

	bool checkPush(const Key& btn, P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].checkPush(btn);
	}
	bool checkPull(const Key& btn, P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].checkPull(btn);
	}
	bool checkPress(const Key& btn, int interval = 1, P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].checkPress(btn, interval);
	}
	bool checkFree(const Key& btn, int interval = 1, P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].checkFree(btn, interval);
	}
	bool checkAnyPush(const Key& btn) const
	{
		for (const auto& controller : controllers_)
			if (controller.checkPush(btn)) return true;
		return false;
	}
	bool checkAnyPull(const Key& btn) const
	{
		for (const auto& controller : controllers_)
			if (controller.checkPull(btn)) return true;
		return false;
	}
	bool checkAnyPress(const Key& btn) const
	{
		for (const auto& controller : controllers_)
			if (controller.checkPress(btn, 1)) return true;
		return false;
	}
	bool checkAnyFree(const Key& btn) const
	{
		for (const auto& controller : controllers_)
			if (controller.checkFree(btn, 1)) return true;
		return false;
	}
	Vec2 getLeftAnalog(P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].getLeftAnalog();
	}
	Vec2 getRightAnalog(P player = P::P1) const
	{
		return controllers_[static_cast<int>(player)].getRightAnalog();
	}

	int getNoController() const
	{
		return controllers_.size();
	}
	int getControllerId(size_t index) const
	{
		if (index >= controllers_.size()) return -1;
		return controllers_[index].getId();
	}

#ifdef DEBUG_HOLD_FREE
	//Debug
	VecHF getHoldFree(int index, P player = P::P1) const
	{
		auto holdFree = controllers_[static_cast<int>(player)].getHoldFree(index);
		return VecHF(holdFree.holdFrame, holdFree.freeFrame);
	}
#endif
	
public:
	//外部から呼ぶ

	//毎フレームを呼ぶ
	static void CheckKey()
	{
		getInstance()->update();
	}
	
	//ボタン確認
	static bool CheckPush(const Key& btn, P player = P::P1)
	{
		return getInstance()->checkPush(btn, player);
	}
	static bool CheckPull(const Key& btn, P player = P::P1)
	{
		return getInstance()->checkPull(btn, player);
	}
	static bool CheckPress(const Key& buttonID, int interval = 1, P player = P::P1)
	{
		if (interval < 1) interval = 1;
		return getInstance()->checkPress(buttonID, interval, player);
	}
	static bool CheckFree(const Key& buttonID, int interval = 1, P player = P::P1)
	{
		if (interval < 1) interval = 1;
		return getInstance()->checkFree(buttonID, interval, player);
	}
	static bool CheckAnyPress(const Key& btn)
	{
		return getInstance()->CheckAnyPress(btn);
	}
	static bool CheckAnyPush(const Key& btn)
	{
		return getInstance()->CheckAnyPush(btn);
	}
	static bool CheckAnyFree(const Key& btn)
	{
		return getInstance()->CheckAnyFree(btn);
	}
	static bool CheckAnyPull(const Key& btn)
	{
		return getInstance()->CheckAnyPull(btn);
	}
	static Vec2 GetLeftAnalog(P player = P::P1)
	{
		return getInstance()->getLeftAnalog(player);
	}
	static Vec2 GetRightAnalog(P player = P::P1)
	{
		return getInstance()->getRightAnalog(player);
	}

	//ゲームパッドの数（ゲームパッドの最初のIDが0なら　キーボードも数えている）
	static int GetNoController()
	{
		return getInstance()->getNoController();
	}

	//デバッグ関数：vectorの指定された添字の値のIDを取得
	static int GetControllerId(size_t index)
	{
		return getInstance()->getControllerId(index);
	}

#ifdef DEBUG_HOLD_FREE
	//Debug
	static VecHF GetHoldFree(int index, P player = P::P1)
	{
		return getInstance()->getHoldFree(index, player);
	}
#endif
};