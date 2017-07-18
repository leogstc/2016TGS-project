#pragma once

#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "../stage/stageInfo.h" //WindDIR

namespace object {
	class WindBase : public map::MapCollisionObj {
	protected:
		anim::Anim anim_;
		map::info::WindDIR dir_;
		float speed_;
		RECT offsetRt_;

	protected:
		//virtual void setupMove();
		void animUpdate();

	public:
		WindBase(int x, int y, map::info::WindDIR dir, float speed = 0.0f);
		virtual ~WindBase() {}

		void init() override;
		void update() override;
		void render() override;

		map::info::WindDIR getWindDir() const;
		ci_ext::Vec3f getWindForce() const;
	};

	//Ç‘Ç¬Ç©ÇÈÇ‹Ç≈ìÆÇ≠ÅAÇ‘Ç¬Ç©ÇÈÇ∆îΩëŒë§Ç…
	class WindMoveX : public WindBase {
	public:
		WindMoveX(int x, int y, map::info::WindDIR dir, float speed);
		void init() override;
		void update() override;
	};

	class WindMoveY : public WindBase {
	public:
		WindMoveY(int x, int y, map::info::WindDIR dir, float speed);
		void init() override;
		void update() override;
	};
	//--------------------------------------------------

	//îÕàÕì‡Ç…à⁄ìÆ
	class WindMoveRangeX : public WindBase {
		int moveFrame_;
		int moveCnt_;
	public:
		WindMoveRangeX(int x, int y, map::info::WindDIR dir, float speed, int moveFrame);
		void init() override;
		void update() override;
	};

	class WindMoveRangeY : public WindBase {
		int moveFrame_;
		int moveCnt_;
	public:
		WindMoveRangeY(int x, int y, map::info::WindDIR dir, float speed, int moveFrame);
		void init() override;
		void update() override;
	};

	
}