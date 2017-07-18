#include "wind.h"

#include "../config.h"
#include "../stage/map.h"

namespace object {
	//-------------------- BASE (NOT MOVE) --------------------
	WindBase::WindBase(int x, int y, map::info::WindDIR dir, float speed)
		:
		MapCollisionObj("object_wind",
			ci_ext::DrawObjf(
				ci_ext::Vec3f(x, y, 0.8),
				ci_ext::Vec2f(config.GAME_SCALE, config.GAME_SCALE),
				"wind",
				ci_ext::Vec2f(2, 2),
				ci_ext::Vec2f(64, 124),
				0xFFFFFFFF,
				0.0f
			),
			ci_ext::Vec3f::zero()
		),
		dir_(dir),
		speed_(speed)
	{
		//OffsetRectはどれが0、1になると正しく表示されていない場合がある（CustomTile）
		//必ず2以上を入れて
		offsetRt_ = { 2, 46, 2, 2 };
		anim_.setAnimReset(4, 3);
		if (dir_ == map::info::WindDIR::LEFT) {
			body_.setDegree(90);
			offsetRt_ = { 14, 32, -30, 32 };
		}
		else if (dir_ == map::info::WindDIR::RIGHT) {
			body_.setDegree(-90);
			offsetRt_ = { -30, 32, 14, 32 };
		}
		else if (dir_ == map::info::WindDIR::DOWN) {
			body_.setDegree(180);
			offsetRt_ = { 2, 2, 2, 46 };
		}
	}

	void WindBase::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setObjCollisionMapRect(offsetRt_, true);

		//Adjust Position
		mapCheckCollisionY(true);

		//Debug for RECT
		setObjCollisionMapRect(offsetRt_, true);
	}

	void WindBase::update()
	{
		animUpdate();
	}

	void WindBase::animUpdate()
	{
		anim_.step();
		body_.setSrc(ci_ext::Vec2f((anim_.getAnimNo() * 64) + 2, 2));
	}

	void WindBase::render()
	{
		//gplib::font::TextNC(800, 20, 0.0f, std::to_string(body_.x()), ARGB(255, 255, 0, 0), 0);
		//gplib::font::TextNC(800, 40, 0.0f, std::to_string(body_.y()), ARGB(255, 255, 0, 0), 0);
		//gplib::draw::CkRect(getObjCollisionMapRect());
		body_.render();
	}

	map::info::WindDIR WindBase::getWindDir() const
	{
		return dir_;
	}

	ci_ext::Vec3f WindBase::getWindForce() const
	{
		switch (dir_) {
		case map::info::WindDIR::LEFT:
			return ci_ext::Vec3f(-config.WIND_FORCE_X, 0, 0);
		case map::info::WindDIR::RIGHT:
			return ci_ext::Vec3f(config.WIND_FORCE_X, 0, 0);
		default:
		case map::info::WindDIR::UP:
			return ci_ext::Vec3f(0, -config.WIND_FORCE_Y, 0);
		case map::info::WindDIR::DOWN:
			return ci_ext::Vec3f(0, config.WIND_FORCE_Y, 0);
		}
		return ci_ext::Vec3f::zero();
	}
	//--------------------------------------------------


	//-------------------- MOVE --------------------
	WindMoveX::WindMoveX(int x, int y, map::info::WindDIR dir, float speed)
		:
		WindBase(x, y, dir, speed)
	{}

	void WindMoveX::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setObjCollisionMapRect(offsetRt_, true);

		//Adjust Position
		mapCheckCollisionY(true);

		//Debug for RECT
		setObjCollisionMapRect(offsetRt_, true);
	}

	void WindMoveX::update()
	{
		animUpdate();

		velocity_.x(speed_);
		if (checkMoveMapX() != -1)
			speed_ *= -1;
		setObjCollisionMapRect(offsetRt_, true);
	}

	WindMoveY::WindMoveY(int x, int y, map::info::WindDIR dir, float speed)
		:
		WindBase(x, y, dir, speed)
	{}

	void WindMoveY::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setObjCollisionMapRect(offsetRt_, true);

		//Adjust Position
		mapCheckCollisionX(true);

		//Debug for RECT
		setObjCollisionMapRect(offsetRt_, true);
	}

	void WindMoveY::update()
	{
		animUpdate();

		velocity_.y(speed_);
		if (checkMoveMapY() != -1)
			speed_ *= -1;
		setObjCollisionMapRect(offsetRt_, true);
	}
	//--------------------------------------------------



	//-------------------- MOVE IN RANGE --------------------
	WindMoveRangeX::WindMoveRangeX(int x, int y, map::info::WindDIR dir, float speed, int moveFrame)
		:
		WindBase(x, y, dir, speed), moveFrame_(moveFrame), moveCnt_(0)
	{}

	void WindMoveRangeX::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setObjCollisionMapRect(offsetRt_, true);

		//Adjust Position
		mapCheckCollisionY(true);

		//Debug for RECT
		setObjCollisionMapRect(offsetRt_, true);
	}

	void WindMoveRangeX::update()
	{
		animUpdate();

		velocity_.x(speed_);
		checkMoveMapX();
		moveCnt_++;
		if (moveCnt_ >= moveFrame_) {
			moveCnt_ = 0;
			speed_ *= -1;
		}
		setObjCollisionMapRect(offsetRt_, true);
	}

	WindMoveRangeY::WindMoveRangeY(int x, int y, map::info::WindDIR dir, float speed, int moveFrame)
		:
		WindBase(x, y, dir, speed), moveFrame_(moveFrame), moveCnt_(0)
	{}

	void WindMoveRangeY::init()
	{
		{
			auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
			nowInMapName_ = map.lock()->getNowMapName();
		}

		setObjCollisionMapRect(offsetRt_, true);

		//Adjust Position
		mapCheckCollisionX(true);

		//Debug for RECT
		setObjCollisionMapRect(offsetRt_, true);
	}

	void WindMoveRangeY::update()
	{
		animUpdate();

		velocity_.y(speed_);
		checkMoveMapY();
		moveCnt_++;
		if (moveCnt_ >= moveFrame_) {
			moveCnt_ = 0;
			speed_ *= -1;
		}
		setObjCollisionMapRect(offsetRt_, true);
	}
	//--------------------------------------------------
}