#pragma once

#include "movableObject.hpp"

namespace game
{

class MenuButton : public ci_ext::Object
{
  std::weak_ptr<MovableObject> info_;
  std::unique_ptr<ci_ext::DrawObjf> obj_;
public:
  MenuButton(const std::string& name, const ci_ext::DrawObjf& obj)
    :
    ci_ext::Object(name)
  {
    obj_ = std::make_unique<ci_ext::DrawObjf>(obj);
  }

  void init() override
  {
/*
    info_ = insertAsChild(new game::MovableObject("Title_Start",
      ci_ext::DrawObjf(
      ci_ext::Vec3f(gplib::system::WINW / 2.0f, gplib::system::WINH / 2.0f, 1.0f),
      ci_ext::Vec2f::one(),
      "background", "res/gra/Start.png",
      ci_ext::Vec2f::zero(),
      ci_ext::Vec2f(174, 64),
      ci_ext::Color(255, 255, 255, 255), 0),
      0, 0));
      */
    info_ = insertAsChild(new game::MovableObject("Button_" + name(),
      *obj_,
      0, 0));
    obj_.reset();
  }
  bool IsClick()
  {
    if (info_.expired())
      return false;
    auto pos = gplib::input::GetMousePosition();
    auto rt = info_.lock()->getBody().makeDrawRect();
    return (rt.left <= pos.x && pos.x <= rt.right &&
      rt.top <= pos.y && pos.y <= rt.bottom);
  }
};

}