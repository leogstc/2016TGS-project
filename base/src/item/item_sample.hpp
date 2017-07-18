#pragma once

#include "itemBase.h"
#include "../effect/effectManager.h"
#include "../ui/uiHPBar.h"



namespace item {
	class Item_001 : public ItemBase {
	public:
		Item_001(int x, int y, int id, int uniqueId)
			:
			ItemBase("item_chrome", "item", 0, x, y, id, uniqueId)
		{}

		void setupAnim()
		{
			anim_.setAnimReset(3, 10);
		}

		void getItem() override
		{
			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired()) {
				effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
			}
			std::cout << "get item chrome | item id = " << id_ << " | uid = " << uid_ << std::endl;
		}
	};

	class Item_002 : public ItemBase {
	public:
		Item_002(int x, int y, int id, int uniqueId)
			:
			ItemBase("item_star", "item", 1, x, y, id, uniqueId)
		{}

		void getItem() override
		{
			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired()) {
				effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
			}
			std::cout << "get item star | item id = " << id_ << " | uid = " << uid_ << std::endl;
		}
	};

	class Item_003 : public ItemBase {
	public:
		Item_003(int x, int y, int id, int uniqueId)
			:
			ItemBase("item_p", "item", 2, x, y, id, uniqueId)
		{}

		void setupAnim()
		{
			anim_.setAnimReset(3, 20);
		}

		void getItem() override
		{
			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired()) {
				effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
			}
			std::cout << "get item p | item id = " << id_ << " | uid = " << uid_ << std::endl;
		}
	};

	class Item_004 : public ItemBase {
	public:
		Item_004(int x, int y, int id, int uniqueId)
			:
			ItemBase("item_p", "item", 3, x, y, id, uniqueId, RECT { 15, 15, 15, 15 })
		{}

		void setupAnim()
		{
			anim_.setAnimReset(3, 10);
		}

		void getItem() override
		{
			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired()) {
				effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
			}
			std::cout << "get item psmall | item id = " << id_ << " | uid = " << uid_ << std::endl;
		}
	};
	class Item_005 : public ItemBase {
	public:
		Item_005(int x, int y, int id, int uniqueId)
			:
			ItemBase("item_heal", "item", 4, x, y, id, uniqueId)
		{}

		void setupAnim()
		{
			anim_.setAnimReset(3, 10);
		}

		void getItem() override
		{
			auto hpBar = ci_ext::weak_cast<BOMB::uiHPBar>(getObjectFromRoot("hp_bar"));
			if (!hpBar.expired())
			{
				hpBar.lock()->doHpRecovery();
			}

			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired()) {
				effMngr.lock()->createEffect("mahoujin", body_.ix(), body_.iy());
			}
			std::cout << "get item heal | item id = " << id_ << " | uid = " << uid_ << std::endl;
		}
	};

}