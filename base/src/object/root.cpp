#include "root.h"

#include "../scene/loading.h"
//#include "../scene/stageSelect.h"
//#include "../scene/stage.hpp"
//#include "../scene/Title.hpp"
//#include "../scene/Logo.hpp"
#include "../effect/effectManager.h"
Root::Root(std::weak_ptr<ci_ext::WindowDx> window)
  :
  Object("root"),
  frame_(0),
  window_(window)
{
}

void Root::init(std::shared_ptr<ci_ext::Object> thisObjectPtr)
{
  setWeakPtr(thisObjectPtr);
  insertAsChild(new scene::Loading(scene::Scene::LOGO));
  //insertAsChild(new scene::Logo());
  //insertAsChild(new scene::Stage());
  //insertAsChild(new scene::StageSelect());
  //insertAsChild(new scene::Title());
  insertAsChild(new effect::EffectManager());
}

void Root::update()
{
  ++frame_;
}

void Root::render()
{
}

int Root::frame() const
{
  return frame_;
}

void Root::changeBkColor(ci_ext::Color newcolor)
{
  window_.lock()->changeBkColor(newcolor);
}

std::weak_ptr<ci_ext::WindowDx> Root::getWindow() const
{
  return window_;
}
