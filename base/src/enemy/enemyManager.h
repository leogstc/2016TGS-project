#pragma once

#include "../lib/ci_ext/object.hpp"
#include "enemyBase.h"
#include <unordered_map>

namespace enemy {
	template <typename T>
	EnemyBase* createInstance(int x, int y, int id)
	{
		return new T(x, y, id);
	}

	class EnemyManager : public ci_ext::Object {
		std::unordered_map<std::string, EnemyBase*(*)(int, int, int)> enemies_;

	public:
		EnemyManager();
		void createEnemy(const std::string& name, int x, int y, int id);
		void destroyEnemies();
	};
}