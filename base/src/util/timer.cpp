#include "timer.h"
#include "../lib/gplib.h"
#include <iostream>
#include <string>

namespace game
{

	Timer::Timer(const std::string& objectName, float sec)
		:
		Object(objectName),
		sec_(sec),
		time_over(false)
	{
	}

	Timer::Timer(float sec)
		:
		Object("game_timer"),
		sec_(sec),
		time_over(false),
		isGameTimer_(true),
		isTimeOut_(false)

	{
	}
	void Timer::init()
	{
		limitTimer_ = insertAsChild(new ci_ext::TimeCounter<float>(""));
		timer_ = sec_;
	}

	void Timer::render()
	{
		if (isGameTimer_)
		{
			std::string name("Žc‚èF " + std::to_string((int)timer_));
			//lib_windowDx.hpp  ---- > gplib::font::Create(2, 35, "consolas");
			gplib::font::TextNC(gplib::system::WINW - 200, 30, 0.0f, name, ARGB(255, 255, 201, 14), 2);
		}
		else
		{
			auto limitTimer = ci_ext::weak_to_shared<ci_ext::TimeCounter<float>>(limitTimer_);
		}
#ifdef _DEBUG
		std::string name("Timer : " + std::to_string(timer_));
		gplib::font::TextNC(10, 100, 0.0f, name, ARGB(255, 255, 0, 0), 0);
#endif
	}

	void Timer::update()
	{
		if (isGameTimer_ && !isSleeping())
		{
			doGameTimer();
		}
		else
		{
			auto limitTimer = ci_ext::weak_to_shared<ci_ext::TimeCounter<float>>(limitTimer_);
			timer_ = sec_ - limitTimer->get();
			if (timer_ <= 0.0f)
			{
				time_over = true;
			}
		}
	}
	float Timer::get() const
	{
		auto limitTimer = ci_ext::weak_to_shared<ci_ext::TimeCounter<float>>(limitTimer_);
		return limitTimer->get();
	}

	void Timer::doGameTimer()
	{
		timer_ -= gplib::system::ONEFRAME_TIME;
		if (timer_ <= 0.0f)
		{
			isTimeOut_ = true;
			kill();
		}
	}

	bool Timer::isTimeOver()
	{
		return time_over;
	}


}