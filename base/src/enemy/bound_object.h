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
		int Rtw_, Rth_; //offRt_の幅、高さ
		float speed_;
		float atkPt_; //攻撃力
		//Dir dir_;
		int rebound_cnt; //跳ね返った回数
		const int MAX_REBOUND; //最大の跳ね返る回数
		const std::string shottype_;
		void vector_Nomalize(ci_ext::Vec3f& vec); //ベクトルを正規化
		float getVec_Length(ci_ext::Vec3f vec);	//ベクトルの長さを返す
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
