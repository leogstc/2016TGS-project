#include "enemy_boss.h"
#include "../config.h"
#include "../stage/map.h"
#include "mine_navi.h"
#include "enemy_boss_shot.h"
#include "enemy_monster.h"
namespace enemy
{
	Enemy_Boss::Enemy_Boss(int x, int y, int id)
		:
		EnemyBase("enemy_boss", "enemy_boss", x, y, id, 200,40), SPRITE_SIZEX(678), SPRITE_SIZEY(600), MAX_CREATE_ENEMY(3), MAX_SHOT(3)
	{
		body_.setPos(ci_ext::Vec3f(body_.x(), body_.y(), 0.8f));
	}

	Enemy_Boss::~Enemy_Boss()
	{}

	void Enemy_Boss::init()
	{
		auto map = ci_ext::weak_cast<map::Map>(getObjectFromRoot("map"));
		nowInMapName_ = map.lock()->getNowMapName();

		setState(State::STAND);
		body_.setSrcSize(ci_ext::Vec2f(SPRITE_SIZEX, SPRITE_SIZEY));
		dir_ = DIR::LEFT;
		anim_.setAnimReset(6, 10);
		quakecnt = 0;
		cnt = 0;
		Gid_ = 0;
		shotid = 0;
		atkcnt = 0;
		e_max = 0;
		speed = 0.5f;
		maxHp = life_;
		insertAsChild(new Mask("mask_" + body_.resname(), "mask_" + body_.resname(), body_));
	}

	void Enemy_Boss::update()
	{
		//off
		offsetRt_.left = body_.ix() - 378 / 2 + 50;
		offsetRt_.right = body_.ix() + 378 / 2;
		offsetRt_.top = body_.iy() - 300 / 4;
		offsetRt_.bottom = body_.iy() + 250;
		setObjCollisionMapRect(offsetRt_);

		defsetRt_ = offsetRt_;

		if (dir_ == DIR::RIGHT)
		{
			ReverseRt();
		}

		switch (state_)
		{
		case State::STAND:updateStand(); break;
		case State::WALK:updateWalk(); break;
		case State::START_Q:updateStartQuake(); break;
		case State::QUAKE:updateQuake(); break;
		case State::ATKMINE:updateMine(); break;
		case State::ENDMINE:updateMineEnd(); break;
		case State::ATKG:updateAtkG(); break;
		case State::ATKSHOT:updateAtkShot(); break;
		case State::DEAD:updateDead(); break;
		}

		mapCheckMoveXY();

		//アニメ
		anim_.step();
		//body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		//方向	
		body_.setScale(ci_ext::Vec2f((float)dir_ * config.GAME_SCALE, 1.f));

		body_.offsetPos(velocity_);

		
		checkMask();
		checkHitPlayer();
		checkhitAtk();

		auto enemies = getObjectsFromRoot({ "enemy_monster" });
		if (enemies.empty())
		{
			e_max = 0;
		}
	}

	void Enemy_Boss::render()
	{
		body_.render();
#ifdef _DEBUG
		gplib::draw::CkRect(offsetRt_, ARGB(255, 255, 255, 0));
		gplib::draw::CkRect(defsetRt_, ARGB(255, 255, 0, 0));
#endif 
	}

