#pragma once

#include "effectBase.h"

namespace effect {
	class EffectClear : public EffectBase {
		enum {
			SIZE = 5,
			HALF = SIZE / 2,
		};

		struct Char {
			bool flag; //On or Off
			float alpha; //Alpha
			float scale; //Scale X & Y
			bool flipScale; //Small -> Big -> Small
			bool stopScale; //Stop scaling
			int i; //Index & Position Calc
			int cnt;
		};

		Char eff_[SIZE];
		const std::string resname_;
		const int sizex_, sizey_;
		int i_; //Index & Position Calc
		bool dispFullText;
		bool finalFlag;

	public:
		EffectClear(int x, int y);
		void init() override;
		void update() override;
		//void render() override;
		void renderLater() override;

	private:
		void createEffects(int no);
		void createText();
	};
}