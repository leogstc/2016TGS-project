#pragma once
#include "../lib/ci_ext/object.hpp"
#include "../lib/ci_ext/Counter.hpp"
namespace game
{

	class Timer : public ci_ext::Object
	{
		std::weak_ptr<ci_ext::Object> limitTimer_; // êßå¿éûä‘Çä«óù
		float sec_;
		float timer_;
		bool time_over;
		bool isGameTimer_;
		bool isTimeOut_;

	public:
		Timer(float sec);
		Timer(const std::string& objectName, float sec);

		//Timer(const std::string& objectName, float sec)
		// :
		// Object(objectName),
		// sec_(sec)
		//{};


		//
		void init()   override;
		void render() override;
		void update() override;
		void doGameTimer();


		//
		float get() const;
		bool isTimeOver();
		bool isTimeOut() { return isTimeOut_; };

	};

}