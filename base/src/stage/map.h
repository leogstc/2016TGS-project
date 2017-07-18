#pragma once

#include "../lib/ci_ext/object.hpp"
#include "../lib/gplib.h"
#include "../lib/ci_ext/vec2.hpp"
#include "../lib/ci_ext/vec3.hpp"
#include "../util/timer.h"
#include "../config.h"
#include "../gameFlag.hpp"
#include "stageFlag.hpp"
#include "stageInfo.h"
#include "camera.h" //StageCamera
#include "../object/animation.hpp"

#include <vector>
#include <map>

//Entities in Map
#include "../map_object/windManager.h"
#include "../item/itemManager.h"
#include "../enemy/enemyManager.h"
#include "../effect/effectManager.h"
#include "../event/stageEventManager.h"
#include "../ui/uiManager.h"

namespace map {
	namespace sprite {
		enum {
			SPRITE_SIZE = 64,
			SPRITE_OFFSET = 2,

			TILE_SIZE = SPRITE_SIZE - (SPRITE_OFFSET * 2),
			HALF_TILE_SIZE = TILE_SIZE / 2,
		};
	}

	//指定した座標が指定したRECT内にいるか？
	bool isPointInRect(POINT p, RECT rt);

	class Map : public ci_ext::Object {
		enum DIR { //Jointに使う
			NONE,
			LEFT,
			RIGHT,
			UP,
			DOWN,
		};

		struct JointGoing {
			RECT rt;
			DIR dir;
			std::string nextName;
			float nextBoffset;
			info::JointDoorType doorType;
			int doorId;

			JointGoing() {
				doorId = -1; //Not door
			}
		};

		//ステージの情報
		StageInfo info_;

		//ゲーム全体のフラグ
		std::weak_ptr<game::GameFlag> gameFlag_;
		//ステージフラグ
		std::weak_ptr<StageFlag> stageFlag_;
		//風管理
		std::weak_ptr<object::WindManager> windManager_;
		//エフェクト管理
		std::weak_ptr<effect::EffectManager> effectManager_;
		//アイテム管理
		std::weak_ptr<item::ItemManager> itemManager_;
		//敵管理
		std::weak_ptr<enemy::EnemyManager> enemyManager_;
		//イベント管理
		std::weak_ptr<gameevent::StageEventManager> eventManager_;
		//UI管理
		std::weak_ptr<BOMB::uiManager> uiManager_;
		//Timer
		std::weak_ptr<game::Timer> timer_;


		//カメラ
		camera::StageCamera stageCamera_;

		//ステージ名
		std::string stageName_;
		//マップ名
		std::string nowMapName_;

		//登場・復活
		POINT playerEntryPos_; //位置
		bool playerEntry_; //フラグ

		//描画関係
		int tileSizeX_;
		//int tileSizeY_; // %と/を使うので、Y(Height)は要らない
		int readyPosY_ = gplib::system::WINH / 2;

		//ファイルから読んだ全体ステージの情報を修正
		void finalizeLoadStage();

		//現在のマップIterator
		void setNowMapIterator(); //現在のマップのIteratorをセット → render()用

		//Objが通れるTileChipIdを設定
		void setNonCollisionTile(const std::vector<int>& tileIds);
		void addNonCollisionTile(int tileId);
		void removeNonCollisionTile(int tileId); ////// <!---------- 現在使っていない ---------->

		//Player登場関係
		player::DIR playerEntryDir_;
		void setNowMapByFlag(); //リセット先のマップ名を現在のマップ名にセット
		void setPlayerEntryDir();
		POINT getPlayerEntryPos(); //登場位置を計算
	public:
		void playerRebirth();	//復活マップへ移動
								//playerRebirth()呼ばれた後　絶対return;して！
								//この処理以降は処理をしないように！ <-- MapCollisionCheckしたらバグが出る
								//Called by map@init & PlayerDeadManager

		void createPlayer(); //生成 <-- Called by EffectReady

	private:
		void createAllEntitiesInMap(const std::string& mapName);
		void destroyAllEntitiesInMap();

		//Joints : 次のマップへの判定
		//＜判定、行き方向、次のマップ名＞
		std::vector<JointGoing> joints_;

		//次のマップ名 -> Joint & renderNext() に使う
		std::string nextMapName_;
		bool changingMap_; //次のマップへ移動中？
		int changeMapCnt_; //次のマップへ移動中のカウンター
		ci_ext::Vec3f changeMapVelocity_; //プレイヤー移動量
		ci_ext::Vec2f changeMapLandPoint_; //次のマップに到着座標
		void initChangeMap(const JointGoing& joint); //次のマップへ移動の最初のフレーム
		void updateChangeMap(); //次のマップへ移動中の処理
		void finishChangeMap(); //次のマップへ移動の最後のフレーム
		void checkRebirthMapFlag(); //復活マップのフラグをチェック

		//Closing Door
		int openDoorId_; //開けた扉のID
		bool closingDoorInProcess_;
	public:
		void setClosingDoorInProcess(bool flag);
		void finishClosingDoorProcess();
		
	public:
		Map(const std::string& stageName, const StageInfo& stageInfo);
		~Map();
		
		void init() override;
		void update() override;
		void render() override;
		void renderNextMap();

		void nextScene();

	private:
		//カメラ関係
		RECT getCameraLimit(const std::string& mapName); //カメラの限界を取得
		void setCameraSetting(const std::string& mapName); //カメラの限界をセット
		void keepCameraInScreen(const ci_ext::Vec2f& pos);

		//次のマップ関係
		void createJoints(const std::string& mapName); //接続判定のRECTを生成
	public:
		void checkJoints(POINT pt); //判定POINTが接続判定を触ったら -> Change Map OK
		bool isChangingMap() const; //次のマップへ移動中？

		//アイテム関係
		void createItems(const std::string& mapName);

		//敵関係
		void createEnemies(const std::string& mapName);

		//風関係
		void createWinds(const std::string& mapName);

		//イベント関係
		void createEvents(const std::string& mapName);

		//Collision関係
		int getChip(const std::string& mapName, int x, int y) const;
		RECT getTileRect(int x, int y) const; //マップ上にタイルチップのRECTを取得
		bool isCollision(int tileId) const;

		//Custom Tile Collision関係
		bool isCustomTile(int tileId) const; //カスタムタイルか？
		RECT getCustomTileSetting(int tileId) const; //カスタムタイルチップのRECTを取得
		RECT getCustomTileRect(const std::string& mapName, int x, int y, int tileId) const; //マップ上にカスタムタイルチップのRECTを取得
		
	private:
		//Secret Tile
		bool isSecretTile(int tileId) const; //隠しタイルか？

	public:
		//Damage Tile
		bool isDamageTile(int tileId) const; //ダメージタイルか？

		//Dead Tile
		bool isDeadTile(int tileId) const; //即死タイルか？

		//Wind Tile
		//bool isWindTile(int tileId) const; //風タイルか？
		//RECT getWindTileRect(const std::string& mapName, int x, int y);
		//info::WindDIR getWindDir(int tileId) const;
		//ci_ext::Vec3f getWindForce(info::WindDIR dir) const;

		//現在のマップ名を取得
		std::string getNowMapName() const;
		
	};
}