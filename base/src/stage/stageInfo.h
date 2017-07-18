#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include <map>
#include <unordered_map>

#include "../player/player.h"

namespace map {
	namespace info {
		//接続の種類
		enum JointType {
			VERTICAL, //縦 : Perform transition Left <-> Right <-- Default by left to right 2d-side scrolling game
			HORIZONTAL, //横 : Perform transition Down <-> Up
		};
		//方向
		enum JointDIR {
			FORWARD, //進むしかない
			BOTH, //戻れる
		};
		//進み方
		enum JointRBLT {
			RIGHTBOTTOM, //デフォルト
			LEFTTOP,
		};
		//扉タイプ
		enum JointDoorType {
			NONE, //普通
			DOOR, //ボス部屋の扉
		};
		//風に飛ばされる方向
		enum WindDIR {
			LEFT, RIGHT,
			UP, DOWN,
		};
	}

	//各部屋（マップ）の情報
	struct MapInfo {
		std::string name; //マップ名
		POINT pos; //Pos by Index
		POINT size; //Size by Index

					//初期化
		MapInfo() {
			name = "";
			pos.x = pos.y = 0;
			size.x = size.y = 0;
		}
	};

	//アニメーションタイルの情報
	struct AnimTileInfo {
		int id;
		int duration;

		AnimTileInfo() {
			id = 0;
			duration = 0;
		}
	};

	//Animation Tile First ID
	struct AnimFirstID {
		std::string name;
		int id;
	};

	//プレイヤー登場・生成のマップ
	struct PlayerEntry {
		std::string name; //マップ名
		POINT pos; //マップ上のindex
		player::DIR dir; //右向きか　左向きか

						 //初期化
		PlayerEntry() {
			name = "";
			pos.x = pos.y = 0;
			dir = player::DIR::RIGHT;
		}
	};

	//接続の情報
	struct JointInfo {
		//map1 : 現在のマップ名
		//map2 : 次のマップ名
		std::string map1, map2;

		//種類
		info::JointType type;

		//Offset of map1 & map2 (index)
		//Vertical   : ↓
		//Horizontal : →
		int offset1, offset2;

		//Offset backward of map1 & map2 (index)
		//RBLT			RB	| LT
		//Vertical   1: ←	| →
		//Vertical   2: →	| ←
		//Horizontal 1: ↑	| ↓
		//Horizontal 2: ↓	| ↑
		float bOffset1, bOffset2;

		int size; //Size of collision (index)
		info::JointDIR dir; //both or forward
		info::JointRBLT going; //RightBottom or LeftTop
		info::JointDoorType doorType;
		int doorId;

		//初期化
		JointInfo() {
			map1 = map2 = "";
			type = info::JointType::VERTICAL;
			offset1 = offset2 = 0;
			bOffset1 = bOffset2 = 0.0f;
			size = 0;
			dir = info::JointDIR::FORWARD;
			going = info::JointRBLT::RIGHTBOTTOM;
			doorType = info::JointDoorType::NONE;
		}
	};

	//Item
	struct ItemInfo {
		std::string name;
		POINT pos; //マップ上のindex
		bool respawn; //マップに再入したら、復活？
		int id; //ID of item (serialize run by that stage)
		int uid; //Unique ID (unique item in the game)

				 //初期化
		ItemInfo() {
			name = "";
			pos.x = pos.y = 0;
			respawn = false;
			id = -1;
			uid = -1;
		}
	};

	//Enemy
	struct EnemyInfo {
		std::string name;
		POINT pos; //マップ上のindex
		bool respawn; //マップに再入したら、復活？
		int id; //ID of enemy (serialize run by that stage)

				//初期化
		EnemyInfo() {
			name = "";
			pos.x = pos.y = 0;
			respawn = true; //常にTRUE
			id = -1;
		}
	};

	//Wind
	//struct WindInfo {
	//	int id; //マップチップID
	//	float force; //強さ
	//	info::WindDIR dir; 
	//};
	struct WindInfo {
		POINT pos; //マップ上のindex
		info::WindDIR dir; //飛ばされる方向
		std::string move; //風の移動方向
		float speed; //移動量
		int range; //移動範囲(index)

