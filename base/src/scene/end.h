#pragma once
#include "../lib/ci_ext/object.hpp"
#include "../effect/effectManager.h"
#include "../util/timer.h"
namespace scene
{
	class End :public ci_ext::Object
	{
		int cnt;
		int startx;
		bool flag;
		std::weak_ptr<effect::EffectManager> efMana;
		std::weak_ptr<game::Timer> timer_;
	public:
		End();
		void init()	  override;
		void update() override;
		void render() override;
		void resume() override;
	};
}
