#pragma once
#include "vec2.hpp"
#include "vec3.hpp"
#include "color.hpp"
#include "../gplib.h"
#include <string>

namespace ci_ext
{

//描画オブジェクト
template<typename T>
class DrawObj
{
  Vec3<T> pos_;
  Vec2<T> scale_;
  std::string resname_;
  Vec2<T> src_;
  Vec2<T> srcSize_;
  Color color_;
  T degree_;
  bool draw_;
public:
  virtual ~DrawObj() {}
  DrawObj() = delete;

  //リソース名から
  DrawObj(
    const Vec3<T>& pos, const Vec2<T>& scale,
    const std::string& resname,
    const Vec2<T>& src, const Vec2<T>& srcSize,
    const Color& color, const T degree)
    :
    pos_(pos),
    scale_(scale),
    resname_(resname),
    src_(src),
    srcSize_(srcSize),
    color_(color),
    degree_(degree),
    draw_(true)
  {
    if (srcSize_.ix() == 0 && srcSize_.iy() == 0)
      adjustSrcSize();
  }

  //ファイル名から
  DrawObj(
    const Vec3<T>& pos, const Vec2<T>& scale,
    const std::string& resname,
    const std::string& filename,
    const Vec2<T>& src, const Vec2<T>& srcSize,
    const Color& color, const T degree)
    :
    pos_(pos),
    scale_(scale),
    resname_(resname),
    src_(src),
    srcSize_(srcSize),
    color_(color),
    degree_(degree),
    draw_(true)
  {
    gplib::draw::LoadObject(resname, filename);
    if (srcSize_.ix() == 0 && srcSize_.iy() == 0)
      adjustSrcSize();
  }

  void adjustSrcSize()
  {
    auto size = gplib::draw::GetImageSize2(resname_);
    setSrcSize(ci_ext::Vec2f(size.cx, size.cy));
  }

  void invisible() { draw_ = false; }
  void visible()   { draw_ = true; }
  void changeVisible(bool f) { draw_ = f; }

  void render() const
  {
    if (!draw_) return;

    gplib::draw::Graph(
      pos_.ix(), pos_.iy(), pos_.z(),
      resname_,
      src_.ix(), src_.iy(),
      srcSize_.ix(), srcSize_.iy(),
      degree_,
      scale_.x(), scale_.y(),
      color_.a(), color_.r(), color_.g(), color_.b());
  }
  void renderLeftTop() const
  {
	  if (!draw_) return;

	  gplib::draw::GraphLeftTop(
		  pos_.ix(), pos_.iy(), pos_.z(),
		  resname_,
		  src_.ix(), src_.iy(),
		  srcSize_.ix(), srcSize_.iy(),
		  degree_, nullptr,
		  scale_.x(), scale_.y(),
		  color_.a(), color_.r(), color_.g(), color_.b());
  }
  void renderNC() const
  {
	  if (!draw_) return;

	  gplib::draw::GraphNC(
		  pos_.ix(), pos_.iy(), pos_.z(),
		  resname_,
		  src_.ix(), src_.iy(),
		  srcSize_.ix(), srcSize_.iy(),
		  degree_,
		  scale_.x(), scale_.y(),
		  color_.a(), color_.r(), color_.g(), color_.b());
  }
  void renderLeftTopNC() const
  {
	  if (!draw_) return;

	  gplib::draw::GraphLeftTopNC(
		  pos_.ix(), pos_.iy(), pos_.z(),
		  resname_,
		  src_.ix(), src_.iy(),
		  srcSize_.ix(), srcSize_.iy(),
		  degree_, nullptr,
		  scale_.x(), scale_.y(),
		  color_.a(), color_.r(), color_.g(), color_.b());
  }

  void setResname(const std::string& resname)
  {
	  resname_ = resname;
  }

  void setPos(const Vec3<T>& pos)
  {
    pos_ = pos;
  }

  void setPos(const T x, const T y, const T z)
  {
    pos_ = Vec3<T>(x, y, z);
  }

  void offsetPos(const Vec3<T>& offset)
  {
    pos_ += offset;
  }

  void setScale(const Vec2<T>& scale)
  {
    scale_ = scale;
  }

  void offsetScale(const Vec2<T>& offset)
  {
    scale_ += offset;
  }

  void setSrc(const Vec2<T>& src)
  {
    src_ = src;
  }

  void setSrcSize(const Vec2<T>& size)
  {
    srcSize_ = size;
  }

  void setSrcInfo(const Vec2<T>& src, const Vec2<T>& size)
  {
    src_ = src;
    srcSize_ = size;
  }

  void setDegree(const T degree)
  {
    degree_ = degree;
  }

  void offsetDegree(const T offset)
  {
    T temp0 = static_cast<T>(0);
    T temp360 = static_cast<T>(360);
    degree_ += offset;
    if (degree_ > temp360)    degree_ -= temp360;
    else if (degree_ < temp0) degree_ += temp360;
  }

  void setColor(const Color& color)
  {
    color_ = color;
  }
  void offsetColor(const Color& offset)
  {
    color_ += offset;
  }
  void setColor(
    unsigned int a, unsigned int r,
    unsigned int g, unsigned int b)
  {
    color_.set(a, r, g, b);
  }
  void offsetColor(
    unsigned int a, unsigned int r,
    unsigned int g, unsigned int b)
  {
    color_.offset(a, r, g, b);
  }

  template<typename T1>
  Vec2<T1> halfsize() const
  {
    return Vec2<T1>(
      static_cast<T1>(scale().x() * srcSize().x() / 2),
      static_cast<T1>(scale().y() * srcSize().y() / 2)
      );
  }
  template<typename T1>
  Vec2<T1> size() const
  {
    return Vec2<T1>(
      static_cast<T1>(scale().x() * srcSize().x()),
      static_cast<T1>(scale().y() * srcSize().y())
      );
  }
  RECT makeDrawRect() const
  {
    auto size = halfsize<long>();
    RECT rc =
    {
      pos().ix() - size.x(),
      pos().iy() - size.y(),
      pos().ix() + size.x(),
      pos().iy() + size.y(),
    };
    return rc;
  }
  Vec3<T> pos()     const { return pos_; }
  Vec2<T> scale()   const { return scale_; }
  Vec2<T> src()     const { return src_; }
  Vec2<T> srcSize() const { return srcSize_; }
  T degree()        const { return degree_; }
  Color color()     const { return color_; }
  T x()             const { return pos_.x(); }
  T y()             const { return pos_.y(); }
  T z()             const { return pos_.z(); }
  int ix()          const { return pos_.ix(); }
  int iy()          const { return pos_.iy(); }
  int iz()          const { return pos_.iz(); }
  DrawObj obj()     const { return *this; }
  int idegree()     const { return static_cast<int>(degree_); }
  std::string resname() const { return resname_; }
//オフセット値考慮の矩形生成
  RECT makeDrawRect(int startx,int starty,int endx,int endy) const
  {
    auto size = halfsize<long>();
    RECT rc =
    {
      pos().ix() - size.x() + startx,
      pos().iy() - size.y() + starty,
      pos().ix() + size.x() - endx,
      pos().iy() + size.y() - endy,
    };
    return rc;
  }

};
//typedef DrawObj<float> DrawObjf;
using DrawObjf = DrawObj<float>;

}