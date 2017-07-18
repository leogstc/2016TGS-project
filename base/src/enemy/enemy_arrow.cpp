#include "enemy_arrow.h"
#include "../config.h"
#include "../stage/map.h"
#include "../lib/gplib.h"
#include "enemy_archer.h"
#include "../player/player.h"

namespace shot
{
	EnemyArrow::EnemyArrow(float dmg)
		:
		ShotBase("enemy_archer_arrow", "enemy_archer_arrow", dmg)
	{
		body_.setSrc(ci_ext::Vec2f(0.f, 0.f));
		body_.setSrcSize(ci_ext::Vec2f(90.f, 15.f));
		body_.setScale(ci_ext::Vec2f(arrowScale_, arrowScale_));
	}

	EnemyArrow::EnemyArrow(float dmg, const ci_ext::Vec3f& pos)
		:
		ShotBase("enemy_archer_arrow", "enemy_archer_arrow", dmg)
	{
		body_.setPos(ci_ext::Vec3f(pos.x(), pos.y(), 0.75f));
		body_.setSrc(ci_ext::Vec2f(0.f, 0.f));
		body_.setSrcSize(ci_ext::Vec2f(90.f, 15.f));
		body_.setScale(ci_ext::Vec2f(arrowScale_, arrowScale_));
	}

	EnemyArrow::~EnemyArrow()
	{
	}

	void EnemyArrow::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		anim_.setAnim(3, 5);
		//auto enemy = ci_ext::weak_to_shared <enemy::Enemy_Archer>(getObjectFromRoot("enemy_archer"));
		//ci_ext::Vec2f shotp = enemy->getPos();
		//body_.setPos(shotp.x(), shotp.y(), 0.75f);
		isShot = false;
	
		offsetRandomAtkSpeed = 0;
		calculatedAngle_ = 0;
		atkspeed = 0.0f;		
	}

	void EnemyArrow::render()
	{
		body_.render();
#ifdef _DEBUG
		auto atkRt = getShotMapCollisionRect();
		gplib::draw::CkRect(atkRt);

		//gplib::font::TextNC(250, 520, 0.0f, "Degree  : " + std::to_string(body_.degree()), ARGB(255, 255, 0, 0), 0); 
		gplib::font::TextNC(400, 80, 0.0f, "Player Pos  : " + std::to_string(distance), ARGB(255, 255, 0, 0), 0);
		gplib::font::TextNC(400, 100, 0.0f, "Angle  : " + std::to_string(calculatedAngle_), ARGB(255, 255, 0, 0), 0);
		gplib::font::TextNC(400, 140, 0.0f, "offsetRandomAtkSpeed  : " + std::to_string(offsetRandomAtkSpeed), ARGB(255, 255, 0, 0), 0);
		gplib::font::TextNC(400, 180, 0.0f, "AtkSpeed  : " + std::to_string(atkspeed), ARGB(255, 255, 0, 0), 0);
#endif
	}

	void EnemyArrow::update()
	{
		anim_.step();
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));
		setObjCollisionMapRect(getShotMapCollisionRect());

		//èdóÕ
		velocity_.offset(0, gravity/2.0f );
		updatefire();

		int i = checkMoveMapX();
		if (i != -1) kill();

		setObjCollisionMapRect(getShotMapCollisionRect());
		checkHitPlayer();
	}

	void EnemyArrow::updatefire()
	{
		auto enemy = ci_ext::weak_to_shared <enemy::Enemy_Archer>(getObjectFromRoot("enemy_archer"));
		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));
		//enemy::DIR shotdir = enemy->getDir();

		if (player != nullptr)
		{
			tpos = player->getPos();

			if (!isShot)
			{
				float diffX = body_.x() - tpos.x();
				float diffY = body_.y() - tpos.y();
				
				// Checks if projectile can hit (x, y) coordinate with initial velocity length under given gravity.
				canHitTarget_ = canHitCoordinate(diffX, diffY, atkspeed);
		
				while (!canHitTarget_)
				{
					atkspeed += 0.5f;
					canHitTarget_ = canHitCoordinate(diffX, diffY, atkspeed);
				}

				//Calculate distance beteen player and enemy.
				distance = sqrt((diffY * diffY) + (diffX * diffX));
				
				//Bad Example
				//if (distance > 450.0f) 
				//{
				//	randomAngle = gplib::math::GetRandom(30, 60);
				//}
				//else
				//{
				//	randomAngle = gplib::math::GetRandom(10, 15);
				//}
				//if ((float)shotdir > 0)
				//{
				//	an = gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()) + randomAngle;
				//	velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
				//	velocity_.y(gplib::math::ROUND_Y(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()) + randomAngle, atkspeed));
				//	isShot = true;
				//}
				//else if ((float)shotdir < 0)
				//{
				//	an = gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()) - randomAngle;
				//	velocity_.x(gplib::math::ROUND_X(gplib::math::DegreeOfPoints2(body_.x(), body_.y(), tpos.x(), tpos.y()), atkspeed));
				//	velocity_.y(gplib::math::ROUND_Y(an, atkspeed));
				//
				//	isShot = true;
				//}
			
				calculatedAngle_ = calAngle(diffX, diffY, atkspeed);

				//Add some offset to atkSpeed to make it less precise.
				offsetRandomAtkSpeed = gplib::math::GetRandom(-0.75f, 0.75f);
				atkspeed = atkspeed / 2.0f;
				
				velocity_.x((gplib::math::ROUND_X(calculatedAngle_ , atkspeed + offsetRandomAtkSpeed)) );
				velocity_.y((gplib::math::ROUND_Y(calculatedAngle_ , atkspeed + offsetRandomAtkSpeed)) );
				isShot = true;


			}
		}

		//ARROWÇÃäpìxÇï\é¶
		body_.setDegree(gplib::math::DegreeOfPoints2(0, 0, velocity_.x(), velocity_.y()));

		body_.offsetPos(velocity_);
	}

	RECT EnemyArrow::getShotMapOffset()
	{
		RECT rt = { 0, 0, 0, 0 };
		rt = { 2,2,2,2 };
		swapLRbyScale(rt.left, rt.right, rt.left, rt.right);
		return rt;
	}

	RECT EnemyArrow::getShotMapCollisionRect()
	{
		RECT offset = getShotMapOffset();
		return makeRectScale(offset.left, offset.top, offset.right, offset.bottom);
	}

	/*
	//http://stackoverflow.com/questions/18047912/calculate-angle-required-to-hit-coordinate-x-y-from-position-other-than-0-0
	//http://www.n-created.com/2013/11/12/projectile-initial-velocity-angle-to-hit-coordinate/
	*/
	float EnemyArrow::calAngle(float diffX,float diffY, float atkspeed)
	{
		float delta = calculateDelta(diffX,diffY,atkspeed);
		float sqrtDelta = sqrt(delta);
		float angleInRadians = atan2(((atkspeed*atkspeed) + sqrtDelta), (gravity*-diffX));

		//convert to degree
		float degrees = gplib::math::Calc_RadToDegree(angleInRadians);

		return degrees;
	}
	bool EnemyArrow::canHitCoordinate(float diffX, float diffY, float velocity)
	{
		return calculateDelta(diffX, diffY, velocity) >= 0;
	}
	float EnemyArrow::calculateDelta(float diffX, float diffY, float velocity)
	{	
		return velocity * velocity * velocity * velocity - gravity * (gravity * diffX * diffX + 2 * diffY * velocity * velocity);
	}



}