#pragma once
#include "titleObj.h"

namespace titleObj
{
	class Tree :public TitleObj
	{
		float ground_;
	public:
		Tree(const std::string& resname,float ground);
	};
}