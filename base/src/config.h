#pragma once

class Config {
public:
	//World
	//const float GAME_SCALE = 0.35f;	//.h�t�@�C����const�̂ŁA�ς����Rebuild���K�{
	const float GAME_SCALE = 1.0f;	//.h�t�@�C����const�̂ŁA�ς����Rebuild���K�{
	const float GRAVITY = 0.4f * GAME_SCALE;

	//Map
	const int TRANSITION_FRAME = 30; //���̃}�b�v�ւ̃t���[��

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



	//�`��
	/*
	1.0f		| �w�i	
	0.95f		| �}�b�v
	:...
	0.8f~0.9f	| �A�C�e���E�G�Ȃ�
	:...
	0.7f~0.79f	| �v���C���[�֘A�i�v���C���[�E�V���b�g�Ȃǁj
	:...
	0.6f		| �}�b�v�i�B���^�C���E�`�b�v�j
	0.56~0.59f	| �G�t�F�N�g
	:...
	0.5f~0.55f	| UI
	0.3f		| ���j���[�E��
	:...
	0.0f		| �f�o�b�O�\��
	//*/
};

extern Config config;