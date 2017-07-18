#include "effectManager.h"

#include "effect_mahoujin.hpp"
#include "effect_clear.h"
#include "effect_ready.h"
#include "effect_popstar.hpp"
#include "effect_gameOver.h"
#include "effect_fadeInBlack.h"
#include "effect_fadeOutBlack.h"
#include "effect_gilgamesh.hpp"

namespace effect {
	EffectManager::EffectManager()
		:
		Object("effect_manager")
	{
		//Insert to table
		//effects_["mahoujin"] = &createInstance<EffectMahoujin>; <-- &付けてもOK、しかし古いコンパイラーはエラーとなる
		effects_["mahoujin"] = createInstance<EffectMahoujin>;
		effects_["clear"] = createInstance<EffectClear>;
		effects_["ready"] = createInstance<EffectReady>;
		effects_["gameover"] = createInstance<EffectGameOver>;
		effects_["popstar"] = createInstance<EffectPopStar>;
		effects_["fadeinblack"] = createInstance<Effect_FadeInBlack>;
		effects_["fadeoutblack"] = createInstance<Effect_FadeOutBlack>;
		effects_["shoteffect"] = createInstance<EffectGilgamesh>;
	}

	void EffectManager::createEffect(const std::string& name, int x, int y)
	{
		if (effects_.find(name) != effects_.end())
			insertAsChild(effects_[name](x, y));
	}

	void EffectManager::destroyEffects()
	{
		auto effs = getChildren();
		for (auto& eff : effs) {
			eff.lock()->kill();
		}
	}
}