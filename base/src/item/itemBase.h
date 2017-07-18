#pragma once

#include "../object/mapCollisionObj.h"
#include "../object/animation.hpp"
#include "../player/player.h"

namespace item {
	class ItemBase : public map::MapCollisionObj {
	protected:
		int id_;
		int uid_;
		anim::Anim anim_;
		RECT offsetRt_;
		std::weak_ptr<player::Player> player_;

	public:
		ItemBase(const std::string& objectName, const std::string& resname, int srcY, int x, int y, int id, int uniqueId, RECT offsetRt = RECT());
		virtual ~ItemBase() {}

		void init() override;
		void update() override;
		void render() override;

	private:
		inline void getPlayerPtr();
		inline bool checkHitPlayer();

	protected:
		//�A�j���[�V����Setup
		virtual void setupAnim();

		//�A�C�e������������̏���
		//�p�������N���X��override�K�{
		virtual void getItem() = 0;

	};
}