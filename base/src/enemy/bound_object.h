#pragma once
#include "../lib/ci_ext/DrawObj.hpp"
#include "../object/mapCollisionObj.h"
#include "enemyManager.h"
#include <unordered_map >
namespace enemy
{
	class Bound_Object : public map::MapCollisionObj
	{
		//enum Dir{right,left,topRight,topLeft,bottomRight,bottomLeft};
		//std::unordered_map <Dir, ci_ext::Vec3f> dirVelocity;
		std::weak_ptr<enemy::EnemyManager> em;
		RECT offRt_;
		int Rtw_, Rth_; //offRt_�̕��A����
		float speed_;
		float atkPt_; //�U����
		//Dir dir_;
		int rebound_cnt; //���˕Ԃ�����
		const int MAX_REBOUND; //�ő�̒��˕Ԃ��
		const std::string shottype_;
		void vector_Nomalize(ci_ext::Vec3f& vec); //�x�N�g���𐳋K��
		float getVec_Length(ci_ext::Vec3f vec);	//�x�N�g���̒�����Ԃ�
		//void change_DIR();
		void checkHitMap();
		bool checkHitEnemy();
	public:
		Bound_Object(const std::string& objectName,const ci_ext::DrawObjf& obj,const RECT& offRt,/*const std::string shottype,*/ const ci_ext::Vec3f& velocity);
		void init() override;
		void update() override;
		void render() override;
		void rebound();
		void initDir();
	};
}
