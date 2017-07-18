#include "mover.h"

//#include "../lib/ci_ext/xinput.hpp"
#include "../lib/xinput.hpp"
//#include "../lib/gplib.h"

void Mover::CheckKey(WORD key)
{
	XInput::CheckKey();
}

WORD Mover::GetData()
{
	return XInput::GetData();
}

void Mover::Vibrate(float left, float right)
{
	XInput::Vibration(left, right);
}


//-------------------- Key Mover --------------------
bool KeyMover::CheckPush(int key) { return XInput::CheckPush((XInput::ButtonsID)key); }
bool KeyMover::CheckPress(int key, int interval) { return XInput::CheckPress((XInput::ButtonsID)key, interval); }
bool KeyMover::CheckPull(int key) { return XInput::CheckPull((XInput::ButtonsID)key); }
bool KeyMover::CheckFree(int key, int interval) { return XInput::CheckFree((XInput::ButtonsID)key, interval); }
float KeyMover::LeftAnalogX() { return XInput::LeftThumb().x; }
float KeyMover::LeftAnalogY() { return XInput::LeftThumb().y; }
float KeyMover::RightAnalogX() { return XInput::RightThumb().x; }
float KeyMover::RightAnalogY() { return XInput::RightThumb().y; }
bool KeyMover::CheckAnyPress()
{
	for (int i = 0; i < Key::MAX;++i) {
		if (CheckPress(i)) {
			return true;
		}
	}
	return false;
}


//-------------------- Auto Mover --------------------
AutoMover::AutoMover()
	:
	prev_(0), now_(0)
{
	for (int i = 0; i < 16; ++i) {
		hold_[i] = 0;
		free_[i] = 0;
	}
}

void AutoMover::CheckKey(WORD key)
{
	prev_ = now_;
	now_ = key;

	for (int i = 0; i < 16; ++i) {
		if (CheckPush(i) || CheckPull(i)) {
			hold_[i] = 0;
			free_[i] = 0;
		}
		if (CheckPress(i)) hold_[i]++;
		if (CheckFree(i)) free_[i]++;
	}
}

bool AutoMover::CheckPush(int key)
{
	int flag = (1 << key);
	return ((now_ & flag) != 0) && ((prev_ & flag) == 0);
}

bool AutoMover::CheckPress(int key, int interval)
{
	int flag = (1 << key);
	return ((now_ & flag) == flag) && ((hold_[key] % interval) == 0);
}

bool AutoMover::CheckPull(int key)
{
	int flag = (1 << key);
	return ((now_ & flag) == 0) && ((prev_ & flag) != 0);
}

bool AutoMover::CheckFree(int key, int interval)
{
	int flag = (1 << key);
	return ((now_ & flag) == 0) && ((free_[key] % interval) == 0);
}

