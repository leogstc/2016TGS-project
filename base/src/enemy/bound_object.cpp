#include "bound_object.h"
#include "../config.h"
#include "../stage/map.h"
#include "../effect/effectManager.h"
#include "../lib/gplib.h"
namespace enemy
{
	Bound_Object::Bound_Object(const std::string & objectName, const ci_ext::DrawObjf & obj, const RECT & offRt, /*const std::string shottype,*/ const ci_ext::Vec3f & velocity)
		:
		MapCollisionObj(
			objectName,
			ci_ext::DrawObjf(
				obj.pos(),		//xyz
				obj.scale(),	//scale
				obj.resname(),	//resname
				obj.src(),		//src
				obj.srcSize(),	//size
				0xFFFFFFFF,		//color
				0.0f			//degree
				),
			velocity			//velocity
			),
		offRt_(offRt),
		speed_(10.f),
		atkPt_(30.f),
		rebound_cnt(0),
		MAX_REBOUND(0)
		//shottype_(shottype)
	{
		using namespace ci_ext;
		using namespace gplib;
		//dirVelocity[Dir::right] = Vec3f(speed_,0.f);
		//dirVelocity[Dir::left] = Vec3f(-speed_, 0.f);
		//dirVelocity[Dir::topRight] = /*Vec3f(0.f, -speed_);*/Vec3f(math::ROUND_X(45.f, speed_), math::ROUND_Y(45.f, speed_));
		//dirVelocity[Dir::topLeft] = Vec3f(math::ROUND_X(135, speed_), math::ROUND_Y(135.f, speed_));
		//dirVelocity[Dir::bottomRight] = Vec3f(math::ROUND_X(315.f, speed_), math::ROUND_Y(315.f, speed_));
		//dirVelocity[Dir::bottomLeft] = Vec3f(math::ROUND_X(225.f, speed_), math::ROUND_Y(225.f, speed_));

		offRt_.bottom -= 20;

		Rtw_ = offRt_.right - offRt_.left;
		Rth_ = offRt_.bottom - offRt_.top;

	}

	void Bound_Object::init()
	{
		auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		nowInMapName_ = map.lock()->getNowMapName();
		em = ci_ext::weak_cast<EnemyManager>(getObjectFromRoot("enemy_manager"));
		//initDir();
		//velocity_ = dirVelocity[dir_];
		//velocity_Çê≥ãKâª
		vector_Nomalize(velocity_);
		velocity_.x(velocity_.x()*speed_);
		velocity_.y(velocity_.y()*speed_);
	}

	void enemy::Bound_Object::initDir()
	{
		//ÉvÉåÉCÉÑÅ[ÇÃà íuÇ∆Ç†ÇΩÇ¡ÇΩíeÇÃéÌóﬁÇ≈ï˚å¸ÇåàÇﬂÇÈ
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		//if (player.lock()->getPos().x() >= body_.x())
		//{
		//	if (shottype_ == "up")
		//		dir_ = Dir::topLeft;
		//	else if (shottype_ == "down")
		//		dir_ = Dir::bottomLeft;
		//	else
		//		dir_ = Dir::left;
		//}
		//else
		//{
		//	if (shottype_ == "up")
		//		dir_ = Dir::topRight;
		//	else if (shottype_ == "down")
		//		dir_ = Dir::bottomRight;
		//	else
		//		dir_ = Dir::right;
		//}
	}

	void Bound_Object::update()
	{
		//offRt_çXêV
		offRt_.right = body_.ix() + Rtw_ / 2;
		offRt_.left = body_.ix() - Rtw_ / 2;
		offRt_.bottom = body_.iy() + Rth_ / 2;
		offRt_.top = body_.iy() - Rth_ / 2;

		offRt_.top += (int)speed_;
		offRt_.bottom -= (int)speed_;
		setObjCollisionMapRect(offRt_);
		//âÒì]
		body_.offsetDegree(5.f);
		//ï«Ç…ìñÇΩÇ¡ÇΩÇÁíµÇÀï‘ÇÈ
		checkHitMap();
		//ìGÇ…ìñÇΩÇ¡ÇΩÇÁíµÇÀï‘ÇÈ
		if (checkHitEnemy())
			rebound();
		//velocity_ = dirVelocity[dir_];
		body_.offsetPos(velocity_);
	}

	void Bound_Object::render()
	{
		body_.render();
#ifdef _DEBUG
		gplib::draw::CkRect(offRt_);
#endif // _DEBUG
	}

	void enemy::Bound_Object::rebound()
	{
		if (++rebound_cnt > MAX_REBOUND)
		{
			kill();
			auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			if (!effMngr.expired())
				effMngr.lock()->createEffect("popstar", body_.ix(), body_.iy());
		}
		//change_DIR();
		//switch (dir_)
		//{
		//case Dir::left:
		//	dir_ = Dir::right;
		//	velocity_.x(speed_);
		//	break;

		//case Dir::right:
		//	dir_ = Dir::left;
		//	velocity_.x(-speed_);
		//	break;
		//}
	}

	//void enemy::Bound_Object::change_DIR()
	//{
	//	if (dir_ == Dir::right) {
	//		dir_ = Dir::left;
	//	}
	//	else if (dir_ == Dir::left) {
	//		dir_ = Dir::right;
	//	}
	//	else if (dir_ == Dir::topRight) {
	//		dir_ = Dir::topLeft;
	//	}
	//	else if (dir_ == Dir::topLeft) {
	//		dir_ = Dir::topRight;
	//	}
	//	else if (dir_ == Dir::bottomRight) {
	//		dir_ = Dir::bottomLeft;
	//	}
	//	else {
	//		dir_ = Dir::bottomRight;
	//	}

	//}

	void enemy::Bound_Object::checkHitMap()
	{
		if (checkCollisionMapX())
		{
			velocity_.x(velocity_.x()*-1.f);
			rebound();
			return;
		}
		offRt_.right = body_.ix() + Rtw_ / 2;
		offRt_.left = body_.ix() - Rtw_ / 2;
		offRt_.bottom = body_.iy() + Rth_ / 2;
		offRt_.top = body_.iy() - Rth_ / 2;
		offRt_.right -= (int)speed_;
		offRt_.left += (int)speed_;
		setObjCollisionMapRect(offRt_);
		if (checkCollisionMapY())
		{
			velocity_.y(velocity_.y()*-1.f);
			rebound();
			return;
		}
	}

	bool enemy::Bound_Object::checkHitEnemy()
	{
		if (!em.expired())
		{
			auto enemies = em.lock()->getChildren();
			for (auto enemy : enemies)
			{
				auto e = ci_ext::weak_cast<EnemyBase>(enemy);
				if (!e.expired())
				{
					if (gplib::math::RectCheck(offRt_, e.lock()->getdefRECT()))
					{
						e.lock()->damage(atkPt_, velocity_);
						return true;
					}
				}
			}
		}
		return false;
	}

	void enemy::Bound_Object::vector_Nomalize(ci_ext::Vec3f& vec)
	{
		float length = getVec_Length(vec);
		ci_ext::Vec3f n;
		n.x(vec.x() / length);
		n.y(vec.y() / length);
		vec = n;
	}

	float enemy::Bound_Object::getVec_Length(ci_ext::Vec3f v)
	{
		float length = sqrtf(v.x()*v.x() + v.y()*v.y());
		return length;
	}
}