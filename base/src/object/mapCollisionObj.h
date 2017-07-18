#pragma once

#include "movableObject.hpp"

namespace map {
	//�}�b�v�Ƃ̔�����s���B
	//�ړ��l�ɉ����ăv���C���[���~������
	class MapCollisionObj : public game::MovableObject
	{
	private:
		//����̃I�t�Z�b�g
		//Offset Start (LeftTop) / End (RightBottom)
		int offsetS_, offsetE_;
		//����p��`
		RECT objRect_;

		//Check Movement <-- Obj�ړ��ɂ���ă`�F�b�N
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
		//Obj�̏�Ԃɂ��A��Ƀ`�F�b�N
		bool checkUnderFloor();
		bool checkHeadFloor();

		//Obj�̈ړ��ɂ��`�F�b�N
		//checkMoveMapOOOO�n
		void mapCheckMoveXY();

	private:
		//Check Collision <-- Obj�̔����`���ˑR�ς��ꍇ
		//�����F x = Offset X | y = Offset Y | opposite = ���Α����`�F�b�N���Ă����H
		//Collision�̋�����Ԃ�
		//Bottom�`�F�b�N�����͓���CustomRect
		int collisionMapLeft(int x = 0, int y = 0, bool opposite = false);
		int collisionMapRight(int x = 0, int y = 0, bool opposite = false);
		int collisionMapTop(int x = 0, int y = 0, bool opposite = false);
		int collisionMapBottom(int x = 0, int y = 0, bool opposite = false); //����

	protected:
		//�v���C���[�֌W
		bool isPlayer_; //���̃I�u�W�F�N�g���v���C���[���H
		std::string nowInMapName_; //���݂ɂ���}�b�v��
		bool invincible_; //���G��� <- �_���[�W���󂯂����E��莞��
		int invincibleCnt_; //���G��ԃJ�E���^�[

		//Obj�̔����`���ˑR�ς��ꍇ
		bool mapCheckCollisionX(bool adjust = false);
		bool mapCheckCollisionY(bool adjust = false);
		bool mapCheckCollision4Way(bool adjust = false);

		//Obj�̔���RECT���Z�b�g
		void setObjCollisionMapRect(int offsetSx = 0, int offsetSy = 0, int offsetEx = 0, int offsetEy = 0);
		void setObjCollisionMapRect(const RECT& rect, bool isOffsetRect = false); //Offset�Ȃ�bool�̈�����true�ɃZ�b�g����
		RECT getObjCollisionMapRect();

		//RECT Making : ��`�̐���
		//Obj�̌����W�Ɍv�Z
		RECT makeRectScale(int startx = 0, int starty = 0, int endx = 0, int endy = 0);
		RECT makeRectNoScale(int startx = 0, int starty = 0, int endx = 0, int endy = 0);

	public:
		//object�� -> getObjectFromOOO�Ɏg��
		//drawObj -> render�\��Obj
		//velocity -> �����ړ���
		//isPlayer -> ����Obj���v���C���[���H <-- �v���C���[�ȊO��true��ݒ肵�Ȃ��ŁI�@�i�f�t�H���g��false�j
		MapCollisionObj(const std::string& objectName, const ci_ext::DrawObjf& drawObj, const ci_ext::Vec3f& velocity, bool isPlayer = false);
		virtual ~MapCollisionObj() {}

		//�p�����i�e�j�̃N���X��override -> �������Ȃ�
		void update() override {}
		void render() override {}

	protected:
		//----- �p�������N���X��override���� -----
		//1�t���[�������������Ȃ�

		//�_���[�W�^�C����G��u�Ԃ̏���
		virtual void doOnceHitDamageTile() {}
		//�����^�C����G��u�Ԃ̏���
		virtual void doOnceHitDeadTile() {}

	private:
		//Wind
		bool windBlowX_; //���ɔ�΂����H
		bool windBlowY_;
		int windBlowXCnt_; //�J�E���^�[
		int windBlowYCnt_;
		ci_ext::Vec3f windBlowVelocity_;
		int windDir_;
		bool isHitWind(const POINT& pt);
		bool isHitWind(const RECT& rt);
		void resetWindBlow();
	protected:
		//���ɐG��u�Ԃ̏���
		virtual void doOnceHitWindObj();

	private:
		//Debug
		void drawHitRect(int x, int y, D3DCOLOR c = ARGB(255, 255, 255, 0));
		void drawHitPt(int x, int y, D3DCOLOR c = ARGB(255, 255, 255, 0));

	};
}
