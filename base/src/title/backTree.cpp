#include "backTree.h"

namespace titleObj
{
	BackTree::BackTree(const std::string& resname, float ground,float posx)
		:
		TitleObj("backTree",resname,ci_ext::Vec3f(posx,ground,0.3f),10.f,2.f)
	{
	}
}