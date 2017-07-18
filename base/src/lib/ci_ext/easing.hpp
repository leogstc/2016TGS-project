#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <unordered_map>
#include <functional>
#include <chrono>
#undef max
#undef min

#include <algorithm>
#include "counter.hpp"



namespace ci_ext
{

// float�̉~����
const float m_pi = static_cast<float>(M_PI);

template <typename T>
using Ease = std::function<T(float, const float, const T&, const T&)>;

// �l�� [min_value, max_value] �ɂ���
template <typename T>
T clamp(const T& value, const T& min_value, const T& max_value)
{
	return std::max(std::min(value, max_value), min_value);
}

//������Ŏw�肵���֐���Ԃ�
// t = �o�ߎ��� b = �n�_ c = �I�_ - �n�_ d = �������� 
//����̓��[�N�Ƃ��Ċ��m�����̂Œ��ӁI
template <typename T>
Ease<T> createEase(const std::string& ease_name)
{
	static const std::unordered_map<std::string, Ease<T>> func_table =
	{
		//{
		//	"LINEAR",
		//	[](float t, const float b, const float c, const float d)
		//	{
		//		t = clamp(t, 0.0f, d);
		//		return c*t / d + b;
		//	}
		//},
		{
			"LINEAR",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return c*t / d + b;
			}
		},

		{
			"BACK_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				float s = 1.70158f;
				t /= d;
				return c*t*t*((s + 1)*t - s) + b;
			}
		},

		{
			"BACK_OUT",
			[](float t, const float b, const float c, const float d)
			{
				float s = 1.70158f;
				t = t / d - 1;
				return c*(t*t*((s + 1)*t + s) + 1) + b;
			}
		},

