#pragma once
#include "easing.hpp"
#include <tuple>
#include <algorithm>
namespace ci_ext
{

template<typename T>
class EaseObj : public ci_ext::Task
{
	std::string name_;//�C�[�W���O��
	T start_;	//�n�_
	T end_;		//�I�_
	T duration_;//�ő厞��
	T elapse_;	//�o�ߎ���
	Ease<T> easing_;//�֐������
	std::weak_ptr<ci_ext::TimeCounter<float>> tm_;

public:
	std::string easeName() const { return name_; }
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
		Task("EaseObjNull"),
		name_(""),
		start_(T()),
		end_(T()),
		duration_(T()),
		elapse_(T()),
		easing_(nullptr)
	{
	}

	virtual ~EaseObj() override {}

	// elapse   = �o�ߎ��� 
	// duration = ��������
	// start    = �n�_
	// end      = �I�_ - �n�_ 
	EaseObj(const std::string& name, T  start, T end, T duration, T elapse = 0.0f)
		:
		Task("EaseObj"),
		name_(name),
		start_(start),
		end_(end),
		duration_(duration),
		elapse_(elapse),
		easing_(name == "" ? (nullptr) : createEase<T>(name))

		//		beginTime_(std::chrono::system_clock::now())
	{
		auto p = std::make_shared<ci_ext::TimeCounter<float>>(name_ + "timecounter_");
		insertChild(p);
		tm_ = p;
	}

	EaseObj(const Object& obj, T elapse = 0.0f)
		:
		EaseObj(obj.name, obj.start, obj.end, obj.duration, elapse)
	{
	}

	//�ʒu���擾
	T getPos()
	{
		elapse_ = clamp<T>(tm_.lock()->get(), 0.0f, duration_);
		// t = �o�ߎ��� d = �������� b = �n�_ c = �I�_ - �n�_ 
		return easing_(elapse_, start_, end_, duration_);
	}
	std::pair<bool, T> getPos2()
	{
		T pos = getPos();
		return std::pair<bool, T>(elapse_ >= duration_, pos);
	}

	//�ʒu���擾
	int getPosi()
	{
		return static_cast<int>(getPos());
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

	virtual void step() override {}
	virtual void draw() override {}
};

inline std::vector<std::string> getLabel()
{
	return{ "easeX", "easeY", "easeZ", "easeScaleX", "easeScaleY", "easeScaleZ", "easeColAlpha", "easeColRed", "easeColGreen", "easeColBlue" };
}
inline std::string getLabel(size_t i)
{
	return getLabel()[i];
}

//-----------------------------------------------------------------------
//�C�[�W���O�I�u�W�F�N�g
//-----------------------------------------------------------------------
class TaskEaseDrawable : public ci_ext::Task
{
	bool noHit_;
	std::unordered_map<std::string, std::shared_ptr<ci_ext::EaseObj<float>>> easeObjs_;

protected:
	DrawObj<float> location_;

	//�����蔻��
	void enableHit()
	{
		noHit_ = false;
	}
	void disableHit()
	{
		noHit_ = true;
	}
	//	TaskEaseDrawable(){}

public:
	virtual ~TaskEaseDrawable() override {}

