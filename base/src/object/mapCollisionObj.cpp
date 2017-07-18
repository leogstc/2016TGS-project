#include "mapCollisionObj.h" //�K�{
#include "../stage/map.h" //�}�b�v������擾����
#include "../map_object/windManager.h"
#include "../map_object/wind.h"

#define WIND_BLOW_X_CNT_LIMIT 60
#define WIND_BLOW_Y_CNT_LIMIT 3

namespace map {
	MapCollisionObj::MapCollisionObj(const std::string& objectName, const ci_ext::DrawObjf& drawObj, const ci_ext::Vec3f& velocity, bool isPlayer)
		:
		MovableObject(objectName, drawObj, velocity),
		offsetS_(1),
		offsetE_(1),
		isPlayer_(isPlayer), nowInMapName_(""),
		invincible_(false), invincibleCnt_(0),
		windBlowX_(0), windBlowY_(0), windBlowXCnt_(0), windBlowYCnt_(0),
		windBlowVelocity_(ci_ext::Vec3f::zero()), windDir_((int)info::WindDIR::UP)
	{
		objRect_ = makeRectScale();
	}

	//�ړ����x���グ�Ă��A���W��⊮��������s��
	//����p��`�������A���̃T�C�Y�Ŕ�����s���B
	int MapCollisionObj::checkMoveMapLeft()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//���̃}�b�v�ֈړ����H
		if (mapData.lock()->isChangingMap()) {
			return 0;
		}

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//�ړ��ʕ����[�v
		//Check from velocity = 1 until current velocity
		for (int x = -1; x >= floor(velocity_.x()); x--) {
			RECT ckRect = objRect_;

			//Shift (RECT, �� x, �� y)
			OffsetRect(&ckRect, x, (int)velocity_.y());

			//����_
			//Making Checking Point
			const int ptSize = 5;
			POINT pt[ptSize] = {
				{ ckRect.left + offsetS_, ckRect.top + offsetS_ },
				{ ckRect.left + offsetS_, ckRect.top + (int)((float)h / 4.0f) },
				{ ckRect.left + offsetS_, ckRect.top + h / 2 },
				{ ckRect.left + offsetS_, ckRect.top + (int)((float)h * 3.0f / 4.0f) },
				{ ckRect.left + offsetS_, ckRect.bottom - offsetE_ }
			};

			for (int i = 0; i < ptSize; i++) {
				//Check Joint of Changing Map
				if (isPlayer_) {
					mapData.lock()->checkJoints(pt[i]);
				}

				//drawHitPt(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
				//�ǂł���Ȃ�
				int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
				if (mapData.lock()->isCollision(tileId)) {
					if (windBlowX_ && i < ptSize - 1) {
						resetWindBlow();
					}
					if (mapData.lock()->isCustomTile(tileId)) {
						RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
						if (isPointInRect(pt[i], custom)) {
							//���Ԃ������̂ŁA�P�h�b�g�E�̈ʒu�ɏC�� (+1)
							body_.offsetPos(ci_ext::Vec3f((float)(x + 1), 0, 0));
							//Shift �����`
							//OffsetRect(&objRect_, x + 1, 0);

							velocity_.x(0.f);
							//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
							//���G��Ԃł͂Ȃ�
							if (isPlayer_ && !invincible_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.y(0.f);
									doOnceHitDamageTile();
								}
							}
							if (mapData.lock()->isDeadTile(tileId)) {
								doOnceHitDeadTile();
							}
							return i;
						}
					}
					else {
						//���Ԃ������̂ŁA�P�h�b�g�E�̈ʒu�ɏC�� (+1)
						body_.offsetPos(ci_ext::Vec3f((float)(x + 1), 0, 0));
						//Shift �����`
						//OffsetRect(&objRect_, x + 1, 0);

						velocity_.x(0.f);
						//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.y(0.f);
								doOnceHitDamageTile();
							}
						}
						if (mapData.lock()->isDeadTile(tileId)) {
							doOnceHitDeadTile();
						}
						return i;
					}
				}
				else {
					if (mapData.lock()->isDamageTile(tileId)) {
						velocity_.y(0.f);
						doOnceHitDamageTile();
						return i;
					}
					//else if (isHitWind(pt[i]) && !windBlowX_ && !windBlowY_) {
					//	doOnceHitWindObj();
					//	return i;
					//	/*RECT windRt = mapData.lock()->getWindTileRect(nowInMapName_, pt[i].x, pt[i].y);
					//	if (isPointInRect(pt[i], windRt)) {
					//		auto wDir = mapData.lock()->getWindDir(tileId);
					//		if (wDir == info::WindDIR::LEFT || wDir == info::WindDIR::RIGHT)
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), true, false);
					//		else
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), false, true);
					//		return i;
					//	}*/
					//}
				}
			}
		}
		body_.offsetPos(ci_ext::Vec3f(velocity_.x(), 0, 0));
		//Shift �����`
		//OffsetRect(&objRect_, (int)velocity_.x(), 0);
		return -1;
	}

	int MapCollisionObj::checkMoveMapRight()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//���̃}�b�v�ֈړ����H
		if (mapData.lock()->isChangingMap()) {
			return 0;
		}

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//�ړ��ʕ����[�v
		//Check from velocity = 1 until current velocity
		for (int x = 1; x <= ceil(velocity_.x()); x++) {
			RECT ckRect = objRect_;

			//Shift (RECT, �� x, �� y)
			OffsetRect(&ckRect, x, (int)velocity_.y());

			//����_
			//Making Checking Point
			const int ptSize = 5;
			POINT pt[ptSize] = {
				{ ckRect.right - offsetE_, ckRect.top + offsetS_ },
				{ ckRect.right - offsetE_, ckRect.top + (int)((float)h / 4.0f) },
				{ ckRect.right - offsetE_, ckRect.top + h / 2 },
				{ ckRect.right - offsetE_, ckRect.top + (int)((float)h * 3.0f / 4.0f) },
				{ ckRect.right - offsetE_, ckRect.bottom - offsetE_ }
			};

			for (int i = 0; i < ptSize; i++) {
				//Check Joint of Changing Map
				if (isPlayer_) {
					mapData.lock()->checkJoints(pt[i]);
				}

				//drawHitPt(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
				//�ǂł���Ȃ�
				int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
				if (mapData.lock()->isCollision(tileId)) {
					if (windBlowX_ && i < ptSize - 1) {
						resetWindBlow();
					}
					if (mapData.lock()->isCustomTile(tileId)) {
						//(-)���W��RECT�o�O���C�� <-- -0.99~0.99�̊Ԃ̌v�Z�͓�������
						if (pt[i].x < 0)
							pt[i].x += offsetE_;
						//End �C��
						RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
						if (isPointInRect(pt[i], custom)) {
							//���Ԃ������̂ŁA�P�h�b�g���̈ʒu�ɏC�� (-1)
							body_.offsetPos(ci_ext::Vec3f((float)(x - 1), 0, 0));
							//Shift �����`
							//OffsetRect(&objRect_, x - 1, 0);

							velocity_.x(0.f);
							//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
							//���G��Ԃł͂Ȃ�
							if (isPlayer_ && !invincible_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.y(0.f);
									doOnceHitDamageTile();
								}
							}
							if (mapData.lock()->isDeadTile(tileId)) {
								doOnceHitDeadTile();
							}
							return i;
						}
					}
					else {
						//���Ԃ������̂ŁA�P�h�b�g���̈ʒu�ɏC�� (-1)
						body_.offsetPos(ci_ext::Vec3f((float)(x - 1), 0, 0));
						//Shift �����`
						//OffsetRect(&objRect_, x - 1, 0);

						velocity_.x(0.f);
						//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.y(0.f);
								doOnceHitDamageTile();
							}
						}
						if (mapData.lock()->isDeadTile(tileId)) {
							doOnceHitDeadTile();
						}
						return i;
					}
				}
				else {
					if (mapData.lock()->isDamageTile(tileId)) {
						velocity_.y(0.f);
						doOnceHitDamageTile();
						return i;
					}
					//else if (isHitWind(pt[i]) && !windBlowX_ && !windBlowY_) {
					//	doOnceHitWindObj();
					//	return i;
					//	/*RECT windRt = mapData.lock()->getWindTileRect(nowInMapName_, pt[i].x, pt[i].y);
					//	if (isPointInRect(pt[i], windRt)) {
					//		auto wDir = mapData.lock()->getWindDir(tileId);
					//		if (wDir == info::WindDIR::LEFT || wDir == info::WindDIR::RIGHT)
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), true, false);
					//		else
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), false, true);
					//		return i;
					//	}*/
					//}
				}
			}
		}
		body_.offsetPos(ci_ext::Vec3f(velocity_.x(), 0, 0));
		//Shift �����`
		//OffsetRect(&objRect_, (int)velocity_.x(), 0);
		return -1;
	}

	int MapCollisionObj::checkMoveMapUp() {
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//���̃}�b�v�ֈړ����H
		if (mapData.lock()->isChangingMap()) {
			return 0;
		}

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//�ړ��ʕ����[�v
		//Check from velocity = 0 until current velocity
		for (int y = 0; y >= floor(velocity_.iy()); y--) {
			RECT ckRect = objRect_;

			//Shift (RECT, �� x, �� y)
			OffsetRect(&ckRect, (int)velocity_.x(), y);

			//����_
			//Making Checking Point
			const int ptSize = 5;
			POINT pt[ptSize] = {
				{ ckRect.left + offsetS_, ckRect.top + offsetS_ },
				{ ckRect.left + (int)((float)w / 4.0f), ckRect.top + offsetS_ },
				{ ckRect.left + w / 2, ckRect.top + offsetS_ },
				{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.top + offsetS_ },
				{ ckRect.right - offsetE_, ckRect.top + offsetS_ },
			};

			for (int i = 0; i < ptSize; i++) {
				//Check Joint of Changing Map
				if (isPlayer_) {
					mapData.lock()->checkJoints(pt[i]);
				}

				//�ǂł���Ȃ�
				int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
				if (mapData.lock()->isCollision(tileId)) {
					if (mapData.lock()->isCustomTile(tileId)) {
						RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
						if (isPointInRect(pt[i], custom)) {
							//���Ԃ������̂ŁA�P�h�b�g���̈ʒu�ɏC�� (+1)
							body_.offsetPos(ci_ext::Vec3f(0, (float)(y + 1), 0));
							//Shift �����`
							//OffsetRect(&objRect_, 0, y + 1);

							velocity_.y(0.f);
							//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
							//���G��Ԃł͂Ȃ�
							if (isPlayer_ && !invincible_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.x(0.f);
									doOnceHitDamageTile();
								}
							}
							if (mapData.lock()->isDeadTile(tileId)) {
								doOnceHitDeadTile();
							}
							return i;
						}
					}
					else {
						//���Ԃ������̂ŁA�P�h�b�g���̈ʒu�ɏC�� (+1)
						body_.offsetPos(ci_ext::Vec3f(0, (float)(y + 1), 0));
						//Shift �����`
						//OffsetRect(&objRect_, 0, y + 1);

						velocity_.y(0.f);
						//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						if (mapData.lock()->isDeadTile(tileId)) {
							doOnceHitDeadTile();
						}
						return i;
					}
				}
				else {
					if (mapData.lock()->isDamageTile(tileId)) {
						velocity_.x(0.f);
						doOnceHitDamageTile();
						return i;
					}
					//else if (isHitWind(pt[i]) && !windBlowX_ && !windBlowY_) {
					//	doOnceHitWindObj();
					//	return i;
					//	/*RECT windRt = mapData.lock()->getWindTileRect(nowInMapName_, pt[i].x, pt[i].y);
					//	if (isPointInRect(pt[i], windRt)) {
					//		auto wDir = mapData.lock()->getWindDir(tileId);
					//		if (wDir == info::WindDIR::LEFT || wDir == info::WindDIR::RIGHT)
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), true, false);
					//		else
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), false, true);
					//		return i;
					//	}*/
					//}
				}
			}
		}
		body_.offsetPos(ci_ext::Vec3f(0, velocity_.y(), 0));
		//Shift �����`
		//OffsetRect(&objRect_, 0, (int)velocity_.y());
		return -1;
	}

	int MapCollisionObj::checkMoveMapDown()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//���̃}�b�v�ֈړ����H
		if (mapData.lock()->isChangingMap()) {
			return 0;
		}

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//�ړ��ʕ����[�v
		//Check from velocity = 0 until current velocity
		for (int y = 0; y <= ceil(velocity_.y()); y++) {
			RECT ckRect = objRect_;

			//Shift (RECT, �� x, �� y)
			OffsetRect(&ckRect, (int)velocity_.x(), y);

			//����_
			//Making Checking Point
			const int ptSize = 5;
			POINT pt[ptSize] = {
				{ ckRect.left + offsetS_, ckRect.bottom - offsetE_ },
				{ ckRect.left + (int)((float)w / 4.0f), ckRect.bottom - offsetE_ },
				{ ckRect.left + w / 2, ckRect.bottom - offsetE_ },
				{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.bottom - offsetE_ },
				{ ckRect.right - offsetE_, ckRect.bottom - offsetE_ },
			};

			for (int i = 0; i < ptSize; i++) {
				//Check Joint of Changing Map
				if (isPlayer_) {
					mapData.lock()->checkJoints(pt[i]);
				}

				//drawHitPt(pt[i].x, pt[i].y);

				//////�󔒂łȂ��Ȃ灁�ǂł���Ȃ�
				//////if (mapData.lock()->getChip(pt[i].x, pt[i].y) != 0)

				//�ǂł���Ȃ�
				int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
				if (mapData.lock()->isCollision(tileId)) {
					if (mapData.lock()->isCustomTile(tileId)) {
						//(-)���W��RECT�o�O���C�� <-- -0.99~0.99�̊Ԃ̌v�Z�͓�������
						if (pt[i].y < 0)
							pt[i].y += offsetE_;
						//End �C��
						RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
						if (isPointInRect(pt[i], custom)) {
							//���Ԃ������̂ŁA�P�h�b�g��̈ʒu�ɏC�� (-1)
							body_.offsetPos(ci_ext::Vec3f(0, (float)(y - 1), 0));
							//Shift �����`
							//OffsetRect(&objRect_, 0, y - 1);

							velocity_.y(0.f);
							//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
							//���G��Ԃł͂Ȃ�
							if (isPlayer_ && !invincible_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.x(0.f);
									doOnceHitDamageTile();
								}
							}
							if (mapData.lock()->isDeadTile(tileId)) {
								doOnceHitDeadTile();
							}
							return i;
						}
					}
					else {
						//���Ԃ������̂ŁA�P�h�b�g��̈ʒu�ɏC�� (-1)
						body_.offsetPos(ci_ext::Vec3f(0, (float)(y - 1), 0));
						//Shift �����`
						//OffsetRect(&objRect_, 0, y - 1);

						velocity_.y(0.f);
						//drawHitRect(pt[i].x, pt[i].y, ARGB(255, 0, 255, 0));
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						if (mapData.lock()->isDeadTile(tileId)) {
							doOnceHitDeadTile();
						}
						return i;
					}
				}
				else {
					if (mapData.lock()->isDamageTile(tileId)) {
						velocity_.x(0.f);
						doOnceHitDamageTile();
						return i;
					}
					//else if (isHitWind(pt[i]) && !windBlowX_ && !windBlowY_) {
					//	doOnceHitWindObj();
					//	return i;
					//	/*RECT windRt = mapData.lock()->getWindTileRect(nowInMapName_, pt[i].x, pt[i].y);
					//	if (isPointInRect(pt[i], windRt)) {
					//		auto wDir = mapData.lock()->getWindDir(tileId);
					//		if (wDir == info::WindDIR::LEFT || wDir == info::WindDIR::RIGHT)
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), true, false);
					//		else
					//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), false, true);
					//		return i;
					//	}*/
					//}
				}
			}
		}
		body_.offsetPos(ci_ext::Vec3f(0, velocity_.y(), 0));
		//Shift �����`
		//OffsetRect(&objRect_, 0, (int)velocity_.y());
		return -1;
	}

	bool map::MapCollisionObj::checkCollisionMapLeft()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ objRect_.left, objRect_.top },
			{ objRect_.left, objRect_.top + (int)((float)h / 4.0f) },
			{ objRect_.left, objRect_.top + h / 2 },
			{ objRect_.left, objRect_.top + (int)((float)h * 3.0f / 4.0f) },
			{ objRect_.left, objRect_.bottom }
		};

		for (int i = 0; i < ptSize; ++i)
		{
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x,pt[i].y);
			if (mapData.lock()->isCollision(tileId))
				return true;
		}
		return false;
	}

	bool map::MapCollisionObj::checkCollisionMapRight()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ objRect_.right, objRect_.top },
			{ objRect_.right, objRect_.top + (int)((float)h / 4.0f) },
			{ objRect_.right, objRect_.top + h / 2 },
			{ objRect_.right, objRect_.top + (int)((float)h * 3.0f / 4.0f) },
			{ objRect_.right, objRect_.bottom }
		};

		for (int i = 0; i < ptSize; ++i)
		{
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId))
				return true;
		}
		return false;
	}

	bool map::MapCollisionObj::checkCollisionMapUp()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ objRect_.left, objRect_.top},
			{ objRect_.left + (int)((float)w / 4.0f), objRect_.top},
			{ objRect_.left + w / 2, objRect_.top},
			{ objRect_.left + (int)((float)w * 3.0f / 4.0f), objRect_.top},
			{ objRect_.right, objRect_.top},
		};

		for (int i = 0; i < ptSize; ++i)
		{
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId))
				return true;
		}
		return false;
	}

	bool map::MapCollisionObj::checkCollisionMapDown()
	{
		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ objRect_.left, objRect_.bottom },
			{ objRect_.left + (int)((float)w / 4.0f), objRect_.bottom },
			{ objRect_.left + w / 2, objRect_.top },
			{ objRect_.left + (int)((float)w * 3.0f / 4.0f), objRect_.bottom },
			{ objRect_.right, objRect_.bottom },
		};

		for (int i = 0; i < ptSize; ++i)
		{
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId))
				return true;
		}
		return false;
	}

	int MapCollisionObj::checkMoveMapX()
	{
		if (velocity_.x() < 0.f) {
			return checkMoveMapLeft();
		}
		else if (velocity_.x() > 0.f) {
			return checkMoveMapRight();
		}
		return -1;
	}

	int MapCollisionObj::checkMoveMapY()
	{
		if (velocity_.y() < 0.f)
			return checkMoveMapUp();
		else if (velocity_.y() >= 0.f)
			return checkMoveMapDown();
		return -1;
	}

	bool map::MapCollisionObj::checkCollisionMapX()
	{
		if (velocity_.y() < 0.f)
			return checkCollisionMapLeft();
		else if (velocity_.y() >= 0.f)
			return checkCollisionMapRight();
		return false;
	}

	bool map::MapCollisionObj::checkCollisionMapY()
	{
		if (velocity_.y() < 0.f)
			return checkCollisionMapUp();
		else if (velocity_.y() >= 0.f)
			return checkCollisionMapDown();
		return false;
	}


	//�����̒n�ʂ𒲂ׂ�
	//���������Ԃ�true���߂�
	bool MapCollisionObj::checkUnderFloor()
	{
		auto map = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		//���̃}�b�v�ֈړ����Ȃ�
		if (map.lock()->isChangingMap()) {
			auto player = ci_ext::weak_cast<player::Player>(map.lock()->getObjectFromChildren("player"));
			//������
			if (player.lock()->getPlayerState() == player::State::FALL)
				return false;
			//���A�ړ���
			else
				return true;
		}

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;

		//�����̏󋵃`�F�b�N
		//���[�ƉE�[�̏󋵂������Ă����B
		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.left + offsetS_, ckRect.bottom + 1 },
			{ ckRect.left + (int)((float)w / 4.0f), ckRect.bottom + 1 },
			{ ckRect.left + w / 2, ckRect.bottom + 1 },
			{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.bottom + 1 },
			{ ckRect.right - offsetE_, ckRect.bottom + 1 },
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			//drawHitPt(pt[i].x, pt[i].y);
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				//(-)���W��RECT�o�O���C�� <-- -0.99~0.99�̊Ԃ̌v�Z�͓�������
				if (pt[i].y < 0)
					pt[i].y += offsetE_;
				//End �C��
				if (mapData.lock()->isCustomTile(tileId)) {
					RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
					if (isPointInRect(pt[i], custom)) {
						//drawHitRect(pt[i].x, pt[i].y);
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						if (mapData.lock()->isDeadTile(tileId)) {
							doOnceHitDeadTile();
						}
						return true;
					}
				}
				else {
					//drawHitRect(pt[i].x, pt[i].y);
					//���G��Ԃł͂Ȃ�
					if (isPlayer_ && !invincible_) {
						//�_���[�W�^�C����HIT!!
						if (mapData.lock()->isDamageTile(tileId)) {
							velocity_.x(0.f);
							doOnceHitDamageTile();
						}
					}
					if (mapData.lock()->isDeadTile(tileId)) {
						doOnceHitDeadTile();
					}
					return true;
				}
			}
			else {
				if (mapData.lock()->isDamageTile(tileId)) {
					velocity_.x(0.f);
					doOnceHitDamageTile();
					return true;
				}
				//else if (isHitWind(pt[i]) && !windBlowX_ && !windBlowY_) {
				//	doOnceHitWindObj();
				//	return true;
				//	/*RECT windRt = mapData.lock()->getWindTileRect(nowInMapName_, pt[i].x, pt[i].y);
				//	if (isPointInRect(pt[i], windRt)) {
				//		auto wDir = mapData.lock()->getWindDir(tileId);
				//		if (wDir == info::WindDIR::LEFT || wDir == info::WindDIR::RIGHT)
				//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), true, false);
				//		else
				//			doOnceHitWindTile(mapData.lock()->getWindForce(wDir), false, true);
				//		return true;
				//	}*/
				//}
			}
		}
		return false;
	}

	//����̒n�ʂ𒲂ׂ�
	//���������Ԃ͐^���߂�
	bool MapCollisionObj::checkHeadFloor()
	{
		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.left + offsetS_, ckRect.top + offsetS_ },
			{ ckRect.left + (int)((float)w / 4.0f), ckRect.top + offsetS_ },
			{ ckRect.left + w / 2, ckRect.top + offsetS_ },
			{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.top + offsetS_ },
			{ ckRect.right - offsetE_, ckRect.top + offsetS_ },
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				if (mapData.lock()->isCustomTile(tileId)) {
					RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
					if (isPointInRect(pt[i], custom)) {
						//���G��Ԃł͂Ȃ�
						if (isPlayer_ && !invincible_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						return true;
					}
				}
				else {
					//���G��Ԃł͂Ȃ�
					if (isPlayer_ && !invincible_) {
						//�_���[�W�^�C����HIT!!
						if (mapData.lock()->isDamageTile(tileId)) {
							velocity_.x(0.f);
							doOnceHitDamageTile();
						}
					}
					return true;
				}
			}
			else if (mapData.lock()->isDamageTile(tileId)) {
				velocity_.x(0.f);
				doOnceHitDamageTile();
				return true;
			}
		}
		body_.offsetPos(ci_ext::Vec3f(0, velocity_.y(), 0));
		return false;
	}

	void MapCollisionObj::mapCheckMoveXY()
	{
		auto map = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		if (!map.lock()->isChangingMap()) {
			if (windBlowX_) {
				velocity_.offset(windBlowVelocity_);
				windBlowXCnt_++;

				if (windBlowXCnt_ >= WIND_BLOW_X_CNT_LIMIT) {
					windBlowXCnt_ = 0;
					windBlowX_ = false;
				}
			}
			if (windBlowY_) {
				windBlowYCnt_++;
				if (windBlowYCnt_ >= WIND_BLOW_Y_CNT_LIMIT) {
					windBlowYCnt_ = 0;
					windBlowY_ = false;
				}
			}
			if (!windBlowX_ && !windBlowY_ && isHitWind(objRect_)) {
			//if (!windBlowY_ && isHitWind(objRect_)) {
				doOnceHitWindObj();
			}

			checkMoveMapX();
			checkMoveMapY();
		}
	}

	//�����F x = Offset X | y = Offset Y | opposite = ���Α����`�F�b�N���Ă����H
	int MapCollisionObj::collisionMapLeft(int x, int y, bool opposite)
	{
		RECT intersect;

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;
		OffsetRect(&ckRect, x, y);

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.left + offsetS_, ckRect.top + offsetS_ },
			{ ckRect.left + offsetS_, ckRect.top + (int)((float)h / 4.0f) },
			{ ckRect.left + offsetS_, ckRect.top + h / 2 },
			{ ckRect.left + offsetS_, ckRect.top + (int)((float)h * 3.0f / 4.0f) },
			{ ckRect.left + offsetS_, ckRect.bottom - offsetE_ }
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				if (mapData.lock()->isCustomTile(tileId)) {
					RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
					if (isPointInRect(pt[i], custom)) {
						IntersectRect(&intersect, &ckRect, &custom);
						//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
						int len = intersect.right - intersect.left;

						//�܂����Α����`�F�b�N���Ă��Ȃ�
						if (opposite == false) {
							//Shift & Check Collision at opposite site
							int oppositeCollisionLen = collisionMapRight(len, y, true);
							if (oppositeCollisionLen == 0) {
								return len; //No opposite collision -> return this collision length
							}
							else {
								return -1; //Both collision
							}
						}
						//���Α��`�F�b�N�ς�
						else {
							if (isPlayer_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.y(0.f);
									doOnceHitDamageTile();
								}
							}
							return len; //return this collision length
						}
					}
				}
				else {
					RECT blockRt = mapData.lock()->getTileRect(pt[i].x, pt[i].y);
					IntersectRect(&intersect, &ckRect, &blockRt);
					//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
					int len = intersect.right - intersect.left;

					//�܂����Α����`�F�b�N���Ă��Ȃ�
					if (opposite == false) {
						//Shift & Check Collision at opposite site
						int oppositeCollisionLen = collisionMapRight(len, y, true);
						if (oppositeCollisionLen == 0) {
							return len; //No opposite collision -> return this collision length
						}
						else {
							return -1; //Both collision
						}
					}
					//���Α��`�F�b�N�ς�
					else {
						if (isPlayer_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.y(0.f);
								doOnceHitDamageTile();
							}
						}
						return len; //return this collision length
					}
				} //End if Custom Tile
			} //End if Collision Tile
		} //End loop for
		//No Collision
		return 0;
	}

	int MapCollisionObj::collisionMapRight(int x, int y, bool opposite)
	{
		RECT intersect;

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;
		OffsetRect(&ckRect, x, y);

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.right - offsetE_, ckRect.top + offsetS_ },
			{ ckRect.right - offsetE_, ckRect.top + (int)((float)h / 4.0f) },
			{ ckRect.right - offsetE_, ckRect.top + h / 2 },
			{ ckRect.right - offsetE_, ckRect.top + (int)((float)h * 3.0f / 4.0f) },
			{ ckRect.right - offsetE_, ckRect.bottom - offsetE_ }
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				if (mapData.lock()->isCustomTile(tileId)) {
					//(-)���W��RECT�o�O���C�� <-- -0.99~0.99�̊Ԃ̌v�Z�͓�������
					if (pt[i].x < 0)
						pt[i].x += offsetE_;
					//End �C��
					RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
					if (isPointInRect(pt[i], custom)) {
						IntersectRect(&intersect, &ckRect, &custom);
						//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
						int len = intersect.right - intersect.left;

						//�܂����Α����`�F�b�N���Ă��Ȃ�
						if (opposite == false) {
							//Shift & Check Collision at opposite site
							int oppositeCollisionLen = collisionMapLeft(-len, y, true);
							if (oppositeCollisionLen == 0) {
								return len; //No opposite collision -> return this collision length
							}
							else {
								return -1; //Both collision
							}
						}
						//���Α��`�F�b�N�ς�
						else {
							if (isPlayer_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.y(0.f);
									doOnceHitDamageTile();
								}
							}
							return len; //return this collision length
						}
					}
				}
				else {
					RECT blockRt = mapData.lock()->getTileRect(pt[i].x, pt[i].y);
					IntersectRect(&intersect, &ckRect, &blockRt);
					//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
					int len = intersect.right - intersect.left;

					//�܂����Α����`�F�b�N���Ă��Ȃ�
					if (opposite == false) {
						//Shift & Check Collision at opposite site
						int oppositeCollisionLen = collisionMapLeft(-len, y, true);
						if (oppositeCollisionLen == 0) {
							return len; //No opposite collision -> return this collision length
						}
						else {
							return -1; //Both collision
						}
					}
					//���Α��`�F�b�N�ς�
					else {
						if (isPlayer_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.y(0.f);
								doOnceHitDamageTile();
							}
						}
						return len; //return this collision length
					}
				} //End if Custom Tile
			} //End if Collision Tile
		} //End loop for
		//No Collision
		return 0;
	}

	int MapCollisionObj::collisionMapTop(int x, int y, bool opposite)
	{
		RECT intersect;

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;
		OffsetRect(&ckRect, x, y);

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.left + offsetS_, ckRect.top + offsetS_ },
			{ ckRect.left + (int)((float)w / 4.0f), ckRect.top + offsetS_ },
			{ ckRect.left + w / 2, ckRect.top + offsetS_ },
			{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.top + offsetS_ },
			{ ckRect.right - offsetE_, ckRect.top + offsetS_ },
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				if (mapData.lock()->isCustomTile(tileId)) {
					RECT custom = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);
					if (isPointInRect(pt[i], custom)) {
						IntersectRect(&intersect, &ckRect, &custom);
						//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
						int len = intersect.bottom - intersect.top;

						//�܂����Α����`�F�b�N���Ă��Ȃ�
						if (opposite == false) {
							//Shift & Check Collision at opposite site
							int oppositeCollisionLen = collisionMapBottom(x, len, true);
							if (oppositeCollisionLen == 0) {
								return len; //No opposite collision -> return this collision length
							}
							else {
								return -1; //Both collision
							}
						}
						//���Α��`�F�b�N�ς�
						else {
							if (isPlayer_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.x(0.f);
									doOnceHitDamageTile();
								}
							}
							return len; //return this collision length
						}
					}
				}
				else {
					RECT blockRt = mapData.lock()->getTileRect(pt[i].x, pt[i].y);
					IntersectRect(&intersect, &ckRect, &blockRt);
					//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
					int len = intersect.bottom - intersect.top;

					//�܂����Α����`�F�b�N���Ă��Ȃ�
					if (opposite == false) {
						//Shift & Check Collision at opposite site
						int oppositeCollisionLen = collisionMapBottom(x, len, true);
						if (oppositeCollisionLen == 0) {
							return len; //No opposite collision -> return this collision length
						}
						else {
							return -1; //Both collision
						}
					}
					//���Α��`�F�b�N�ς�
					else {
						if (isPlayer_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						return len; //return this collision length
					}
				} //End if Custom Tile
			} //End if Collision Tile
		} //End loop for
		//No Collision
		return 0;
	}

	//Bottom�`�F�b�N�����͓���CustomRect
	int MapCollisionObj::collisionMapBottom(int x, int y, bool opposite)
	{
		RECT intersect;

		//�����`�̕��������߂Ă���
		int w = objRect_.right - objRect_.left;
		int h = objRect_.bottom - objRect_.top;

		RECT ckRect = objRect_;
		OffsetRect(&ckRect, x, y);

		//����_
		//Making Checking Point
		const int ptSize = 5;
		POINT pt[ptSize] = {
			{ ckRect.left + offsetS_, ckRect.bottom - offsetE_ },
			{ ckRect.left + (int)((float)w / 4.0f), ckRect.bottom - offsetE_ },
			{ ckRect.left + w / 2, ckRect.bottom - offsetE_ },
			{ ckRect.left + (int)((float)w * 3.0f / 4.0f), ckRect.bottom - offsetE_ },
			{ ckRect.right - offsetE_, ckRect.bottom - offsetE_ },
		};

		auto mapData = ci_ext::weak_cast<Map>(getObjectFromRoot("map"));
		for (int i = 0; i < ptSize; i++) {
			//�ǂł���Ȃ�
			int tileId = mapData.lock()->getChip(nowInMapName_, pt[i].x, pt[i].y);
			if (mapData.lock()->isCollision(tileId)) {
				if (mapData.lock()->isCustomTile(tileId)) {
					//(-)���W��RECT�o�O���C�� <-- -0.99~0.99�̊Ԃ̌v�Z�͓�������
					if (pt[i].y < 0)
						pt[i].y += offsetE_;
					//End �C��

					RECT customData = mapData.lock()->getCustomTileRect(nowInMapName_, pt[i].x, pt[i].y, tileId);

					//�ˑR��`���ς���Ă��AObj�������Ȃ�
					RECT custom = {
						customData.left,
						customData.top,
						customData.right,
						customData.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE),
					};
					//gplib::draw::CkRect(custom, ARGB(255, 255, 255, 0));
					if (isPointInRect(pt[i], custom)) {
						IntersectRect(&intersect, &ckRect, &custom);
						//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
						int len = intersect.bottom - intersect.top;

						//�܂����Α����`�F�b�N���Ă��Ȃ�
						if (opposite == false) {
							//Shift & Check Collision at opposite site
							int oppositeCollisionLen = collisionMapTop(x, -len, true);
							if (oppositeCollisionLen == 0) {
								return len; //No opposite collision -> return this collision length
							}
							else {
								return -1; //Both collision
							}
						}
						//���Α��`�F�b�N�ς�
						else {
							if (isPlayer_) {
								//�_���[�W�^�C����HIT!!
								if (mapData.lock()->isDamageTile(tileId)) {
									velocity_.x(0.f);
									doOnceHitDamageTile();
								}
							}
							return len; //return this collision length
						}
					}
				}
				else {
					RECT blockRt = mapData.lock()->getTileRect(pt[i].x, pt[i].y);
					IntersectRect(&intersect, &ckRect, &blockRt);
					//gplib::draw::CkRect(intersect, ARGB(255,0,255,255));
					int len = intersect.bottom - intersect.top;

					//�܂����Α����`�F�b�N���Ă��Ȃ�
					if (opposite == false) {
						//Shift & Check Collision at opposite site
						int oppositeCollisionLen = collisionMapTop(x, -len, true);
						if (oppositeCollisionLen == 0) {
							return len; //No opposite collision -> return this collision length
						}
						else {
							return -1; //Both collision
						}
					}
					//���Α��`�F�b�N�ς�
					else {
						if (isPlayer_) {
							//�_���[�W�^�C����HIT!!
							if (mapData.lock()->isDamageTile(tileId)) {
								velocity_.x(0.f);
								doOnceHitDamageTile();
							}
						}
						return len; //return this collision length
					}
				} //End if Custom Tile
			} //End if Collision Tile
		} //End loop for
		//No Collision
		return 0;
	}

	bool MapCollisionObj::mapCheckCollisionX(bool adjust) {
		//true -> collision
		//false -> no collision

		int x = 0;

		x = collisionMapLeft();
		if (x == -1) {
			invincible_ = false; //Remove invincible mode <-- Debug
			return true; //Both side collision
		}
		else {
			//1 side collision
			if (adjust && x != 0) //Adjust position
				body_.offsetPos(ci_ext::Vec3f(x, 0, 0));
		}

		x = collisionMapRight();
		if (x == -1) {
			invincible_ = false; //Remove invincible mode <-- Debug
			return true; //Both side collision
		}
		else {
			//1 side collision
			if (adjust && x != 0) //Adjust position
				body_.offsetPos(ci_ext::Vec3f(-x, 0, 0));
		}

		return false;
	}

	bool MapCollisionObj::mapCheckCollisionY(bool adjust) {
		//true -> collision
		//false -> no collision

		int y = 0;

		y = collisionMapBottom();
		if (y == -1) {
			invincible_ = false; //Remove invincible mode <-- Debug
			return true; //Both side collision
		}
		else {
			//1 side collision
			if (adjust && y != 0) //Adjust position
				body_.offsetPos(ci_ext::Vec3f(0, -y, 0));
		}

		y = collisionMapTop();
		if (y == -1) {
			invincible_ = false; //Remove invincible mode <-- Debug
			return true; //Both side collision
		}
		else {
			//1 side collision
			if (adjust && y != 0) //Adjust position
				body_.offsetPos(ci_ext::Vec3f(0, y, 0));
		}

		return false;
	}

	bool MapCollisionObj::mapCheckCollision4Way(bool adjust)
	{
		//true -> collision x-axis OR y-axis
		//false -> no collision

		bool bothX = false, bothY = false;

		bothX = mapCheckCollisionX(adjust);
		bothY = mapCheckCollisionY(adjust);

		if (bothX && bothY)
			return true;

		return false;
	}

	void MapCollisionObj::setObjCollisionMapRect(int offsetSx, int offsetSy, int offsetEx, int offsetEy)
	{
		objRect_ = makeRectScale(offsetSx, offsetSy, offsetEx, offsetEy);
	}

	void MapCollisionObj::setObjCollisionMapRect(const RECT & rect, bool isOffsetRect)
	{
		if (isOffsetRect)
			setObjCollisionMapRect(rect.left, rect.top, rect.right, rect.bottom);
		else
			objRect_ = rect;
	}

	RECT MapCollisionObj::getObjCollisionMapRect()
	{
		return objRect_;
	}

	//RECT Making : ��`�̐���
	//Obj�̌����W�Ɍv�Z
	RECT MapCollisionObj::makeRectScale(int startx, int starty, int endx, int endy) {
		RECT rt =
		{
			LONG(body_.pos().ix() - (body_.srcSize().x() / 2) * abs(body_.scale().x()) + (startx * abs(body_.scale().x()))),
			LONG(body_.pos().iy() - (body_.srcSize().y() / 2) * abs(body_.scale().y()) + (starty * abs(body_.scale().y()))),
			LONG(body_.pos().ix() + (body_.srcSize().x() / 2) * abs(body_.scale().x()) - (endx * abs(body_.scale().x()))),
			LONG(body_.pos().iy() + (body_.srcSize().y() / 2) * abs(body_.scale().y()) - (endy * abs(body_.scale().y())))
		};
		return rt;
	}

	RECT MapCollisionObj::makeRectNoScale(int startx, int starty, int endx, int endy) {
		RECT rt =
		{
			LONG(body_.pos().ix() - (body_.srcSize().x() / 2) + startx),
			LONG(body_.pos().iy() - (body_.srcSize().y() / 2) + starty),
			LONG(body_.pos().ix() + (body_.srcSize().x() / 2) - endx),
			LONG(body_.pos().iy() + (body_.srcSize().y() / 2) - endy)
		};
		return rt;
	}

	bool MapCollisionObj::isHitWind(const POINT& pt)
	{
		auto winds = getObjectFromRoot("wind_manager").lock()->getChildren();
		for (auto& w : winds) {
			auto wind = ci_ext::weak_cast<object::WindBase>(w);
			RECT windRt = wind.lock()->getObjCollisionMapRect();
			if (isPointInRect(pt, windRt)) {
				windBlowVelocity_ = wind.lock()->getWindForce();
				windDir_ = (int)wind.lock()->getWindDir();
				return true;
			}
		}
		return false;
	}

	bool MapCollisionObj::isHitWind(const RECT& rt)
	{
		auto winds = getObjectFromRoot("wind_manager").lock()->getChildren();
		for (auto& w : winds) {
			auto wind = ci_ext::weak_cast<object::WindBase>(w);
			RECT windRt = wind.lock()->getObjCollisionMapRect();
			RECT intersect;
			IntersectRect(&intersect, &windRt, &rt);
			if (intersect.right - intersect.left > 0) {
				windBlowVelocity_ = wind.lock()->getWindForce();
				windDir_ = (int)wind.lock()->getWindDir();
				return true;
			}
		}
		return false;
	}

	void MapCollisionObj::doOnceHitWindObj()
	{
		if (windDir_ == (int)info::WindDIR::LEFT || windDir_ == (int)info::WindDIR::RIGHT) {
			windBlowX_ = true;
			windBlowXCnt_ = 0;
		}
		else {
			velocity_.y(0); //�d�̓��Z�b�g
			windBlowY_ = true;
			windBlowYCnt_ = 0;
		}
		velocity_.offset(windBlowVelocity_);
	}

	void MapCollisionObj::resetWindBlow()
	{
		windBlowX_ = false;
		windBlowXCnt_ = 0;
		windBlowY_ = false;
		windBlowYCnt_ = 0;
		windDir_ = info::WindDIR::UP;
		windBlowVelocity_ = ci_ext::Vec3f::zero();
	}

	//--------------------------------------------------
	// Debug
	//--------------------------------------------------
	void MapCollisionObj::drawHitRect(int x, int y, D3DCOLOR c)
	{
		RECT rt;
		//SetRect(&rt, x / 32 * 32, y / 32 * 32, (x / 32 * 32) + 32, (y / 32 * 32) + 32);
		SetRect(&rt, x / 30 * 30, y / 30 * 30, (x / 30 * 30) + 30, (y / 30 * 30) + 30);
		//gplib::draw::CkRect(rt, c);
	}

	void MapCollisionObj::drawHitPt(int x, int y, D3DCOLOR c)
	{
		RECT rt;
		SetRect(&rt, x, y, x + 2, y + 2);
		//gplib::draw::CkRect(rt, c);
	}

}