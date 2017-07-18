#include "titleObj.h"

namespace titleObj
{
	using namespace ci_ext;
	using namespace gplib;
	using namespace std;

	TitleObj::TitleObj(const string& objname,const string& resname,const ci_ext::Vec3f& pos,float offsetY,float speed)
		:
		MovableObject(objname,
			ci_ext::DrawObjf(ci_ext::Vec3f::zero(),
				ci_ext::Vec2f::one(),
				resname,
				ci_ext::Vec2f::zero(),
				ci_ext::Vec2f::zero(),
				ci_ext::Color(255, 255, 255, 255),
				0.f),
			-speed//speed
			),
		posy_(pos.y() - (body_.srcSize().y() / 2.f) + offsetY),
		posz_(pos.z())
	{
		body_.setPos(Vec3f(pos.x(), posy_,pos.z()));
	}

	void TitleObj::update()
	{
		if (isOutScreen())
			body_.setPos(ci_ext::Vec3f(system::WINW + body_.srcSize().x() / 2.f, posy_, posz_));

		body_.offsetPos(velocity_);
	}

	bool TitleObj::isOutScreen()
	{
		return body_.pos().x() + (body_.srcSize().x() / 2.f) <= 0.f;
	}
}