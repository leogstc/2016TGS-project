#pragma once

#include "../object/animation.hpp"
#include "../object/mapCollisionObj.h"
#include "playerInfo.hpp" //�v���C���[�̏����A�N�Z�X
#include "mover.h"
#include "shot.h"
#include "../util/timer.h"

//Recording Demo
//#define RECORD_DEMO

namespace player {
	namespace sprite {
		enum {
			//Player Size
			SPRITE_SIZE = 128,
			SPRITE_HALF_SIZE = SPRITE_SIZE / 2,
		};

		enum Offset {
			//�����蔻��̃I�t�Z�b�g
			OFFSET_STAND_TOP = 43,
			OFFSET_STAND_BOTTOM = 24,
			OFFSET_STAND_LEFT = 47,
			OFFSET_STAND_RIGHT = 49,

			OFFSET_JUMP_TOP1 = 43,
			OFFSET_JUMP_BOTTOM1 = 24,
			OFFSET_JUMP_LEFT1 = 47,
			OFFSET_JUMP_RIGHT1 = 49,

			OFFSET_JUMP_TOP2 = 43,
			OFFSET_JUMP_BOTTOM2 = 24,
			OFFSET_JUMP_LEFT2 = 47,
			OFFSET_JUMP_RIGHT2 = 49,

			OFFSET_JUMP_TOP3 = 55,
			OFFSET_JUMP_BOTTOM3 = 24,
			OFFSET_JUMP_LEFT3 = 47,
			OFFSET_JUMP_RIGHT3 = 49,

			OFFSET_JUMP_TOP4 = 43,
			OFFSET_JUMP_BOTTOM4 = 24,
			OFFSET_JUMP_LEFT4 = 47,
			OFFSET_JUMP_RIGHT4 = 49,

			OFFSET_JUMP_TOP5 = 30,
			OFFSET_JUMP_BOTTOM5 = 24,
			OFFSET_JUMP_LEFT5 = 47,
			OFFSET_JUMP_RIGHT5 = 49,

			OFFSET_FALL_TOP = 43,
			OFFSET_FALL_BOTTOM = 24,
			OFFSET_FALL_LEFT = 47,
			OFFSET_FALL_RIGHT = 49,

			//OFFSET_FALL_TOP2 = 59,
			//OFFSET_FALL_BOTTOM2 = 24,
			//OFFSET_FALL_LEFT2 = 53,
			//OFFSET_FALL_RIGHT2 = 40,

			//OFFSET_FALL_TOP3 = 68,
			//OFFSET_FALL_BOTTOM3 = 24,
			//OFFSET_FALL_LEFT3 = 53,
			//OFFSET_FALL_RIGHT3 = 40,

			//OFFSET_FALL_TOP4 = 73,
			//OFFSET_FALL_BOTTOM4 = 24,
			//OFFSET_FALL_LEFT4 = 53,
			//OFFSET_FALL_RIGHT4 = 40,

			//OFFSET_FALL_TOP5 = 66,
			//OFFSET_FALL_BOTTOM5 = 24,
			//OFFSET_FALL_LEFT5 = 53,
			//OFFSET_FALL_RIGHT5 = 40,

			//OFFSET_ATK_C_TOP1 = 43,
			//OFFSET_ATK_C_BOTTOM1 = 24,
			//OFFSET_ATK_C_LEFT1 = 47,
			//OFFSET_ATK_C_RIGHT1 = 49,

			//OFFSET_ATK_C_TOP2 = 43,
			//OFFSET_ATK_C_BOTTOM2 = 24,
			//OFFSET_ATK_C_LEFT2 = 47,
			//OFFSET_ATK_C_RIGHT2 = 49,

			//OFFSET_ATK_C_TOP3 = 43,
			//OFFSET_ATK_C_BOTTOM3 = 24,
			//OFFSET_ATK_C_LEFT3 = 47,
			//OFFSET_ATK_C_RIGHT3 = 49,

			//OFFSET_ATK_C_TOP4 = 43,
			//OFFSET_ATK_C_BOTTOM4 = 24,
			//OFFSET_ATK_C_LEFT4 = 47,
			//OFFSET_ATK_C_RIGHT4 = 49,

			//OFFSET_ATK_C_TOP5 = 43,
			//OFFSET_ATK_C_BOTTOM5 = 24,
			//OFFSET_ATK_C_LEFT5 = 47,
			//OFFSET_ATK_C_RIGHT5 = 49,

			//OFFSET_ATK_C_TOP6 = 43,
			//OFFSET_ATK_C_BOTTOM6 = 24,
			//OFFSET_ATK_C_LEFT6 = 47,
			//OFFSET_ATK_C_RIGHT6 = 49,

			//OFFSET_ATK_C_TOP7 = 43,
			//OFFSET_ATK_C_BOTTOM7 = 24,
			//OFFSET_ATK_C_LEFT7 = 47,
			//OFFSET_ATK_C_RIGHT7 = 49,

			OFFSET_FLY_TOP = 45,
			OFFSET_FLY_BOTTOM = 30,
			OFFSET_FLY_LEFT = 32,
			OFFSET_FLY_RIGHT = 34,

