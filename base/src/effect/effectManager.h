#pragma once

#include "../lib/ci_ext/object.hpp"
#include "effectBase.h"
#include <unordered_map>

namespace effect {
	//Reference
	//Function pointer
	//http://stackoverflow.com/questions/582331/
	//Lambda function (C++11)
	//http://stackoverflow.com/questions/27894130/

	//This template uses "582331" one
	template <typename T>
	EffectBase* createInstance(int x, int y)
	{
		return new T(x, y);
	}

	class EffectManager : public ci_ext::Object {
		//Using Lambda function
		//std::unordered_map<std::string, std::function<EffectBase*(int, int)>> effects_;

		//Using function pointer
		//Value description	-->	Return type	| Function Pointer	| Arguments
		//						EffectBase*	| (*)				| (int, int)
		std::unordered_map<std::string, EffectBase*(*)(int, int)> effects_;

	public:
		EffectManager();
		void createEffect(const std::string& name, int x = 0, int y = 0);
		void destroyEffects();
	};
}