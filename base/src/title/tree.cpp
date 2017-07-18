#include "tree.h"
namespace titleObj
{
	using namespace ci_ext;
	using namespace gplib;
	Tree::Tree(const std::string& resname,float ground)
		:
		TitleObj("tree",resname,Vec3f(system::WINW, ground,0.1f),2.f,3.f)
	{
	}
}