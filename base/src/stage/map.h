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

	//�w�肵�����W���w�肵��RECT���ɂ��邩�H
	bool isPointInRect(POINT p, RECT rt);

	class Map : public ci_ext::Object {
		enum DIR { //Joint�Ɏg��
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

		//�X�e�[�W�̏��
		StageInfo info_;

		//�Q�[���S�̂̃t���O
		std::weak_ptr<game::GameFlag> gameFlag_;
		//�X�e�[�W�t���O
		std::weak_ptr<StageFlag> stageFlag_;
		//���Ǘ�
		std::weak_ptr<object::WindManager> windManager_;
		//�G�t�F�N�g�Ǘ�
		std::weak_ptr<effect::EffectManager> effectManager_;
		//�A�C�e���Ǘ�
		std::weak_ptr<item::ItemManager> itemManager_;
		//�G�Ǘ�
		std::weak_ptr<enemy::EnemyManager> enemyManager_;
		//�C�x���g�Ǘ�
		std::weak_ptr<gameevent::StageEventManager> eventManager_;
		//UI�Ǘ�
		std::weak_ptr<BOMB::uiManager> uiManager_;
		//Timer
		std::weak_ptr<game::Timer> timer_;


		//�J����
		camera::StageCamera stageCamera_;

		//�X�e�[�W��
		std::string stageName_;
		//�}�b�v��
		std::string nowMapName_;

		//�o��E����
		POINT playerEntryPos_; //�ʒu
		bool playerEntry_; //�t���O

		//�`��֌W
		int tileSizeX_;
		//int tileSizeY_; // %��/���g���̂ŁAY(Height)�͗v��Ȃ�
		int readyPosY_ = gplib::system::WINH / 2;

		//�t�@�C������ǂ񂾑S�̃X�e�[�W�̏����C��
		void finalizeLoadStage();

		//���݂̃}�b�vIterator
		void setNowMapIterator(); //���݂̃}�b�v��Iterator���Z�b�g �� render()�p

		//Obj���ʂ��TileChipId��ݒ�
		void setNonCollisionTile(const std::vector<int>& tileIds);
		void addNonCollisionTile(int tileId);
		void removeNonCollisionTile(int tileId); ////// <!---------- ���ݎg���Ă��Ȃ� ---------->

		//Player�o��֌W
		player::DIR playerEntryDir_;
		void setNowMapByFlag(); //���Z�b�g��̃}�b�v�������݂̃}�b�v���ɃZ�b�g
		void setPlayerEntryDir();
		POINT getPlayerEntryPos(); //�o��ʒu���v�Z
	public:
		void playerRebirth();	//�����}�b�v�ֈړ�
								//playerRebirth()�Ă΂ꂽ��@���return;���āI
								//���̏����ȍ~�͏��������Ȃ��悤�ɁI <-- MapCollisionCheck������o�O���o��
								//Called by map@init & PlayerDeadManager

		void createPlayer(); //���� <-- Called by EffectReady

	private:
		void createAllEntitiesInMap(const std::string& mapName);
		void destroyAllEntitiesInMap();

		//Joints : ���̃}�b�v�ւ̔���
		//������A�s�������A���̃}�b�v����
		std::vector<JointGoing> joints_;

		//���̃}�b�v�� -> Joint & renderNext() �Ɏg��
		std::string nextMapName_;
		bool changingMap_; //���̃}�b�v�ֈړ����H
		int changeMapCnt_; //���̃}�b�v�ֈړ����̃J�E���^�[
		ci_ext::Vec3f changeMapVelocity_; //�v���C���[�ړ���
		ci_ext::Vec2f changeMapLandPoint_; //���̃}�b�v�ɓ������W
		void initChangeMap(const JointGoing& joint); //���̃}�b�v�ֈړ��̍ŏ��̃t���[��
		void updateChangeMap(); //���̃}�b�v�ֈړ����̏���
		void finishChangeMap(); //���̃}�b�v�ֈړ��̍Ō�̃t���[��
		void checkRebirthMapFlag(); //�����}�b�v�̃t���O���`�F�b�N

		//Closing Door
		int openDoorId_; //�J��������ID
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
		//�J�����֌W
		RECT getCameraLimit(const std::string& mapName); //�J�����̌��E���擾
		void setCameraSetting(const std::string& mapName); //�J�����̌��E���Z�b�g
		void keepCameraInScreen(const ci_ext::Vec2f& pos);

		//���̃}�b�v�֌W
		void createJoints(const std::string& mapName); //�ڑ������RECT�𐶐�
	public:
		void checkJoints(POINT pt); //����POINT���ڑ������G������ -> Change Map OK
		bool isChangingMap() const; //���̃}�b�v�ֈړ����H

		//�A�C�e���֌W
		void createItems(const std::string& mapName);

		//�G�֌W
		void createEnemies(const std::string& mapName);

		//���֌W
		void createWinds(const std::string& mapName);

		//�C�x���g�֌W
		void createEvents(const std::string& mapName);

		//Collision�֌W
		int getChip(const std::string& mapName, int x, int y) const;
		RECT getTileRect(int x, int y) const; //�}�b�v��Ƀ^�C���`�b�v��RECT���擾
		bool isCollision(int tileId) const;

		//Custom Tile Collision�֌W
		bool isCustomTile(int tileId) const; //�J�X�^���^�C�����H
		RECT getCustomTileSetting(int tileId) const; //�J�X�^���^�C���`�b�v��RECT���擾
		RECT getCustomTileRect(const std::string& mapName, int x, int y, int tileId) const; //�}�b�v��ɃJ�X�^���^�C���`�b�v��RECT���擾
		
	private:
		//Secret Tile
		bool isSecretTile(int tileId) const; //�B���^�C�����H

	public:
		//Damage Tile
		bool isDamageTile(int tileId) const; //�_���[�W�^�C�����H

		//Dead Tile
		bool isDeadTile(int tileId) const; //�����^�C�����H

		//Wind Tile
		//bool isWindTile(int tileId) const; //���^�C�����H
		//RECT getWindTileRect(const std::string& mapName, int x, int y);
		//info::WindDIR getWindDir(int tileId) const;
		//ci_ext::Vec3f getWindForce(info::WindDIR dir) const;

		//���݂̃}�b�v�����擾
		std::string getNowMapName() const;
		
	};
}