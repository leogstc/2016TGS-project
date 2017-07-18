#pragma once

#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "ShotBase.h"
#include <map>
namespace shot
{

	class PlayerShot :public ShotBase
	{
		float  pow_;
		const std::string type_;
	public:
		PlayerShot(float pow,const std::string& type);
		~PlayerShot();
		void init() override;
		void initVelocity();
		void render() override;
		void update() override;
		void updatefire();
		bool isShot;

		template <typename T = int>
		void swapLRbyScale(T& a, T& b, T aVal, T bVal)
		{
			a = aVal;
			b = bVal;
		}

		RECT getShotMapOffset();
		RECT getShotMapCollisionRect();
		float getPow();
		std::string getType();
	};
}
