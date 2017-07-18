#include "cloud.h"

namespace titleObj
{
	titleObj::Cloud::Cloud(const std::string & resname, float ground, float posx)
		:
		TitleObj("cloud", resname, ci_ext::Vec3f(posx, ground, 0.5f), -524.f, 0.5f)
	{

	}
}