	void Enemy_Boss::updateStand()
	{
		//アニメ
		velocity_.x(0);
		velocity_.offset(ci_ext::Vec3f(0, config.GRAVITY, 0));
		gplib::se::Stop("bosswalk");
		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));

		if (player != nullptr)
		{
			tpos = player->getPos();
			if (tpos.x() < body_.x())
			{
				dir_ = DIR::LEFT;
			}
			else
			{
				dir_ = DIR::RIGHT;
			}

			if (checkUnderFloor())
			{
				if (abs(tpos.x() - body_.x()) > 500)
				{
					setState(State::WALK);
					gplib::se::PlayLoop("bosswalk");
				}
			}
		}


		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 0));

		if (checkUnderFloor())
		{
			velocity_.y(0);
		}

		if (++atkcnt > 180) //攻撃パターン
		{
			state_ = State(atkpatten[rand()%10]);
			setState(state_);
			if (e_max == 0)
			{
				Atkmode_ = ATKMode(gplib::math::GetRandom(0, 1));
				setAtkMode(Atkmode_);
			}
			else
			{
				Atkmode_ = ATKMode::MINE;
				setAtkMode(Atkmode_);
			}
			atkcnt = 0;
			
		}
		if (state_ == State::ATKSHOT)
		{
			create_timer_ = insertAsChild(new game::Timer("generator_timer", 0.5f));
		}
	}

	void Enemy_Boss::updateWalk()
	{
		//歩く
		//velocity_.x(0);
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 600));
		auto player = ci_ext::weak_to_shared<player::Player>(getObjectFromRoot("player"));

		if (player != nullptr)
		{
			tpos = player->getPos();
			if (tpos.x() < body_.x())
			{
				dir_ = DIR::LEFT;
				if (abs(tpos.x() - body_.x()) > 500)
				{
					velocity_.x(-speed);
				}
				else
				{
					setState(State::STAND);
				}
			}
			else
			{
				dir_ = DIR::RIGHT;
				if (abs(tpos.x() - body_.x()) > 500)
				{
					velocity_.x(speed);
				}
				else
				{
					setState(State::STAND);
				}
			}
		}
		mapCheckCollisionX(true);
	}

	//攻撃アニメ
	void Enemy_Boss::updateStartQuake()
	{
		velocity_.x(0);
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 600 * 2));
		if (anim_.isLastFrame())
		{
			setState(State::QUAKE);
			gplib::se::Play("bosswalk");
		}
	}

	void Enemy_Boss::updateQuake()
	{
		//震動
		POINT lookAt = gplib::camera::GetLookAt();

		quakeX = gplib::math::GetRandom(1, 10);
		quakeY = gplib::math::GetRandom(1, 10);
		gplib::camera::SetLookAt((float)lookAt.x + quakeX, (float)lookAt.y + quakeY);
		if (Atkmode_ == ATKMode::MINE)
		{
			if (++quakecnt > 30)
			{
				setState(State::ATKMINE);
				quakecnt = 0;
			}
		}
		else if (Atkmode_ == ATKMode::GENERA)
		{
			if (cnt == 1)
			{
				setState(State::ATKG);
				create_timer_ = insertAsChild(new game::Timer("generator_timer", 0.5f));
				cnt = 0;
			}
			else 
			{
				setState(State::START_Q);
				cnt++;
			}
		}
	}

	void Enemy_Boss::updateMine()
	{
		//mine
		insertAsChild(new shot::MineNavi());
		setState(State::ENDMINE);

	}

	void Enemy_Boss::updateMineEnd() //硬直のため
	{
		auto shotover = getObjectFromMyChildren("minenavi");
		if (!shotover.expired())
		{
			if (shotover.lock()->isDestroy())
			{
				setState(State::STAND);
			}
		}
	}

	//敵生成
	void Enemy_Boss::updateAtkG()
	{
		POINT lookAt = gplib::camera::GetCameraPos();
		if (Gid_ < MAX_CREATE_ENEMY)
		{
			auto create_timer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
			if (create_timer->isPause())
			{
				create_timer->run();
			}
			if (create_timer->isTimeOver())
			{
				genX = gplib::math::GetRandom(0, 1000);
				gplib::se::Play("appear");
				insertAsChild(new Enemy_Monster(lookAt.x + genX, lookAt.y + 100, id_));
				//ci_ext::weak_cast<enemy::EnemyManager>(getParentPtr()).lock()->createEnemy("enemy_monster", lookAt.x + genX, lookAt.y + 100, id_);
				Gid_++;
				e_max++;
				create_timer_.lock()->kill();
				create_timer_ = insertAsChild(new game::Timer("generator_timer", 1.f));
			}
		}
		else
		{
			create_timer_.lock()->pause();//タイマー停止
			setState(State::STAND);
			create_timer_.lock()->kill();
			Gid_ = 0;
		}
	}

	//弾
	void Enemy_Boss::updateAtkShot()
	{
		auto effMngr = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));

		if (shotid < MAX_SHOT)
		{
			auto create_shottimer = ci_ext::weak_to_shared<game::Timer>(create_timer_);
			if (create_shottimer->isPause())
			{
				create_shottimer->run();
			}
			if (create_shottimer->isTimeOver())
			{
				if (!effMngr.expired())
				{
					gplib::se::Play("boss_shot");
					effMngr.lock()->createEffect("shoteffect", body_.ix(), body_.iy() + shotid * 100);
					insertAsChild(new shot::BossShot(body_.ix(), body_.iy() + shotid * 100, 20));
					shotid++;
					create_timer_.lock()->kill();
					create_timer_ = insertAsChild(new game::Timer("generator_timer", 1.f));
				}
			}
		}
		else
		{
			create_timer_.lock()->pause();//タイマー停止
			setState(State::STAND);
			create_timer_.lock()->kill();
			shotid = 0;
		}
	}

	//死亡
	void Enemy_Boss::updateDead()
	{
		body_.setSrc(ci_ext::Vec2f(body_.srcSize().x() * anim_.getAnimNo(), 1800));
		if (anim_.isLastFrame())
		{
			pause();
		}

	}

	void Enemy_Boss::ReverseRt()
	{
		offsetRt_.left = body_.ix() - 378 / 2;
		offsetRt_.right = body_.ix() + 378 / 2 - 50;
		defsetRt_.left= body_.ix() - 378 / 2;
		defsetRt_.right = body_.ix() + 378 / 2 - 50;
		setObjCollisionMapRect(offsetRt_);
	}

	//アニメーション切り替え
	void Enemy_Boss::setStateAnim(State state)
	{
		state_ = state;
		switch (state_)
		{
		case State::STAND: anim_.setAnimReset(6, 10); break;
		case State::WALK: anim_.setAnimReset(9, 15); break;
		case State::START_Q: anim_.setAnimReset(7, 4); break;
		case State::DEAD: anim_.setAnimReset(5, 6); break;
		}
	}

	ci_ext::Vec2f Enemy_Boss::getPos() const
	{
		return{ body_.pos().x(), body_.pos().y() };
	}

	DIR Enemy_Boss::getDir() const
	{
		return dir_;
	}

	void Enemy_Boss::setState(State state)
	{
		//state_ = state;
		setStateAnim(state);
		anim_.resetNo();
	}

	void Enemy_Boss::setAtkMode(ATKMode atkmode)
	{
		Atkmode_ = atkmode;
	}
}