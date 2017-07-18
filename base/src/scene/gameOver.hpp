#pragma once
#include "../lib/ci_ext/object.hpp"
#include <map>
#include "../effect/effectManager.h"
#include "../util/timer.h"
#include "end.h"
#include "../effect/effect_gameOver.h"
namespace scene
{
	class GameOver : public ci_ext::Object
	{
		std::weak_ptr<game::Timer> timer_;
		std::weak_ptr<effect::EffectManager> effMngr;
	public:
		GameOver()
			:
			ci_ext::Object("scene_gameOver")
		{}

		void init() override
		{
			timer_ = insertAsChild(new game::Timer("gameover_timer", 5.f));
			effMngr= ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			//auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired())
				effMngr.lock()->createEffect("gameover");
		}

		void update() override
		{
			auto timer = ci_ext::weak_cast<game::Timer>(timer_);
			if (!timer.expired())
			{
				if (timer.lock()->isTimeOver())
				{
					//auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
					effMngr.lock()->createEffect("fadeinblack");
					sleep(45);
				}
			}
		}

		void resume() override
		{
			
			//if (!effMngr.expired())
			//	effMngr.lock()->kill();
			auto effect = getObjectFromRoot("gameover");
			effect.lock()->kill();
			//insertToParent(new End());
			getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::END));
			kill();

		}

		void render() override
		{
		}
	};
}