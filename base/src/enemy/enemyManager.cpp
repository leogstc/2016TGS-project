#include "enemyManager.h"
#include "../lib/gplib.h"

//#include "enemy_sample.hpp"
//#include "generator.h"
//#include "enemy_koh.h"
#include "enemy_fly.h"
//#include "enemy_orangeSlime.h"
#include "enemy_bat.h"
#include "enemy_slime.h"
#include "enemy_archer.h"
#include "enemy_monster.h"
#include "enemy_boss.h"

namespace enemy {
	EnemyManager::EnemyManager()
		:
		Object("enemy_manager")
	{
		//Insert to table
		//enemies_["bug"] = createInstance<Enemy_001>;
		//enemies_["cursor"] = createInstance<Enemy_002>;
		//enemies_["generator001"] = createInstance<BAYASHI::EnemyGenerator>;
		//enemies_["enemy_eplayer"] = createInstance<EnemyKoh>;
		enemies_["enemy_fly"] = createInstance<Enemy_Fly>;
		//enemies_["orangeSlime"] = createInstance<Enemy_OrangeSlime>;
		enemies_["enemy_monster"] = createInstance<Enemy_Monster>;
		enemies_["enemy_bat"] = createInstance<Enemy_Bat>;
		enemies_["enemy_slime"] = createInstance<Enemy_Slime>;
		enemies_["enemy_archer"] = createInstance<Enemy_Archer>;
		enemies_["enemy_boss"] = createInstance<Enemy_Boss>;
	}

	void EnemyManager::createEnemy(const std::string& name, int x, int y, int id)
	{
		if (enemies_.find(name) != enemies_.end())
			insertAsChild(enemies_[name](x, y, id));
		/*if (name == "bug")
			insertAsChild(new Enemy_001(x, y, id));
		else if (name == "cursor")
			insertAsChild(new Enemy_002(x, y, id));*/
	}

	void EnemyManager::destroyEnemies()
	{
		auto enemies = getChildren();
		for (auto& enemy : enemies) {
			enemy.lock()->kill();
		}
	}
}