			//OFFSET_FLY_ATK_TOP = 63,
			//OFFSET_FLY_ATK_BOTTOM = 63,
			//OFFSET_FLY_ATK_LEFT = 63,
			//OFFSET_FLY_ATK_RIGHT = 63,

			//OFFSET_FLY_ATK_END_TOP = 34,
			//OFFSET_FLY_ATK_END_BOTTOM = 27,
			//OFFSET_FLY_ATK_END_LEFT = 42,
			//OFFSET_FLY_ATK_END_RIGHT = 38,

			OFFSET_DEAD_TOP = 100,
			OFFSET_DEAD_BOTTOM = 24,
			OFFSET_DEAD_LEFT = 60,
			OFFSET_DEAD_RIGHT = 60,
			//----- END �����蔻��I�t�Z�b�g
		};

		enum MapCollisionOffset {
			//Map Collision -> MINimum offset
			MAP_COL_OFFSET_STAND_TOP = OFFSET_STAND_TOP,
			MAP_COL_OFFSET_STAND_BOTTOM = OFFSET_STAND_BOTTOM,
			MAP_COL_OFFSET_STAND_LEFT = OFFSET_STAND_LEFT,
			MAP_COL_OFFSET_STAND_RIGHT = OFFSET_STAND_RIGHT,

			MAP_COL_OFFSET_JUMP_TOP = OFFSET_JUMP_TOP5,
			MAP_COL_OFFSET_JUMP_BOTTOM = OFFSET_JUMP_BOTTOM5,
			MAP_COL_OFFSET_JUMP_LEFT = OFFSET_JUMP_LEFT5,
			MAP_COL_OFFSET_JUMP_RIGHT = OFFSET_JUMP_RIGHT5,

			MAP_COL_OFFSET_FALL_TOP = OFFSET_FALL_TOP,
			MAP_COL_OFFSET_FALL_BOTTOM = OFFSET_FALL_BOTTOM,
			MAP_COL_OFFSET_FALL_LEFT = OFFSET_FALL_LEFT,
			MAP_COL_OFFSET_FALL_RIGHT = OFFSET_FALL_RIGHT,

			MAP_COL_OFFSET_FLY_TOP = OFFSET_FLY_TOP,
			MAP_COL_OFFSET_FLY_BOTTOM = OFFSET_FLY_BOTTOM,
			MAP_COL_OFFSET_FLY_LEFT = OFFSET_FLY_LEFT,
			MAP_COL_OFFSET_FLY_RIGHT = OFFSET_FLY_RIGHT,

			//MAP_COL_OFFSET_FLY_ATK_TOP = 34,
			//MAP_COL_OFFSET_FLY_ATK_BOTTOM = 8,
			//MAP_COL_OFFSET_FLY_ATK_LEFT = 34,
			//MAP_COL_OFFSET_FLY_ATK_RIGHT = 16,

			//MAP_COL_OFFSET_FLY_ATK_END_TOP = OFFSET_FLY_ATK_END_TOP,
			//MAP_COL_OFFSET_FLY_ATK_END_BOTTOM = OFFSET_FLY_ATK_END_BOTTOM,
			//MAP_COL_OFFSET_FLY_ATK_END_LEFT = OFFSET_FLY_ATK_END_LEFT,
			//MAP_COL_OFFSET_FLY_ATK_END_RIGHT = OFFSET_FLY_ATK_END_RIGHT,

			MAP_COL_OFFSET_DEAD_TOP = OFFSET_DEAD_TOP,
			MAP_COL_OFFSET_DEAD_BOTTOM = OFFSET_DEAD_BOTTOM,
			MAP_COL_OFFSET_DEAD_LEFT = OFFSET_DEAD_LEFT,
			MAP_COL_OFFSET_DEAD_RIGHT = OFFSET_DEAD_RIGHT,
			//----- END Map Collision MINimum offset
		};

		enum Animation {
			//�A�j���[�V�����t���[��
			ANIM_STAND = 6,
			ANIM_WALK = 10,
			ANIM_JUMP = 4,
			ANIM_FALL = 4,
			ANIM_ATK_C = 6,
			ANIM_ATK_M = 4,
			ANIM_START_FLY = 3,
			ANIM_FLY = 6,
			ANIM_HIT = 3,
			ANIM_FLY_ATK = 8,
			ANIM_FLY_ATK_END = 2,
			ANIM_DEAD = 5, 

			//�A�j���[�V�������x�i�t���[���j
			ANIM_SPD_STAND = 6,
			ANIM_SPD_WALK = 5,
			ANIM_SPD_JUMP = 3,
			ANIM_SPD_FALL = 1,
			ANIM_SPD_ATK_C = 3,
			ANIM_SPD_ATK_M = 6,
			ANIM_SPD_START_FLY = 3,
			ANIM_SPD_FLY = 6,
			ANIM_SPD_HIT = 6,
			ANIM_SPD_FLY_ATK = 2,
			ANIM_SPD_FLY_ATK_END = 4,
			ANIM_SPD_DEAD = 8,
		};

