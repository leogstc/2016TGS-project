#pragma once

#include <vector>
namespace ci_ext
{


	/*
	//‚Â‚©‚Á‚Ä‚È‚¢‚æ
	union ColorU
	{
	unsigned int val;
	struct
	{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
	};
	};
	*/

	/*
	template<typename T>
	unsigned int MakeColor(T a, T r, T g, T b)
	{
		int color = ((a & 0xFF) << 24) |
			((r & 0xFF) << 16) |
			((g & 0xFF) << 8) |
			(b & 0xff);
		return color;
	}
	*/

	template<typename T>
	unsigned int GetAlpha(T color)
	{
		return (color >> 24) & 0xFF;
	}

	template<typename T>
	unsigned int GetRed(T color)
	{
		return (color >> 16) & 0xFF;
	}

	template<typename T>
	unsigned int GetGreen(T color)
	{
		return (color >> 8) & 0xFF;
	}

	template<typename T>
	unsigned int GetBlue(T color)
	{
		return color & 0xFF;
	}

	inline unsigned int MakeColor(int a, int r, int g, int b)
	{
		int color = ((a & 0xFF) << 24) |
			((r & 0xFF) << 16) |
			((g & 0xFF) << 8) |
			(b & 0xff);
		return color;
	}

	inline unsigned int MakeColor(float a, float r, float g, float b)
	{
		int color = 
			(int(255.0f * a) << 24) |
			(int(255.0f * r) << 16) |
			(int(255.0f * g) << 8) |
			 int(255.0f * b);
		return color;
	}

	class Color
	{
		struct Type
		{
			float r_, g_, b_, a_;
		}v;
//		float a_, r_, g_, b_;
		Color() {}

	public:
		Color(const unsigned int val)
		{
			set(val);
		}

		Color(int a, int r, int g, int b)
		{
			seti(a, r, g, b);
		}

		Color(float a, float r, float g, float b)
		{
			setf(a, r, g, b);
		}

    unsigned int val() const { return MakeColor(v.a_, v.r_, v.g_, v.b_); }

		operator int()           const { return val(); }
		operator unsigned int()  const { return val(); }
		operator long()          const { return val(); }
		operator unsigned long() const { return val(); }
   operator float*()
    {
      return reinterpret_cast<float*>(&v);
    }

    float af() const { return v.a_; }
    float rf() const { return v.r_; }
    float gf() const { return v.g_; }
    float bf() const { return v.b_; }
    unsigned int ai() const { return static_cast<unsigned int>(v.a_ * 255.0f); }
    unsigned int ri() const { return static_cast<unsigned int>(v.r_ * 255.0f); }
    unsigned int gi() const { return static_cast<unsigned int>(v.g_ * 255.0f); }
    unsigned int bi() const { return static_cast<unsigned int>(v.b_ * 255.0f); }
    void ai(unsigned int a) { v.a_ = (a & 0xFF) / 255.0f; }
    void ri(unsigned int r) { v.r_ = (r & 0xFF) / 255.0f; }
    void gi(unsigned int g) { v.g_ = (g & 0xFF) / 255.0f; }
    void bi(unsigned int b) { v.b_ = (b & 0xFF) / 255.0f; }
    void af(float a) { v.a_ = a; }
    void rf(float r) { v.r_ = r; }
    void gf(float g) { v.g_ = g; }
    void bf(float b) { v.b_ = b; }
    void offsetai(int a) { v.a_ = offseti(static_cast<int>(v.a_ * 255.0f), a) / 255.0f; }
    void offsetri(int r) { v.r_ = offseti(static_cast<int>(v.r_ * 255.0f), r) / 255.0f; }
    void offsetgi(int g) { v.g_ = offseti(static_cast<int>(v.g_ * 255.0f), g) / 255.0f; }
    void offsetbi(int b) { v.b_ = offseti(static_cast<int>(v.b_ * 255.0f), b) / 255.0f; }
    void offsetaf(float a) { v.a_ = offsetf(v.a_, a); }
    void offsetrf(float r) { v.r_ = offsetf(v.r_, r); }
    void offsetgf(float g) { v.g_ = offsetf(v.g_, g); }
    void offsetbf(float b) { v.b_ = offsetf(v.b_, b); }

		Color::Type get()
		{
			Color::Type ret = { rf(), gf(), bf(), af()};
			return ret;
		}
		void get(std::vector<float>& value)
		{
			value.push_back(rf());
			value.push_back(gf());
			value.push_back(bf());
			value.push_back(af());
		}
		void set(unsigned int val)
		{
			ai(GetAlpha(val));
			ri(GetRed(val));
			gi(GetGreen(val));
			bi(GetBlue(val));
		}
		void offset(int a, int r, int g, int b)
		{
			offsetai(a);
			offsetri(r);
			offsetgi(g);
			offsetbi(b);
		}
		void offset(float a, float r, float g, float b)
		{
			offsetaf(a);
			offsetrf(r);
			offsetgf(g);
			offsetbf(b);
		}
		void seti(int a, int r, int g, int b)
		{
      v.a_ = a / 2550.f;
      v.r_ = r / 2550.f;
      v.g_ = g / 2550.f;
      v.b_ = b / 2550.f;
		}
		void setf(float a, float r, float g, float b)
		{
      v.a_ = a;
      v.r_ = r;
      v.g_ = g;
      v.b_ = b;
		}

		Color& operator=(unsigned int color)
		{
			set(color);
			return *this;
		}

		int offseti(int color, int offset)
		{
			color += offset;
			if (color >= 256)   color = 255;
			else if (color < 0) color = 0;
			return color;
		}
		float offsetf(float color, float offset)
		{
			color += offset;
			if (color >= 1.0f)   color = 1.0f;
			else if (color < 0) color = 0;
			return color;
		}
		static int makeARGB(int a, int r, int g, int b)
		{
			return MakeColor(a, r, g, b);
		}
		static int makeRGBA(int r, int g, int b, int a)
		{
			return MakeColor(a, r, g, b);
		}

	};


}