#pragma once

#include <Windows.h>

class Mover {
public:
	enum Key {
		UP, DOWN, LEFT, RIGHT,
		START, BACK,
		L_Thumb, R_Thumb,
		L, R, L2, R2,
		A, B, X, Y,
		MAX
	};

	Mover() {}
	virtual ~Mover() {}

	virtual void CheckKey(WORD key = 0);
	WORD GetData(); //Debug/Record

	void Vibrate(float left = 1.0f, float right = 1.0f);

	virtual bool CheckPush(int key) { return false; }
	virtual bool CheckPress(int key, int interval = 1) { return false; }
	virtual bool CheckPull(int key) { return false; }
	virtual bool CheckFree(int key, int interval = 1) { return false; }
	virtual bool CheckAnyPress() { return false; }
	virtual float LeftAnalogX() { return 0; }
	virtual float LeftAnalogY() { return 0; }
	virtual float RightAnalogX() { return 0; }
	virtual float RightAnalogY() { return 0; }
};

class KeyMover : public Mover {
public:
	bool CheckPush(int key) override;
	bool CheckPress(int key, int interval = 1) override;
	bool CheckPull(int key);
	bool CheckFree(int key, int interval = 1) override;
	float LeftAnalogX() override;
	float LeftAnalogY() override;
	float RightAnalogX() override;
	float RightAnalogY() override;
	bool CheckAnyPress() override;
};

class AutoMover : public Mover {
	WORD prev_;
	WORD now_;
	UINT hold_[16];
	UINT free_[16];

public:
	AutoMover();
	void CheckKey(WORD key = 0) override;
	bool CheckPush(int key) override;
	bool CheckPress(int key, int interval = 1) override;
	bool CheckPull(int key);
	bool CheckFree(int key, int interval = 1) override;
};