#pragma once

#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "../player/ShotBase.h"

namespace shot
{
	class EnemyArrow : public ShotBase
	{
	private:
		ci_ext::Vec3f tpos;
		float distance;
		float offsetRandomAtkSpeed;
		float atkspeed;
		float gravity = 0.2f;;
		float calculatedAngle_;
		bool canHitTarget_;
	

	public:
		EnemyArrow(float dmg);
		EnemyArrow(float dmg, const ci_ext::Vec3f& pos);
		~EnemyArrow();
		void init() override;
		void render() override;
		void update() override;
		void updatefire();
		bool isShot;
		const float arrowScale_ = 0.8f;

		template <typename T = int>
		void swapLRbyScale(T& a, T& b, T aVal, T bVal)
		{
			a = aVal;
			b = bVal;
		}

		RECT getShotMapOffset();
		RECT getShotMapCollisionRect();
		float calAngle(float diffX,float diffY, float speed);
		bool canHitCoordinate(float diffX, float diffY, float speed);
		float calculateDelta(float diffX, float diffY, float speed);
	};
}
