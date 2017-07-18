#pragma once
#include "titleObj.h"

namespace titleObj
{
	class BackTree :public TitleObj
	{
	public:
		BackTree(const std::string& resname, float ground,float posx);
	};
}