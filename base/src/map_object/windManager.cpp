#include "windManager.h"
#include "../lib/gplib.h"

//Calculate
#include "../config.h"
#include "../stage/map.h"

#include "wind.h"

#define DEFAULT_WIND_MOVE_SPEED 10.0f

namespace object {
	WindManager::WindManager()
		:
		Object("wind_manager")
	{}

	void WindManager::createWind(int x, int y, map::info::WindDIR dir, std::string move, float speed, int range)
	{
		if (speed == 0)
			speed = DEFAULT_WIND_MOVE_SPEED;

		if (range == 0) {
			if (move == "x")
				insertAsChild(new WindMoveX(x, y, dir, speed));
			else if (move == "y")
				insertAsChild(new WindMoveY(x, y, dir, speed));
			else
				insertAsChild(new WindBase(x, y, dir));
		}
		else {
			int moveFrame = abs(int(round(float(range * map::sprite::TILE_SIZE * config.GAME_SCALE) / speed)));

			if (move == "x")
				insertAsChild(new WindMoveRangeX(x, y, dir, speed, moveFrame));
			else if (move == "y")
				insertAsChild(new WindMoveRangeY(x, y, dir, speed, moveFrame));
		}
	}

	void WindManager::destroyWinds()
	{
		auto winds = getChildren();
		for (auto& wind : winds) {
			wind.lock()->kill();
		}
	}
}