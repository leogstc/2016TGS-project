#pragma once

#include "enemyBase.h"

namespace enemy {
	class Enemy_001 : public EnemyBase {
	public:
		Enemy_001(int x, int y, int id)
			:
			EnemyBase("enemy_bug", "enemy001", x, y,0,id)
		{
			body_.setSrcSize(ci_ext::Vec2f(64, 64));
		}
	};

	class Enemy_002 : public EnemyBase {
	public:
		Enemy_002(int x, int y, int id)
			:
			EnemyBase("enemy_cursor", "enemy002", x, y,0,id)
		{
			body_.setSrcSize(ci_ext::Vec2f(96, 96));
		}
	};
}