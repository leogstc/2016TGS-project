#include "playerDead.h"
#include "../stage/map.h"

namespace player {
	PlayerDeadManager::PlayerDeadManager(float x, float y)
		:
		Object("player_dead_manager"),
		pos_(Point2f(x, y)),
		t_(0)
	{}

	void PlayerDeadManager::init() {}

	void PlayerDeadManager::update()
	{
		//Reset Stage
		if (t_ == 120) {
			//Destroy this Object
			kill();

			//Reset Stage
			auto map = ci_ext::weak_cast<map::Map>(getParentPtr());
			map.lock()->playerRebirth();
			//playerRebirth()呼ばれたら　絶対return;して！
			//次の処理をしないように！ <-- MapCollisionCheckしたらバグが出る
			return;
		}

		//Create Effect
		if (t_ == 0 || t_ == 30 || t_ == 60) {
			for (int i = 0; i < 360; i += 45) {
				insertAsChild(new PlayerDeadCircle(pos_.x, pos_.y, (float)i, 5.0f, 20.0f));
			}
		}
		
		t_++;
	}

	void PlayerDeadManager::render() {}




	PlayerDeadCircle::PlayerDeadCircle(float x, float y, float degree, float speed, float radius)
		:
		Object("player_dead_" + std::to_string((int)degree)),
		pos_(Point2f(x, y)),
		radius_(radius)
	{
		speed_.x = gplib::math::ROUND_X(degree, speed);
		speed_.y = gplib::math::ROUND_Y(degree, speed);
	}

	void PlayerDeadCircle::init() {}

	void PlayerDeadCircle::update()
	{
		pos_.x += speed_.x;
		pos_.y += speed_.y;

		//Out of Screen
		if (!gplib::camera::InScreenCameraCenter(pos_.x, pos_.y, radius_, radius_)) {
			kill();
		}
	}

	void PlayerDeadCircle::render()
	{
		for (float i = 0; i < 360; ++i) {
			float x1, y1, x2, y2;
			x1 = pos_.x + (float)cos(i * M_PI / 180) * radius_;
			y1 = pos_.y + (float)-sin(i * M_PI / 180) * radius_;
			x2 = pos_.x + (float)cos((i + 1) * M_PI / 180) * radius_;
			y2 = pos_.y + (float)-sin((i + 1) * M_PI / 180) * radius_;
			//Draw_Line(x1, y1, x2, y2, ...);
			gplib::draw::Line((int)x1, (int)y1, (int)x2, (int)y2, 0.0f, ARGB(255, 0, 255, 255), 1);
		}
	}
}