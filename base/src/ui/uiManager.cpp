#include "uiManager.h"

#include "uiHPFrame.h"
#include "uiHPBar.h"
#include "uiMPFrame.h"
#include "uiMPBar.h"
#include "uiAvatar.h"
#include "bossUiHPFrame.h"
#include "bossUiHPBar.h"
#include "uiLife.h"
namespace BOMB
{
	uiManager::uiManager()
		:
		Object("ui_manager")
	{
		ui_["hp_frame"] = createInstance<uiHPFrame>;
		ui_["hp_bar"] = createInstance<uiHPBar>;
		ui_["mp_frame"] = createInstance<uiMPFrame>;
		ui_["mp_bar"] = createInstance<uiMPBar>;
		ui_["avatar"] = createInstance<uiAvatar>;
		ui_["boss_hp_frame"] = createInstance<bossUiHPFrame>;
		ui_["boss_hp_bar"] = createInstance<bossUiHPBar>;
	}

	void uiManager::createUI(const std::string& resName)
	{
		if (ui_.find(resName) != ui_.end())
		{
			insertAsChild(ui_[resName](resName));
		}
	}

	void uiManager::destroyUI()
	{
		auto uis = getChildren();
		for (auto& ui : uis) {
			ui.lock()->kill();
		}
	}


}