		{
			"BACK_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				float s = 1.70158f;
				t = t / d - 1;
				return c*(t*t*((s + 1)*t + s) + 1) + b;
			}
		},
		
		{
			"BOUNCE_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d) < (1 / 2.75f)) {
					return c*(7.5625f*t*t) + b;
				}
				else if (t < (2 / 2.75f)) {
					t -= (1.5f / 2.75f);
					return c*(7.5625f*t*t + .75f) + b;
				}
				else if (t < (2.5f / 2.75f)) {
					t -= (2.25f / 2.75f);
					return c*(7.5625f*t*t + .9375f) + b;
				}
				else {
					t -= (2.625f / 2.75f);
					return c*(7.5625f*t*t + .984375f) + b;
				}
			}
		},
		
		{
			"BOUNCE_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return func_table.at("BOUNCE_OUT")(d - t, 0.0f, c, d) + b;
				//return c - EasingBounceOut(d - t, 0.0f, c, d) + b;
			}
		},
		
		{
			"BOUNCE_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if (t < d / 2) return func_table.at("BOUNCE_IN")(t * 2, 0.0f, c, d) * .5f + b;
				else					 return func_table.at("BOUNCE_OUT")(t * 2 - d, 0.0f, c, d) * .5f + c*.5f + b;
			}
		},

		{
			"CIRCLE_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return -c * (std::sqrt(1 - t*t) - 1) + b;
			}
		},

		{
			"CIRCLE_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t = t / d - 1;
				return c * std::sqrt(1 - t*t) + b;
			}
		},

		{
			"CIRCLE_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d / 2) < 1) return -c / 2 * (std::sqrt(1 - t*t) - 1) + b;
				t -= 2;
				return c / 2 * (std::sqrt(1 - t*t) + 1) + b;
			}
		},

		{
			"CUBIC_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return c*t*t*t + b;
			}
		},

		{
			"CUBIC_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t = t / d - 1;
				return c*(t*t*t + 1) + b;
			}
		},

		{
			"CUBIC_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d / 2) < 1) return c / 2 * t*t*t + b;
				t -= 2;
				return c / 2 * (t*t*t + 2) + b;
			}
		},

		{
			"ELASTIC_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if (t == 0)      return b;
				if ((t /= d) == 1) return b + c;
				float p = d*0.3f;

				float a = c;
				float s = p / 4;
				t -= 1;
				return -(a * std::pow(2.0f, 10.0f*t) * std::sin((t*d - s)*(2.0f * m_pi) / p)) + b;
			}
		},

		{
			"ELASTIC_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if (t == 0)      return b;
				if ((t /= d) == 1) return b + c;
				float p = d*0.3f;

				float a = c;
				float s = p / 4;
				return (a* std::pow(2.0f, -10.0f*t) * std::sin((t*d - s)*(2.0f * m_pi) / p) + c + b);
			}
		},

		{
			"ELASTIC_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if (t == 0) return b;
				if ((t /= d / 2) == 2) return b + c;
				float p = d*(0.3f*1.5f);

				float a = c;
				float s = p / 4;
				if (t < 1)
				{
					t -= 1;
					return -0.5f*(a * std::pow(2.0f, 10.0f*t) * std::sin((t*d - s)*(2.0f * m_pi) / p)) + b;
				}
				t -= 1;
				return a * std::pow(2.0f, -10.0f*t) * std::sin((t*d - s)*(2.0f * m_pi) / p)*0.5f + c + b;
			}
		},

		{
			"EXPO_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return (t == 0) ? b : c * std::pow(2.0f, 10.0f * (t / d - 1.0f)) + b;
			}
		},

		{
			"EXPO_OUT",
			[](float t, const float b, const float c, const float d)
			{
				return (t == d) ? b + c : c * (-std::pow(2.0f, -10.0f * t / d) + 1.0f) + b;
			}
		},

		{
			"EXPO_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if (t == 0)         return b;
				if (t == d)         return b + c;
				if ((t /= d / 2) < 1) return c / 2 * std::pow(2.0f, 10.0f * (t - 1.0f)) + b;
				return c / 2 * (-std::pow(2.0f, -10.0f * --t) + 2.0f) + b;
			}
		},

		{
			"QUAD_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return c*t*t + b;
			}
		},

		{
			"QUAD_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return -c *t*(t - 2) + b;
			}
		},

		{
			"QUAD_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d / 2) < 1) return c / 2 * t*t + b;
				--t;
				return -c / 2 * (t*(t - 2) - 1) + b;
			}
		},

		{
			"QUART_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return c*t*t*t*t + b;
			}
		},

		{
			"QUART_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t = t / d - 1;
				return -c * (t*t*t*t - 1) + b;
			}
		},

		{
			"QUART_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d / 2) < 1) return c / 2 * t*t*t*t + b;
				t -= 2;
				return -c / 2 * (t*t*t*t - 2) + b;
			}
		},

		{
			"QUINT_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t /= d;
				return c*t*t*t*t*t + b;
			}
		},

		{
			"QUINT_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				t = t / d - 1;
				return c*(t*t*t*t*t + 1) + b;
			}
		},

		{
			"QUINT_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);

				if ((t /= d / 2) < 1) return c / 2 * t*t*t*t*t + b;
				t -= 2;
				return c / 2 * (t*t*t*t*t + 2) + b;
			}
		},

		{
			"SINE_IN",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return -c * std::cos(t / d * (m_pi / 2)) + c + b;
			}
		},

		{
			"SINE_OUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return c * std::sin(t / d * (m_pi / 2)) + b;
			}
		},

		{
			"SINE_INOUT",
			[](float t, const float b, const float c, const float d)
			{
				t = clamp(t, 0.0f, d);
				return -c / 2 * (std::cos(m_pi*t / d) - 1) + b;
			}
		}
	};
	return func_table.at(ease_name);
}
/*
template<typename T>
class EaseObj
{
	std::string name_;//�C�[�W���O��
	T start_;	//�n�_
	T end_;		//�I�_
	T duration_;//�ő厞��
	T elapse_;	//�o�ߎ���
	Ease<T> easing_;//�֐������
	std::chrono::system_clock::time_point  beginTime_;//�J�n���ԕۑ�

public:
	struct Object2
	{
		int i;
		float j;
	};
	struct Object
	{
		std::string name;//�C�[�W���O��
		T start;	//�n�_
		T end;		//�I�_
		T duration;//�ő厞��
	};
	//���g�p���͂��̃R���X�g���N�^���g��
	EaseObj() 
		:
		name_(""),
		start_(T()),
		end_(T()),
		duration_(T()),
		elapse_(T()),
		easing_(nullptr)
	{
	}

	// elapse   = �o�ߎ��� 
	// duration = ��������
	// start    = �n�_
	// end      = �I�_ - �n�_ 
	EaseObj(const std::string& name, T  start, T end, T duration, T elapse = 0.0f)
		:
		name_(name),
		start_(start),
		end_(end),
		duration_(duration),
		elapse_(elapse),
		easing_(name == "" ? (nullptr): createEase<T>(name)),

		beginTime_(std::chrono::system_clock::now())
	{
	}

	EaseObj(const Object& obj, T elapse = 0.0f)
		:
		EaseObj(obj.name, obj.start, obj.end, obj.duration, elapse)
	{
	}

	//����i�߂�(�����J�E���^��1�𑫂��A�t���[���Ő��䂵�Ă���Ƃ��Ɏg��)
	T updateFrame()
	{
		++elapse_;
		return getPos();
	}

	//����i�߂�(�����J�E���^�𐶐�����Ă���̌o�ߎ��Ԃɂ���)
	T updateTime()
	{
//		elapse_ = std::chrono::system_clock::now() - beginTime_;
		auto a = std::chrono::system_clock::now() - beginTime_;
		elapse_ = a.count() / 10000000.0f;
		return getPos();
	}

	//�ʒu���擾
	T getPos()
	{
		clamp<T>(elapse_, 0.0f, duration_);
		// t = �o�ߎ��� d = �������� b = �n�_ c = �I�_ - �n�_ 
		return easing_(elapse_, start_, end_, duration_);
	}

	T operator()()
	{
		return getPos();
	}

	//�C�[�W���O�֐����ݒ肳��Ă���Ƃ�true���Ԃ�
	bool valid() const
	{
		return easing_ != nullptr;
	}
};
*/
}