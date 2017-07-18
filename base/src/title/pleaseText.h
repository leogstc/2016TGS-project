#pragma once
#include "titleObj.h"

namespace titleObj
{
	class TitleText :public TitleObj
	{
		bool show;
		int cnt;
		const int MAX_CNT_;
	public:
		TitleText(const int& MAX_CNT);
		void update() override;
		void render() override;
	};
}
