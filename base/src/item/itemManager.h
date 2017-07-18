#pragma once

#include "../lib/ci_ext/object.hpp"
#include "itemBase.h"
#include <unordered_map>

namespace item {
	template <typename T>
	ItemBase* createInstance(int x, int y, int id, int uniqueId)
	{
		return new T(x, y, id, uniqueId);
	}

	class ItemManager : public ci_ext::Object {
		std::unordered_map<std::string, ItemBase*(*)(int, int, int, int)> items_;

	public:
		ItemManager();
		void createItem(const std::string& name, int x, int y, int id, int uniqueId);
		void destroyItems();
	};
}