	TaskEaseDrawable(
		const std::string& name,
		std::unordered_map<std::string, std::shared_ptr<ci_ext::EaseObj<float>>>& easeObjs,
		const Vec3f& pos, const Vec3f& scale, IMGOBJ img,
		const Vec3f& src, const Vec3f& srcSize,
		const Color& color, float degree,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		Task("Drawable_" + name, status, draw),
		easeObjs_(easeObjs),
		location_(pos, scale, img, src, srcSize, color, degree),
		noHit_(noHit)
	{
		for (auto& obj : easeObjs_)
		{
			insertChild(obj.second);
		}
	}
	
/*
	TaskEaseDrawable(
		const std::string& name,
		ci_ext::EaseObj<float> easeX,
		ci_ext::EaseObj<float> easeY,
		const Vec3f& pos, const Vec3f& scale, IMGOBJ img,
		const Vec3f& src, const Vec3f& srcSize,
		const Color& color, float degree,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		Task("Drawable_" + name, status, draw),
		location_(easeX, easeY, pos, scale, img, src, srcSize, color, degree),
		noHit_(noHit)
	{
	}
*/

/*	//DrawObj����ݒ�
	TaskEaseDrawable(
		const std::string& name,
		const DrawEaseObjf& location,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		TaskEaseDrawable(
		name, location.easeX(), location.easeY(),
		location.pos(), location.scale(),
		location.img(),
		location.src(), location.srcSize(),
		location.color(), location.degree(),
		noHit, status, draw)
	{
	}
	*/
	//DrawObj����ݒ�
	TaskEaseDrawable(
		const std::string& name,
		std::unordered_map<std::string, std::shared_ptr<ci_ext::EaseObj<float>>>& easeObjs,
		const DrawObjf& location,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		TaskEaseDrawable(
		name, easeObjs,
		location.pos(), location.scale(),
		location.img(),
		location.src(), location.srcSize(),
		location.color(), location.degree(),
		noHit, status, draw)
	{
	}

	//DrawObj����ݒ�
	TaskEaseDrawable(
		const std::string& name,
		std::unordered_map<std::string, std::shared_ptr<ci_ext::EaseObj<float>>>&& easeObjs,
		const DrawObjf& location,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		TaskEaseDrawable(
		name, easeObjs,
		location.pos(), location.scale(),
		location.img(),
		location.src(), location.srcSize(),
		location.color(), location.degree(),
		noHit, status, draw)
	{
	}
/*	//�摜�t�@�C�����Őݒ�
	TaskEaseDrawable(
		const std::string& name,
		ci_ext::EaseObj<float> easeX,
		ci_ext::EaseObj<float> easeY,
		const Vec3f& pos, const Vec3f& scale,
		const std::string& filename,
		const Vec3f& src, const Vec3f& srcSize,
		const Color& color, float degree,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		TaskEaseDrawable(
		name, easeX, easeY,
		pos, scale,
		Draw_LoadObject(filename),
		src, srcSize,
		color, degree,
		noHit, status, draw)
	{
	}
*/
/*	//�摜�t�@�C�����Ŕ����F���ݒ�
	TaskEaseDrawable(
		const std::string& name,
		ci_ext::EaseObj<float> easeX,
		ci_ext::EaseObj<float> easeY,
		const Vec3f& pos, const Vec3f& scale,
		const std::string& filename,
		const ci_ext::Color& taransparentColor,
		const Vec3f& src, const Vec3f& srcSize,
		const Color& color, float degree,
		bool noHit = false,
		Task::Status status = Task::Status::run,
		Task::DrawStatus draw = Task::DrawStatus::on)
		:
		TaskEaseDrawable(
		name, easeX, easeY,
		pos, scale,
		Draw_LoadObject(filename, taransparentColor),
		src, srcSize,
		color, degree,
		noHit, status, draw)
	{
	}
	*/

	virtual void step() override
	{
		Vec3f pos(location_.pos());
		Vec3f scale(location_.scale());
		Color color(0xFFFFFFFF);
		std::vector<std::string> eraseName;
		for (auto& easeObj : easeObjs_)
		{
			auto obj = easeObj.second->getPos2();

			if (easeObj.first == getLabel(0))
				pos.x(obj.second);
			else if (easeObj.first == getLabel(1))
				pos.y(obj.second);
			else if (easeObj.first == getLabel(2))
				pos.z(obj.second);
			//�X�P�[��
			else if (easeObj.first == getLabel(3))
				scale.x(obj.second);
			else if (easeObj.first == getLabel(4))
				scale.y(obj.second);
			else if (easeObj.first == getLabel(5))
				scale.z(obj.second);
			//�F
			else if (easeObj.first == getLabel(6))
				color.a(static_cast<int>(obj.second));
			else if (easeObj.first == getLabel(7))
				color.r(static_cast<int>(obj.second));
			else if (easeObj.first == getLabel(8))
				color.g(static_cast<int>(obj.second));
			else if (easeObj.first == getLabel(9))
				color.b(static_cast<int>(obj.second));

			if (obj.first)
				eraseName.push_back(easeObj.first);
	//		= ci_ext::EaseObj<float>();
		}
		location_.setPos(pos);
		location_.setScale(scale);
		location_.setColor(color);

		for (auto& name: eraseName)
		{
			easeObjs_.erase(name);
		}

/*
		easeObjs_.erase(std::remove_if(easeObjs_.begin(),
			easeObjs_.end(),
			[](std::unordered_map<std::string, ci_ext::EaseObj<float>>::value_type& obj)
		{
			return (obj.second.easeName() == "");
		}),
			easeObjs_.end()
		);
*/
	}
	virtual void draw() override
	{
		location_.draw();
	}

	virtual RECT getCollisionRect() const
	{
		auto size = location_.halfsize<long>();
		RECT rc =
		{
			location_.pos().ix() - size.x(),
			location_.pos().iy() - size.y(),
			location_.pos().ix() + size.x(),
			location_.pos().iy() + size.y(),
		};
		return rc;
	}

	bool isHit() const
	{
		return !noHit_;
	}

	DrawObjf location() const
	{
		return location_;
	};

	virtual void damage(TaskWeakPtr& obj)
	{
		//	kill();
	}
};
typedef std::shared_ptr<TaskDrawable> TaskDrawablePtr;
typedef std::weak_ptr<TaskDrawable> TaskDrawableWeakPtr;
}

