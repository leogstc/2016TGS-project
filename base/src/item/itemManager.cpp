#include "itemManager.h"
#include "../lib/gplib.h"

#include "item_sample.hpp"

namespace item {
	ItemManager::ItemManager()
		:
		Object("item_manager")
	{
		//Insert to table
		items_["chrome"] = createInstance<Item_001>;
		items_["star"] = createInstance<Item_002>;
		items_["p"] = createInstance<Item_003>;
		items_["psmall"] = createInstance<Item_004>;
		items_["healItem"] = createInstance<Item_005>;
	}

	void ItemManager::createItem(const std::string& name, int x, int y, int id, int uniqueId)
	{
		if (items_.find(name) != items_.end())
			insertAsChild(items_[name](x, y, id, uniqueId));
		/*if (name == "chrome")
			insertAsChild(new Item_001(x, y, id, uniqueId));
		else if (name == "star")
			insertAsChild(new Item_002(x, y, id, uniqueId));
		else if (name == "p")
			insertAsChild(new Item_003(x, y, id, uniqueId));
		else if (name == "psmall")
			insertAsChild(new Item_004(x, y, id, uniqueId));*/
	}

	void ItemManager::destroyItems()
	{
		auto items = getChildren();
		for (auto& item : items) {
			item.lock()->kill();
		}
	}
}