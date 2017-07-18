#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include <map>
#include <unordered_map>

#include "../player/player.h"

namespace map {
	namespace info {
		//�ڑ��̎��
		enum JointType {
			VERTICAL, //�c : Perform transition Left <-> Right <-- Default by left to right 2d-side scrolling game
			HORIZONTAL, //�� : Perform transition Down <-> Up
		};
		//����
		enum JointDIR {
			FORWARD, //�i�ނ����Ȃ�
			BOTH, //�߂��
		};
		//�i�ݕ�
		enum JointRBLT {
			RIGHTBOTTOM, //�f�t�H���g
			LEFTTOP,
		};
		//���^�C�v
		enum JointDoorType {
			NONE, //����
			DOOR, //�{�X�����̔�
		};
		//���ɔ�΂�������
		enum WindDIR {
			LEFT, RIGHT,
			UP, DOWN,
		};
	}

	//�e�����i�}�b�v�j�̏��
	struct MapInfo {
		std::string name; //�}�b�v��
		POINT pos; //Pos by Index
		POINT size; //Size by Index

					//������
		MapInfo() {
			name = "";
			pos.x = pos.y = 0;
			size.x = size.y = 0;
		}
	};

	//�A�j���[�V�����^�C���̏��
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

	//�v���C���[�o��E�����̃}�b�v
	struct PlayerEntry {
		std::string name; //�}�b�v��
		POINT pos; //�}�b�v���index
		player::DIR dir; //�E�������@��������

						 //������
		PlayerEntry() {
			name = "";
			pos.x = pos.y = 0;
			dir = player::DIR::RIGHT;
		}
	};

	//�ڑ��̏��
	struct JointInfo {
		//map1 : ���݂̃}�b�v��
		//map2 : ���̃}�b�v��
		std::string map1, map2;

		//���
		info::JointType type;

		//Offset of map1 & map2 (index)
		//Vertical   : ��
		//Horizontal : ��
		int offset1, offset2;

		//Offset backward of map1 & map2 (index)
		//RBLT			RB	| LT
		//Vertical   1: ��	| ��
		//Vertical   2: ��	| ��
		//Horizontal 1: ��	| ��
		//Horizontal 2: ��	| ��
		float bOffset1, bOffset2;

		int size; //Size of collision (index)
		info::JointDIR dir; //both or forward
		info::JointRBLT going; //RightBottom or LeftTop
		info::JointDoorType doorType;
		int doorId;

		//������
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
		POINT pos; //�}�b�v���index
		bool respawn; //�}�b�v�ɍē�������A�����H
		int id; //ID of item (serialize run by that stage)
		int uid; //Unique ID (unique item in the game)

				 //������
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
		POINT pos; //�}�b�v���index
		bool respawn; //�}�b�v�ɍē�������A�����H
		int id; //ID of enemy (serialize run by that stage)

				//������
		EnemyInfo() {
			name = "";
			pos.x = pos.y = 0;
			respawn = true; //���TRUE
			id = -1;
		}
	};

	//Wind
	//struct WindInfo {
	//	int id; //�}�b�v�`�b�vID
	//	float force; //����
	//	info::WindDIR dir; 
	//};
	struct WindInfo {
		POINT pos; //�}�b�v���index
		info::WindDIR dir; //��΂�������
		std::string move; //���̈ړ�����
		float speed; //�ړ���
		int range; //�ړ��͈�(index)

				   //������
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
		POINT pos; //�}�b�v���index
		POINT size; //���E����
					//bool reDead;	//�v���C���[�����񂾂�A���̃C�x���g�͕����ł���
					//false �ɂȂ�Ɓ@���̃Q�[�����[���h���A�P�񔭐�������@��ؕ������Ȃ��I

					//������
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
	//�X�e�[�W����Obj���
	//�t�@�C���ǂݍ��ނƃ}�b�v�\���E����Ɏg��
	struct StageInfo final {
		//##################################################
		//-------------------- Info �p���Ɏg�� --------------------
		//##################################################
		PlayerEntry startMap;
		PlayerEntry continueMap;
		std::vector<PlayerEntry> continueMaps;
		bool isPlayerDemo;

		//�����̏��
		//< MapName, Map Info >
		std::unordered_map<std::string, MapInfo> mapInfos;

		//�����̃^�C��
		//< MapName, 2-D Map Array >
		std::unordered_map<std::string, std::vector<std::vector<int>>> stageMaps; //�S�̃X�e�[�W�̏��
		std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator nowMapIt; //���݂̃}�b�v��iterator �� render()�p
		std::unordered_map<std::string, std::vector<std::vector<int>>>::iterator nextMapIt; //���̃}�b�v��iterator �� render()�p
		//�w�i�^�C���t���O�@���@����<�}�b�v��>�͔w�i�^�C�����g�����H
		std::unordered_map<std::string, bool> stageBgFlags;
		//�w�i�^�C�����o�b�t�@
		std::unordered_map<std::string, std::vector<std::vector<int>>> stageBgs;
		//�A�j���[�V�����^�C��
		std::unordered_map<std::string, anim::Anim> animTilesClassByName; //�e�L�[�Ɉ��Anim�N���X������
		std::vector<AnimFirstID> animTileFirstID; //�e�L�[�ɃA�j���[�V�����̍ŏ���TileID������

		//�S�̃X�e�[�W�̃A�C�e��
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<ItemInfo>> stageItems;

		//�S�̃X�e�[�W�̓G
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<EnemyInfo>> stageEnemies;

		//�S�̃X�e�[�W�̕�
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<WindInfo>> stageWinds;

		//�S�̃X�e�[�W�̃C�x���g
		//< MapName, Info Array >
		std::unordered_map<std::string, std::vector<EventInfo>> stageEvents;
		std::unordered_map<std::string, std::vector<EventInfo>> stageAutoEvents;
		std::unordered_map<std::string, std::vector<EventInfo>> stageTutorialEvents;

		//�X�e�[�W�̃J�X�^���^�C��
		std::map<int, RECT> customTile; //�J�X�^��RECT�ł��I�@Offset����Ȃ��̂Œ���!

		//�e�����i�}�b�v�j�̐ړ_
		std::vector<JointInfo> jointInfos;

		//Non Collision ID
		std::vector<int> nonCollisionId;

		//Secret Tile ID
		std::vector<int> secretTileId;

		//Damage Tile ID | �G������A�_���[�W���󂯂�
		std::vector<int> damageTileId;

		//Wind Tile ID & ���� | �G������A��΂���
		//std::vector<WindInfo> windTiles;

		//Animation Tile�̏��
		//< ���ʖ�, Info Array >
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
			nonCollisionId.push_back(0); //Default �\�����@��\�����@�摜�Ō��߂�
			nonCollisionId.push_back(-1); //Default ��\��
			secretTileId.clear();
			damageTileId.clear();
			//windTiles.clear();
		}

		StageInfo() { clear(); }

		~StageInfo() { clear(); }
	};
}