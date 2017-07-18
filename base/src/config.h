#pragma once

class Config {
public:
	//World
	//const float GAME_SCALE = 0.35f;	//.hファイルのconstので、変えるとRebuildが必須
	const float GAME_SCALE = 1.0f;	//.hファイルのconstので、変えるとRebuildが必須
	const float GRAVITY = 0.4f * GAME_SCALE;

	//Map
	const int TRANSITION_FRAME = 30; //次のマップへのフレーム

	//Wind
	const float WIND_FORCE_X = 10.0f;
	const float WIND_FORCE_Y = 10.0f;
	
	//Player
	//Movement
	const float PLAYER_MOVE_SPD = 5.5f * GAME_SCALE;
	const float PLAYER_FLY_SPD = 7.0f * GAME_SCALE;
	const float PLAYER_JUMP_MOVEX_SPD = PLAYER_MOVE_SPD;
	//const float PLAYER_JUMP_MOVEX_SPD = 3.0f * GAME_SCALE;
	//const float PLAYER_FALL_MOVEX_SPD = 3.0f * GAME_SCALE; = JUMP SPD

	//Jump
	const float PLAYER_JUMP_POW = 7.0f * GAME_SCALE;
	const float PLAYER_JUMP_INC = 0.24f * GAME_SCALE;

	//Get Hit
	const float PLAYER_HIT_X_SPD = PLAYER_MOVE_SPD / 5;
	const float PLAYER_HIT_Y_SPD = PLAYER_JUMP_POW;

	//Shot
	const float PLAYER_MAGIC_SPD = 8.0f * GAME_SCALE;
	const int ATK_M_MP_USE = 25;



	//描画
	/*
	1.0f		| 背景	
	0.95f		| マップ
	:...
	0.8f~0.9f	| アイテム・敵など
	:...
	0.7f~0.79f	| プレイヤー関連（プレイヤー・ショットなど）
	:...
	0.6f		| マップ（隠しタイル・チップ）
	0.56~0.59f	| エフェクト
	:...
	0.5f~0.55f	| UI
	0.3f		| メニュー・他
	:...
	0.0f		| デバッグ表示
	//*/
};

extern Config config;