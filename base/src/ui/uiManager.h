#pragma once

#include "../lib/ci_ext/object.hpp"
#include "uiBase.h"
#include <unordered_map>

namespace BOMB {

	template <typename T>
	uiBase* createInstance(const std::string& resName)
	{
		return new T(resName);
	}

	class uiManager : public ci_ext::Object 
	{
		//Value description	------->	Return type	| Function Pointer	| Arguments
		//								uiBase*		| (*)				| (string)
		std::unordered_map<std::string, uiBase* (*)(const std::string& resName)> ui_;

	public:
		uiManager();
		void createUI(const std::string& resName);
		void destroyUI();
	};
}