#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/lib_WindowDx.hpp"

//ルートオブジェクト
class Root : public ci_ext::Object
{
  int frame_;
  std::weak_ptr<ci_ext::WindowDx> window_;
public:
  Root(std::weak_ptr<ci_ext::WindowDx> window);

  void init(std::shared_ptr<ci_ext::Object> thisObjectPtr);
  void update() override;
  void render() override;
  int frame() const;

  void changeBkColor(ci_ext::Color newcolor);
  std::weak_ptr<ci_ext::WindowDx> getWindow() const;
};