				   //初期化
		WindInfo() {
			pos.x = pos.y = 0;
			dir = info::WindDIR::UP;
			move = "";
			speed = 0;
			range = 0;
		}
	};

	//Event
	struct EventInfo {
		std::string name; //Identifier 
		std::string cmd; //Command
		POINT pos; //マップ上のindex
		POINT size; //幅・高さ
					//bool reDead;	//プレイヤーが死んだら、このイベントは復活できる
					//false になると　そのゲームワールド中、１回発生したら　一切復活しない！

					//初期化
		EventInfo() {
			name = "";
			cmd = "";
			pos.x = pos.y = 0;
			size.x = size.y = 0;
			//reDead = false;
		}
	};

	//----------------------------------------------------------------------------------------------------
	//----------------------------------------	MAIN	----------------------------------------
	//----------------------------------------------------------------------------------------------------
	//ステージ内のObj情報
	//ファイル読み込むとマップ表示・判定に使う
	struct StageInfo final {
		//##################################################
		//-------------------- Info 継承に使う --------------------
		//##################################################
		PlayerEntry startMap;
		PlayerEntry continueMap;
		std::vector<PlayerEntry> continueMaps;
		bool isPlayerDemo;

		//部屋の情報
		//< MapName, Map Info >
		std::unordered_map<std::string, MapInfo> mapInfos;

		//部屋のタイル
		//< MapName, 2-D Map Array >
		std::unordered_map<std::string, std::vector<std::vector<int>>> stageMaps; //全体ステージの情報
		std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator nowMapIt; //現在のマップのiterator → render()用
		std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator nextMapIt; //次のマップのiterator → render()用
		//背景タイルフラグ　→　この<マップ名>は背景タイルを使うか？
		std::unordered_map<std::string, bool> stageBgFlags;
		//背景タイル→バッファ
		std::unordered_map<std::string, std::vector<std::vector<int>>> stageBgs;
		//アニメーションタイル
		std::unordered_map<std::string, anim::Anim> animTilesClassByName; //各キーに一つのAnimクラスを持つ
		std::vector<AnimFirstID> animTileFirstID; //各キーにアニメーションの最初のTileIDを持つ

		//全体ステージのアイテム
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<ItemInfo>> stageItems;

		//全体ステージの敵
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<EnemyInfo>> stageEnemies;

		//全体ステージの風
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<WindInfo>> stageWinds;

		//全体ステージのイベント
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<EventInfo>> stageEvents;
		std::unordered_map<std::string, std::vector<EventInfo>> stageAutoEvents;
		std::unordered_map<std::string, std::vector<EventInfo>> stageTutorialEvents;

		//ステージのカスタムタイル
		std::map<int, RECT> customTile; //カスタムRECTです！　Offsetじゃないので注意!

		//各部屋（マップ）の接点
		std::vector<JointInfo> jointInfos;

		//Non Collision ID
		std::vector<int> nonCollisionId;

		//Secret Tile ID
		std::vector<int> secretTileId;

		//Damage Tile ID | 触ったら、ダメージを受ける
		std::vector<int> damageTileId;

		//Wind Tile ID & 方向 | 触ったら、飛ばせる
		//std::vector<WindInfo> windTiles;

		//Animation Tileの情報
		//< 識別名, Info Array >
		std::unordered_map<std::string, std::vector<AnimTileInfo>> animTilesIO;

		void clear()
		{
			isPlayerDemo = false;

			//Clear Memory
			mapInfos.clear();
			stageMaps.clear();
			nowMapIt = stageMaps.end(); //Set iterator to null
			nextMapIt = stageMaps.end(); //Set iterator to null
			stageBgFlags.clear();
			stageBgs.clear();
			animTilesClassByName.clear();
			animTileFirstID.clear();
			stageItems.clear();
			stageEnemies.clear();
			stageWinds.clear();
			stageEvents.clear();
			stageAutoEvents.clear();

			customTile.clear();
			jointInfos.clear();
			nonCollisionId.clear();
			nonCollisionId.push_back(0); //Default 表示か　非表示か　画像で決める
			nonCollisionId.push_back(-1); //Default 非表示
			secretTileId.clear();
			damageTileId.clear();
			//windTiles.clear();
		}

		StageInfo() { clear(); }

		~StageInfo() { clear(); }
	};
}