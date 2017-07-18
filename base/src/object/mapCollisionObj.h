#pragma once

#include "movableObject.hpp"

namespace map {
	//マップとの判定を行う。
	//移動値に応じてプレイヤーを停止させる
	class MapCollisionObj : public game::MovableObject
	{
	private:
		//判定のオフセット
		//Offset Start (LeftTop) / End (RightBottom)
		int offsetS_, offsetE_;
		//判定用矩形
		RECT objRect_;

		//Check Movement <-- Obj移動によってチェック
		int checkMoveMapLeft();
		int checkMoveMapRight();
		int checkMoveMapDown();
		int checkMoveMapUp();
		bool checkCollisionMapLeft(); //
		bool checkCollisionMapRight(); //
		bool checkCollisionMapUp(); //
		bool checkCollisionMapDown(); //
	protected:
		int checkMoveMapX();
		int checkMoveMapY();
		bool checkCollisionMapX(); //
		bool checkCollisionMapY(); //
		//Objの状態により、常にチェック
		bool checkUnderFloor();
		bool checkHeadFloor();

		//Objの移動によりチェック
		//checkMoveMapOOOO系
		void mapCheckMoveXY();

	private:
		//Check Collision <-- Objの判定矩形が突然変わる場合
		//引数： x = Offset X | y = Offset Y | opposite = 反対側をチェックしていた？
		//Collisionの距離を返す
		//Bottomチェックだけは特別CustomRect
		int collisionMapLeft(int x = 0, int y = 0, bool opposite = false);
		int collisionMapRight(int x = 0, int y = 0, bool opposite = false);
		int collisionMapTop(int x = 0, int y = 0, bool opposite = false);
		int collisionMapBottom(int x = 0, int y = 0, bool opposite = false); //特別

	protected:
		//プレイヤー関係
		bool isPlayer_; //このオブジェクトがプレイヤーか？
		std::string nowInMapName_; //現在にいるマップ名
		bool invincible_; //無敵状態 <- ダメージを受けた時・一定時間
		int invincibleCnt_; //無敵状態カウンター

		//Objの判定矩形が突然変わる場合
		bool mapCheckCollisionX(bool adjust = false);
		bool mapCheckCollisionY(bool adjust = false);
		bool mapCheckCollision4Way(bool adjust = false);

		//Objの判定RECTをセット
		void setObjCollisionMapRect(int offsetSx = 0, int offsetSy = 0, int offsetEx = 0, int offsetEy = 0);
		void setObjCollisionMapRect(const RECT& rect, bool isOffsetRect = false); //Offsetならboolの引数はtrueにセットして
		RECT getObjCollisionMapRect();

		//RECT Making : 矩形の生成
		//Objの現座標に計算
		RECT makeRectScale(int startx = 0, int starty = 0, int endx = 0, int endy = 0);
		RECT makeRectNoScale(int startx = 0, int starty = 0, int endx = 0, int endy = 0);

	public:
		//object名 -> getObjectFromOOOに使う
		//drawObj -> render可能なObj
		//velocity -> 初期移動量
		//isPlayer -> このObjがプレイヤーか？ <-- プレイヤー以外はtrueを設定しないで！　（デフォルトはfalse）
		MapCollisionObj(const std::string& objectName, const ci_ext::DrawObjf& drawObj, const ci_ext::Vec3f& velocity, bool isPlayer = false);
		virtual ~MapCollisionObj() {}

		//継承元（親）のクラスをoverride -> 何もしない
		void update() override {}
		void render() override {}

	protected:
		//----- 継承したクラスでoverrideして -----
		//1フレームしか処理しない

		//ダメージタイルを触る瞬間の処理
		virtual void doOnceHitDamageTile() {}
		//即死タイルを触る瞬間の処理
		virtual void doOnceHitDeadTile() {}

	private:
		//Wind
		bool windBlowX_; //風に飛ばされる？
		bool windBlowY_;
		int windBlowXCnt_; //カウンター
		int windBlowYCnt_;
		ci_ext::Vec3f windBlowVelocity_;
		int windDir_;
		bool isHitWind(const POINT& pt);
		bool isHitWind(const RECT& rt);
		void resetWindBlow();
	protected:
		//風に触る瞬間の処理
		virtual void doOnceHitWindObj();

	private:
		//Debug
		void drawHitRect(int x, int y, D3DCOLOR c = ARGB(255, 255, 255, 0));
		void drawHitPt(int x, int y, D3DCOLOR c = ARGB(255, 255, 255, 0));

	};
}