		//enum CustomCollisionFrame {
		//	//�U��������`�F�b�N�ł���A�j���[�V�����t���[��
		//	//2^(n�t���[����) | 2^(n�t���[����) | ...
		//	//n��0����X�^�[�g
		//	ATK_C_AVAILABLE = 0x04 | 0x08 | 0x10,
		//	FLY_ATK_AVAILABLE = 0xFF,
		//};
	}

	//��ԁF�l�̓A�j���[�V�����̃X���b�g
	enum class State {
		NONE = -1,	//�����E�ݒ肵�Ȃ� -> Move�̕���
		STAND = 0,	//����
		WALK = 1,	//����
		JUMP = 2,	//�W�����v
		FALL = 3,	//����
		ATK_C = 4,	//�ߐڍU��
		ATK_M = 5,	//���@�U��
		START_FLY = 9,	//��Ԏn��
		FLY = 6,	//���
		HIT = 7,	//�_���[�W���󂯂�
		DEAD = 8, 
		FLY_ATK = 10,
		FLY_ATK_END = 11,
	};

	//�v���C���[�̕���
	enum class DIR {
		LEFT = -1,
		RIGHT = 1,
	};


	class Player : public map::MapCollisionObj {
		bool isDemo_;
#ifdef RECORD_DEMO
		std::vector<WORD> recordBtn;
		void saveRec();
#else
		std::vector<WORD> readRec;
		void loadRec();
#endif

		anim::Anim anim_; //�A�j���[�V�����Ǘ�
		State state_; //���
		DIR dir_; //����
		bool controlable_; //����ł��邩�H
		int cnt_; //�J�E���^�[
		bool initDead_; //���ʏu�Ԃ̏����t���O
		bool isShot_;
		float shotPow_; //shot�̍U����
		int deadcnt;//���S��V�[���؂�ւ�
		std::weak_ptr<Object> create_timer_;//�e�^�C�}�[

		std::weak_ptr<PlayerInfo> info_; //Player�̏�� (HP/MP/etc.)
		std::shared_ptr<Mover> mover_;
		std::shared_ptr<KeyMover> keyMover_; //Demo Play��p

		//�L�[����
		void MoveKeyX();
		void MoveKeyY();

		//��Ԑݒ�
		void setState(State state = State::STAND); //��ԕύX�ł��邩���m�F���A�ł���ΕύX����
		void setStateForce(State state); //��ԕύX | ������Ɏg�����͋����I�ɏ�ԕύX

		//��`�m�F�p
		template <typename T = int>
		bool isRectValueEqual(const RECT& rt, T val) {
			return (rt.left == val && rt.top == val && rt.right == val && rt.bottom == val);
		}

	public:
		Player(int x = 0, int y = 0, DIR dir = DIR::RIGHT, bool isDemo = false);
		~Player();

		void init() override;
		void render() override;
		void resume() override;

		void update() override;
		void updateStand();
		void updateWalk();
		void updateJump();
		void updateFall();
		//void updateAtkC();
		void updateAtkM();
		//void updateStartFly(); //�A�j���[�V��������
		void updateFly();
		//void updateFlyAtk();
		//void updateFlyAtkEnd();
		void updateHit(); //�_���[�W���󂯂�
		void updateDead();

		void doOnceHitByEnemy(float dmg);
		void doOnceHitDamageTile() override;
		void doOnceHitDeadTile() override;

		void initDead();

		//�l�����ւ��� -> ����RECT�����Ɏg��
		template <typename T = int>
		void swapLRbyScale(T& a, T& b, T aVal, T bVal) {
			//if (body_.scale().x() < 0) {
			if (dir_ == DIR::LEFT) {
				a = bVal;
				b = aVal;
			}
			else {
				a = aVal;
				b = bVal;
			}
		}

	private:
		//�v���C���[�̃I�t�Z�b�g���擾
		RECT getBodyOffset(State state = State::NONE);
		RECT getSpriteMapOffset(State state = State::NONE);

	public:
		//�v���C���[�̌����W�ɂ��RECT��Ԃ�
		RECT getBodyCollisionRect(State state = State::NONE); //Body Collision �� �G�̂����蔻��
		RECT getSpriteMapCollisionRect(State state = State::NONE); //Body Collision �� �}�b�v�̂����蔻��
		//RECT getAtkCollision(); //Attack Collision

		//�v���C���[�̍��W
		ci_ext::Vec2f getPos() const; //�v���C���[�̍��W���擾
		void offsetPos(const ci_ext::Vec3f& offset); //�w�肵���l�ɂ��A�v���C���[�̍��W�������
		void setVelocity(float x = 0.0f, float y = 0.0f); //�ړ��ʂ��Z�b�g

		//�v���C���[�̕���
		DIR getDir() const;

		//�v���C���[�̏��
		State getPlayerState() const;
		void setPlayerState(State state); //�O�������ԕύX
		void setPlayerStateJoint(State state); //Joint�Ɣ�����ĂԁAJoint�Ɣ��ȊO����Ă΂Ȃ��ŁI�I
		void setDeadState(State state);

		bool isInvincible() const;

		//����
		void setControlable(bool flag);
	};
}