#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <Windows.h>
#include <vector>
#include "math.h"
namespace ci_ext
{

template<typename T = float>
class Vec2
{
  T x_, y_;

public:
  static Vec2<T> one()
  {
    return Vec2<T>(1,1);
  }
  static Vec2<T> zero()
  {
    return Vec2<T>(0, 0);
  }

  void x(T x) { x_ = x; }
  void y(T y) { y_ = y; }
  void z(T z) { z_ = z; }
  
  template<typename T1, typename T2>
  Vec2(T1 x, T2 y)
    :
    x_(static_cast<T>(x)),
    y_(static_cast<T>(y))
  {}

  Vec2(T x = 0.0f, T y = 0.0f)
    :
    x_(x),
    y_(y)
    {}
  Vec2(const Vec2<T>& v) 
    :
    x_(v.x()),
    y_(v.y())
  {}

  Vec2(const std::vector<float>& v)
	{
		set(v);
	}
	~Vec2() {}
  T x() const { return x_; }
  T y() const { return y_; }

  int ix() const { return static_cast<int>(x_); }
  int iy() const { return static_cast<int>(y_); }

  Vec2<T>& offset(T xx, T yy)
  {
    x(x() + xx);
    y(y() + yy);
    return *this;
  }

  Vec2<T>& offset(const Vec2<T>& p)
  {
    x(x() + p.x());
    y(y() + p.y());
    return *this;
  }

  Vec2<T>& set(const Vec2<T>& p)
  {
    x(p.x());
    y(p.y());
    return *this;
  }
  Vec2<T>& set(T xx = 0.0f, T yy = 0.0f)
  {
    x(xx);
    y(yy);
    return *this;
  }

	Vec2<T>& set(const std::vector<float>& v)
	{
		x_ = y_ = T(0);
		if (v.size() >= 1)
			x_ = v[0];
		if (v.size() >= 2)
			y_ = v[1];
		return *this;
	}

  operator POINT() 
  {
    POINT p = {static_cast<long>(x_), static_cast<long>(y_)};
    return p;
  }

  Vec2<T>& operator=(const Vec2<T>& v)
  {
    set(v);
    return *this;
  }

  Vec2<T>& operator=(const POINT& p)
  {
    set(static_cast<T>(p.x), static_cast<T>(p.y));
    return *this;
  }

  Vec2<T>& operator+=(const Vec2<T>& p)
  {
    x(x() + p.x());
    y(y() + p.y());
    return *this;
  }

  Vec2<T> operator+(const Vec2<T>& p) const
  {
    return Vec2<T>(x() + p.x(), y() + p.y());
  }

  Vec2<T>& operator-=(const Vec2<T>& p)
  {
    x(x() - p.x());
    y(y() - p.y());
    return *this;
  }

  Vec2<T> operator-(const Vec2<T>& p) const
  {
    return Vec2<T>(x() - p.x(), y() - p.y());
  }
  bool operator==(const Vec2<T>& p)
  {
    return (x() == p.x() && y() == p.y());
  }
  bool operator==(T c)
  {
    return (x() == c && y() == c);
  }
  Vec2<T>& translate(const Vec2<T>& translate)
  {
    x( x() + translate.x());
    y( y() + translate.y());
    return *this;
  }
  Vec2<T>& rotate(T radian)
  {
    T xx = x();
    T yy = y();
    x( xx * std::cos(-radian) - yy * std::sin(-radian));
    y( xx * std::sin(-radian) + yy * std::cos(-radian));
    return *this;
  }
  Vec2<T>& rotateDegree(T degree)
  {
    rotate(DegreeToRadian(degree));
    return *this;
  }
  Vec2<T>& scale(const Vec2<T>& scale)
  {
    x(x() * scale.x());
    y(y() * scale.y());
    return *this;
  }
  static Vec2<T>* null() { return static_cast<Vec2<T>*>(nullptr); }
};

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

}