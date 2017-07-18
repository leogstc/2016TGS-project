#pragma once
#include "../lib/ci_ext/object.hpp"
#include "../lib/ci_ext/DrawObj.hpp"
#include "../lib/ci_ext/math.hpp"

namespace game
{

class MovableObject : public ci_ext::Object
{
protected:
  ci_ext::DrawObjf body_;
  ci_ext::Vec3f velocity_;
public:
  MovableObject(const std::string& objectName, const ci_ext::DrawObjf& f, float degree, float speed)
    :
    Object(objectName),
    body_(f),
    velocity_(cos(ci_ext::DegreeToRadian(degree)) * speed, -sin(ci_ext::DegreeToRadian(degree)) * speed, 0)
  {
  }
  MovableObject(const std::string& objectName, const ci_ext::DrawObjf& f, const ci_ext::Vec3f& v)
	  :
	  Object(objectName),
	  body_(f),
	  velocity_(v)
  {
  }

  void render() override
  {
    body_.render();
  }
  void update() override
  {
    body_.offsetPos(velocity_);
    auto halfsize = body_.halfsize<float>();
    if (!gplib::camera::InScreenCameraCenter(
      body_.x(), body_.y(),
      halfsize.x(),
      halfsize.y()))
    {
      kill();
    }
  }

  const ci_ext::DrawObjf& getBody()
  {
    return body_;
  }

  const ci_ext::Vec3f& getVelocity()
  {
    return velocity_;
  }
};

}