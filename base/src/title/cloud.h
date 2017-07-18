#pragma once
#include "titleObj.h"
namespace titleObj
{
	class Cloud : public TitleObj
	{
	public:
		Cloud(const std::string& resname, float ground, float posx);
	};
}