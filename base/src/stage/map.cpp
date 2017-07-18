/*
タイルID
-3	| 即死　（非表示）
-2	| Collision　（非表示）
-1	| Non-Collision・通過　（非表示）
0	| Non-Collision
1~	| マップ設定
*/

#define TILE_ID_DEAD -3
#define TILE_ID_COLLISION_NO_DISP -2
#define TILE_ID_NON_COLLISION_NO_DISP -1

#define BG_TILE_MULTIPLE 1000

#include "map.h"

#include "../player/player.h" //プレイヤーの座標・状態を取得ため
#include "../enemy/bound_object.h"
#include "../ui/bossUiHPFrame.h"
#include "../event/eventTutorial.h"
#include "camera.h"
#include "jointDoor.h" //扉のグラフィックと開閉処理
//#include "../scene/gameOver.hpp"
#include "../scene/loading.h"
#include "bg_star.hpp"

namespace map {
	bool isPointInRect(POINT p, RECT rt)
	{
		if (p.x >= rt.left && p.x <= rt.right && p.y >= rt.top && p.y <= rt.bottom)
			return true;
		return false;
	}

	Map::Map(const std::string& stageName, const StageInfo& stageInfo)
		:
		Object("map"), //"map"の識別名の変更は厳禁！
		info_(stageInfo),
		stageName_(stageName), nowMapName_(""),
		playerEntry_(false),
		tileSizeX_(0),// tileSizeY_(0),
		changingMap_(false),
		openDoorId_(-1), closingDoorInProcess_(false)
	{
		tileSizeX_ = gplib::draw::GetImageWidth("map") / sprite::SPRITE_SIZE;
		//tileSizeY_ = gplib::draw::GetImageHeight("map") / sprite::SPRITE_SIZE;
	}

	Map::~Map()
	{
		//Reset Camera
		gplib::camera::SetLookAt((float)(gplib::system::WINW / 2), (float)(gplib::system::WINH / 2));
	}

	void Map::init()
	{
		//全体ゲームのフラグを取得 | Uniqueアイテムなどの情報を取得
		gameFlag_ = ci_ext::weak_cast<game::GameFlag>(getObjectFromRoot("game_flag"));
		{
			//Stage Flag生成
			stageFlag_ = ci_ext::weak_cast<StageFlag>(getObjectFromRoot("stage_flag"));
			//If non-exist
			if (stageFlag_.expired()) {
				//Create
				auto ptr = getParentPtr().lock()->insertAsChild(new StageFlag());
				stageFlag_ = ci_ext::weak_cast<StageFlag>(ptr);
			}
			//else {} //存在していれば　生成しない

			//Wind Manager生成
			windManager_ = ci_ext::weak_cast<object::WindManager>(getObjectFromRoot("wind_manager"));
			//If non-exist
			if (windManager_.expired()) {
				//Create
				auto ptr = insertAsChild(new object::WindManager());
				windManager_ = ci_ext::weak_cast<object::WindManager>(ptr);
			}

			//Effect Manager生成
			effectManager_ = ci_ext::weak_cast<effect::EffectManager>(getObjectFromRoot("effect_manager"));
			//If non-exist
			if (effectManager_.expired()) {
				//Create
				auto ptr = getRootObject().lock()->insertAsChild(new effect::EffectManager());
				effectManager_ = ci_ext::weak_cast<effect::EffectManager>(ptr);
			}

			//Item Manager生成
			itemManager_ = ci_ext::weak_cast<item::ItemManager>(getObjectFromRoot("item_manager"));
			//If non-exist
			if (itemManager_.expired()) {
				//Create
				auto ptr = insertAsChild(new item::ItemManager());
				itemManager_ = ci_ext::weak_cast<item::ItemManager>(ptr);
			}

			//Enemy Manager生成
			enemyManager_ = ci_ext::weak_cast<enemy::EnemyManager>(getObjectFromRoot("enemy_manager"));
			//If non-exist
			if (enemyManager_.expired()) {
				//Create
				auto ptr = insertAsChild(new enemy::EnemyManager());
				enemyManager_ = ci_ext::weak_cast<enemy::EnemyManager>(ptr);
			}

			//Event Manager生成
			eventManager_ = ci_ext::weak_cast<gameevent::StageEventManager>(getObjectFromRoot("stage_event_manager"));
			//If non-exist
			if (eventManager_.expired()) {
				//Create
				auto ptr = insertAsChild(new gameevent::StageEventManager());
				eventManager_ = ci_ext::weak_cast<gameevent::StageEventManager>(ptr);
			}

			//UI Manager生成
			uiManager_ = ci_ext::weak_cast<BOMB::uiManager>(getObjectFromRoot("ui_manager"));
			//If non-exist
			if (uiManager_.expired()) {
				//Create
				auto ptr = insertAsChild(new BOMB::uiManager());
				uiManager_ = ci_ext::weak_cast<BOMB::uiManager>(ptr);


			}

			//Timer
			timer_ = ci_ext::weak_cast<game::Timer>(getObjectFromRoot("game_timer"));
			if (timer_.expired())
			{
				auto ptr = insertAsChild(new game::Timer(180));
				timer_ = ci_ext::weak_cast<game::Timer>(ptr);
			}

			//残機
			ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info")).lock()->resetContinue();

			//BGM
			gplib::bgm::Play("stagebgm");
		}

		//全体ステージの情報を修正
		finalizeLoadStage();
		//スタートマップをセット
		//nowMapName_ = startMap_.name;
		setNowMapByFlag();
		setNowMapIterator();

		//-- This Stage Animation Tile Setup
		for (auto& animTileIn : info_.animTilesIO) {
			//Animation Step
			std::vector<int> durs;
			for (auto& a : animTileIn.second) {
				durs.push_back(a.duration);
			}
			anim::Anim a;
			a.setAnimReset(animTileIn.second.size(), durs);
			info_.animTilesClassByName.insert({ animTileIn.first, a });

			//Animation Tile First ID をセット -> render()用
			AnimFirstID firstId;
			firstId.name = animTileIn.first;
			firstId.id = animTileIn.second.at(0).id;
			info_.animTileFirstID.push_back(firstId);
		}
		//-- End Animation Tile Setup

		//BG
		//insertAsChild(new bg::BgStar());
	

		//Player生成までの処理
		playerRebirth();
	}

	void Map::update()
	{
		//Animation Tile
		for (auto& animt : info_.animTilesClassByName) {
			animt.second.step();
		}

		//Player Entry中 ("READY" Effect表示中)
		if (playerEntry_) {
			gplib::bgm::AllStop();
			gplib::bgm::Play("stagebgm");
			return;
		}


		//次のマップへ移動中なら
		if (isChangingMap()) {
			updateChangeMap();
		}
		else {
			auto player = ci_ext::weak_cast<player::Player>(getObjectFromChildren("player"));
			if (!player.expired()) {
				//player::State playerState = player.lock()->getPlayerState();
				//if (playerState != player::State::DEAD) {
					//プレイヤーの座標に対してカメラを移動
					keepCameraInScreen(player.lock()->getPos());
				//}
			}
		}


		if (nowMapName_ == "boss")
		{
			gplib::bgm::Stop("stagebgm");
			auto bossUI = ci_ext::weak_cast <BOMB::bossUiHPFrame>(getObjectFromRoot("boss_hp_frame"));
			if (bossUI.expired())
			{
				uiManager_.lock()->createUI("boss_hp_frame");
				uiManager_.lock()->createUI("boss_hp_bar");
			}
			gplib::bgm::Play("bossbgm");
			auto bound = ci_ext::weak_cast <enemy::Bound_Object>(getObjectFromRoot("bound_enemy_boss"));
			if (!bound.expired())
			{
				if (bound.lock()->isDestroy())
				{
					effectManager_.lock()->createEffect("clear", gplib::system::WINW / 2, gplib::system::WINH / 2);
					gplib::bgm::AllStop();
					
				}
			}
		}

		if (!timer_.expired() && timer_.lock()->isDestroy())
		{
			//effectManager_.lock()->createEffect("fadeinblack");
			//getParentPtr().lock()->sleep(45);

			auto rootC = getObjectsFromRoot({ "scene" });
			for (auto& c : rootC)
				c.lock()->kill();
			getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::GAME_OVER));
			//effectManager_.lock()->createEffect("clear", gplib::system::WINW / 2, gplib::system::WINH / 2);
			gplib::bgm::AllStop();

		}

		gplib::bgm::EndCheck();

	}

	void Map::render()
	{
		//処理速度を上がるため for-loop のスタートとエンドを計算する

		int mapPosX = info_.mapInfos.at(nowMapName_).pos.x;
		int mapPosY = info_.mapInfos.at(nowMapName_).pos.y;
		int mapSizeX = info_.mapInfos.at(nowMapName_).size.x;
		int mapSizeY = info_.mapInfos.at(nowMapName_).size.y;

		int startx, endx, starty, endy;

		//BG
		gplib::draw::GraphLeftTopNC(0, 0, 1.0f, "bg", 0, 0, 1280, 720, 0, nullptr, 1.0f, 1.0f);


		//Calculate loop start & end
		{
			POINT lookAt = gplib::camera::GetLookAt();
			int screenL = (lookAt.x - gplib::system::WINW / 2);
			int screenR = (lookAt.x + gplib::system::WINW / 2);
			int screenU = (lookAt.y - gplib::system::WINH / 2);
			int screenD = (lookAt.y + gplib::system::WINH / 2);

			//X
			if (mapPosX * sprite::TILE_SIZE * config.GAME_SCALE >= screenL)
				startx = 0;
			else {
				//startx = diff by tile size
				startx = (int)(abs(screenL - (mapPosX * sprite::TILE_SIZE * config.GAME_SCALE)) / (sprite::TILE_SIZE * config.GAME_SCALE));
			}

			if ((mapPosX + mapSizeX) * sprite::TILE_SIZE * config.GAME_SCALE <= screenR)
				endx = mapSizeX;
			else {
				endx = (int)(startx + ((screenR - screenL) / (sprite::TILE_SIZE *config.GAME_SCALE)) + 2);
				if (endx > mapSizeX)
					endx = mapSizeX;
			}

			//Y
			if (mapPosY * sprite::TILE_SIZE * config.GAME_SCALE >= screenU)
				starty = 0;
			else {
				//starty = diff by tile size
				starty = (int)(abs(screenU - (mapPosY * sprite::TILE_SIZE * config.GAME_SCALE)) / (sprite::TILE_SIZE * config.GAME_SCALE));
			}

			if ((mapPosY + mapSizeY) * sprite::TILE_SIZE * config.GAME_SCALE <= screenD)
				endy = mapSizeY;
			else {
				endy = (int)(starty + ((screenD - screenU) / (sprite::TILE_SIZE *config.GAME_SCALE)) + 2);
				if (endy > mapSizeY)
					endy = mapSizeY;
			}
		}
		
		//decrease Random Access when rendering big map
		//mapPosX, mapPosY, sizey, sizex を用意する
		//-2 => No need to render underfloor (dead tile)
		//size_t sizey = stageMaps_.at(nowMapName_).size() - 2;
		//size_t sizey = nowMapIt_->second.size() - 2;
		//for (size_t y = 0; y < sizey; ++y) {
		//	//size_t sizex = stageMaps_.at(nowMapName_)[y].size();
		//	size_t sizex = nowMapIt_->second[y].size();
		//	for (size_t x = 0; x < sizex; ++x) {

		for (int y = starty; y < endy; ++y) {
			for (int x = startx; x < endx; ++x) {
				//int tileId = stageMaps_.at(nowMapName_)[y][x];
				int tileId = info_.nowMapIt->second[y][x];

				//----- BG -----
				if (info_.stageBgFlags[nowMapName_]) {
					int bgTileId = tileId / BG_TILE_MULTIPLE;

					//BG Map Chip
					int chipBgx = bgTileId % tileSizeX_;
					int chipBgy = bgTileId / tileSizeX_;

					float renderZbg = 0.96f;
					if (isSecretTile(bgTileId))
						renderZbg = 0.61f;

					//Animation Tile
					for (auto& a : info_.animTileFirstID) {
						if (bgTileId == a.id) {
							int newTileId = info_.animTilesIO[a.name].at(info_.animTilesClassByName[a.name].getAnimNo()).id;
							chipBgx = newTileId % tileSizeX_;
							chipBgy = newTileId / tileSizeX_;
							break;
						}
					}

					gplib::draw::GraphLeftTop(
						((int)x + mapPosX) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
						((int)y + mapPosY) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
						renderZbg, //XYZ
						"map",
						(chipBgx * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src X
						(chipBgy * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src Y
						sprite::TILE_SIZE, sprite::TILE_SIZE, //Size
						0.0f, nullptr, //Degree, center
						config.GAME_SCALE, config.GAME_SCALE
					);
				}

				//----- End Bg -----

				int mapTileId = tileId % BG_TILE_MULTIPLE;
				int chipx = mapTileId % tileSizeX_;
				int chipy = mapTileId / tileSizeX_;

				float renderZ = 0.95f;
				if (isSecretTile(mapTileId))
					renderZ = 0.6f;

				//Animation Tile
				for (auto& a : info_.animTileFirstID) {
					if (mapTileId == a.id) {
						int newTileId = info_.animTilesIO[a.name].at(info_.animTilesClassByName[a.name].getAnimNo()).id;
						chipx = newTileId % tileSizeX_;
						chipy = newTileId / tileSizeX_;
						break;
					}
				}

				//Map Chip
				//size_tはuintので計算してもマイナスの値にならない -> デバッグ：(int)をキャストする
				gplib::draw::GraphLeftTop(
					((int)x + mapPosX) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
					((int)y + mapPosY) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
					renderZ, //XYZ
					"map",
					(chipx * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src X
					(chipy * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src Y
					sprite::TILE_SIZE, sprite::TILE_SIZE, //Size
					0.0f, nullptr, //Degree, center
					config.GAME_SCALE, config.GAME_SCALE
					);
			}
		}

		//次のマップへ移動中なら
		if (isChangingMap())
			renderNextMap();

#ifdef _DEBUG
		//Debug
		{
			for (auto& j : joints_) {
				gplib::draw::CkRect(j.rt, ARGB(255, 255, 0, 255));
			}
		}
		//----- End Debug
#endif
	}

	void Map::renderNextMap()
	{
		//If closing door processing
		if (closingDoorInProcess_) {
			//Not render duplicate of current map
			return;
			//if not return by this line --> program error, thrown by nextMapName = ""
		}


		int mapPosX = info_.mapInfos.at(nextMapName_).pos.x;
		int mapPosY = info_.mapInfos.at(nextMapName_).pos.y;
		int mapSizeX = info_.mapInfos.at(nextMapName_).size.x;
		int mapSizeY = info_.mapInfos.at(nextMapName_).size.y;

		int startx, endx, starty, endy;

		//Calculate loop start & end
		{
			POINT lookAt = gplib::camera::GetLookAt();
			int screenL = (lookAt.x - gplib::system::WINW / 2);
			int screenR = (lookAt.x + gplib::system::WINW / 2);
			int screenU = (lookAt.y - gplib::system::WINH / 2);
			int screenD = (lookAt.y + gplib::system::WINH / 2);

			//X
			if (mapPosX * sprite::TILE_SIZE * config.GAME_SCALE >= screenL)
				startx = 0;
			else {
				//startx = diff by tile size
				startx = (int)(abs(screenL - (mapPosX * sprite::TILE_SIZE * config.GAME_SCALE)) / (sprite::TILE_SIZE * config.GAME_SCALE));
			}

			if ((mapPosX + mapSizeX) * sprite::TILE_SIZE * config.GAME_SCALE <= screenR)
				endx = mapSizeX;
			else {
				endx = (int)(startx + ((screenR - screenL) / (sprite::TILE_SIZE *config.GAME_SCALE)) + 2);
				if (endx > mapSizeX)
					endx = mapSizeX;
			}

			//Y
			if (mapPosY * sprite::TILE_SIZE * config.GAME_SCALE >= screenU)
				starty = 0;
			else {
				//starty = diff by tile size
				starty = (int)(abs(screenU - (mapPosY * sprite::TILE_SIZE * config.GAME_SCALE)) / (sprite::TILE_SIZE * config.GAME_SCALE));
			}

			if ((mapPosY + mapSizeY) * sprite::TILE_SIZE * config.GAME_SCALE <= screenD)
				endy = mapSizeY;
			else {
				endy = (int)(starty + ((screenD - screenU) / (sprite::TILE_SIZE *config.GAME_SCALE)) + 2);
				if (endy > mapSizeY)
					endy = mapSizeY;
			}
		}

		//-2 => No need to render underfloor (dead tile)
		//size_t sizey = nextMapIt_->second.size() - 2;
		//for (size_t y = 0; y < sizey; ++y) {
		//	size_t sizex = nextMapIt_->second[y].size();
		//	for (size_t x = 0; x < sizex; ++x) {

		for (int y = starty; y < endy; ++y) {
			for (int x = startx; x < endx; ++x) {
				int tileId = info_.nextMapIt->second[y][x];

				//----- BG -----
				if (info_.stageBgFlags[nextMapName_]) {
					int bgTileId = tileId / BG_TILE_MULTIPLE;

					//BG Map Chip
					int chipBgx = bgTileId % tileSizeX_;
					int chipBgy = bgTileId / tileSizeX_;

					float renderZbg = 0.96f;
					if (isSecretTile(bgTileId))
						renderZbg = 0.61f;

					//Animation Tile
					for (auto& a : info_.animTileFirstID) {
						if (bgTileId == a.id) {
							int newTileId = info_.animTilesIO[a.name].at(info_.animTilesClassByName[a.name].getAnimNo()).id;
							chipBgx = newTileId % tileSizeX_;
							chipBgy = newTileId / tileSizeX_;
							break;
						}
					}

					gplib::draw::GraphLeftTop(
						((int)x + mapPosX) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
						((int)y + mapPosY) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
						renderZbg, //XYZ
						"map",
						(chipBgx * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src X
						(chipBgy * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src Y
						sprite::TILE_SIZE, sprite::TILE_SIZE, //Size
						0.0f, nullptr, //Degree, center
						config.GAME_SCALE, config.GAME_SCALE
					);
				}
				//----- End Bg -----

				int mapTileId = tileId % BG_TILE_MULTIPLE;
				int chipx = mapTileId % tileSizeX_;
				int chipy = mapTileId / tileSizeX_;

				float renderZ = 0.95f;
				if (isSecretTile(mapTileId))
					renderZ = 0.6f;

				//Animation Tile
				for (auto& a : info_.animTileFirstID) {
					if (mapTileId == a.id) {
						int newTileId = info_.animTilesIO[a.name].at(info_.animTilesClassByName[a.name].getAnimNo()).id;
						chipx = newTileId % tileSizeX_;
						chipy = newTileId / tileSizeX_;
						break;
					}
				}

				//Map Chip
				//size_tはuintので計算してもマイナスの値にならない -> デバッグ：(int)をキャストする
				gplib::draw::GraphLeftTop(
					((int)x + mapPosX) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
					((int)y + mapPosY) * ((float)sprite::TILE_SIZE * config.GAME_SCALE),
					renderZ, //XYZ
					"map",
					(chipx * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src X
					(chipy * sprite::SPRITE_SIZE) + sprite::SPRITE_OFFSET, //Src Y
					sprite::TILE_SIZE, sprite::TILE_SIZE, //Size
					0.0f, nullptr, //Degree, center
					config.GAME_SCALE, config.GAME_SCALE
				);
			}
		}
	}

	void Map::nextScene()
	{
		//Run All Root Object (if was paused by any function)
		getRootObject().lock()->runAll();

		//Because of renderLater(), it's need to kill separately.
		auto tutorial = ci_ext::weak_cast < gameevent::EventTutorial > (getObjectFromRoot("event_tutorial"));
		if (!tutorial.expired())
		{
			tutorial.lock()->~EventTutorial();
		}

		//Warp out of stage
		auto rootC = getObjectsFromRoot({ "scene" });
		for (auto& c : rootC) {
			c.lock()->kill();
		}
		getRootObject().lock()->insertAsChild(new scene::Loading(scene::Scene::LOGO));
	}

	void Map::finalizeLoadStage()
	{
		//Check Secret Tile ID & Non-Collision Tile ID
		//for (auto& s : secretTileId_) {
		//	//Secret Tile is Collision Tile
		//	if (isCollision(s)) {
		//		//Add to Non-Collision -> Remove from Collision
		//		addNonCollisionTile(s);
		//	}
		//}

		//各部屋（マップ）
		for (auto& s : info_.stageMaps) {
			//BG情報
			if (info_.stageBgFlags[s.first]) {
				//BG Tile情報を追加
				for (size_t y = 0; y < s.second.size(); ++y) {
					for (size_t x = 0; x < s.second[y].size(); ++x) {
						s.second[y][x] += info_.stageBgs.at(s.first)[y][x] * BG_TILE_MULTIPLE;
					}
				}
			}

			//床の下に　空白 & 即死 Tileを追加
			std::vector<int> addLine1;
			std::vector<int> addLine2;
			//空白・判定タイル
			for (const auto& mx : s.second.at(s.second.size() - 1)) {
				if (isCollision(mx))
					addLine1.push_back(TILE_ID_COLLISION_NO_DISP);
				else
					addLine1.push_back(TILE_ID_NON_COLLISION_NO_DISP);
			}
			s.second.push_back(addLine1);

			//空白の下に即死タイル
			for (const auto& mx : s.second.at(s.second.size() - 1)) {
				if (mx == TILE_ID_NON_COLLISION_NO_DISP)
					addLine2.push_back(TILE_ID_DEAD);
				else
					addLine2.push_back(TILE_ID_COLLISION_NO_DISP);
			}
			s.second.push_back(addLine2);
		}

		//Clear Buffer
		info_.stageBgs.clear();

		//Debug
		//std::cout << "-----------" << std::endl;
		//for (auto& my : stageMaps_.at("first")) {
		//	for (const auto& mx : my)
		//		std::cout << mx << " ";
		//	std::cout << std::endl;
		//}
	}

	void Map::setNowMapIterator()
	{
		//Set Iterator to access tile chip
		info_.nowMapIt = info_.stageMaps.find(nowMapName_);
		//Not found -> Debug
		if (info_.nowMapIt == info_.stageMaps.end())
			info_.nowMapIt = info_.stageMaps.begin();
	}

	void Map::setNonCollisionTile(const std::vector<int>& tileIds)
	{
		info_.nonCollisionId = tileIds;
	}

	void Map::addNonCollisionTile(int tileId)
	{
		info_.nonCollisionId.push_back(tileId);
	}

	void Map::removeNonCollisionTile(int tileId)
	{
		//Pred class
		class Delete {
			int i_;
		public:
			Delete(int i) {
				i_ = i;
			}
			bool operator()(Delete d) {
				return i_ == d.i_;
			}
		};
		info_.nonCollisionId.erase(std::remove_if(
			info_.nonCollisionId.begin(), info_.nonCollisionId.end(), Delete(tileId)),
			info_.nonCollisionId.end()
		);
	}

	//Called by EffectReady
	void Map::createPlayer()
	{
		{ //デバッグ処理
			auto players = getObjectsFromChildren({ "player" });
			for (auto& player : players) {
				//If player still exist
				if (!player.expired()) {
					//Kill
					player.lock()->kill();
				}
			}
		}
		//生成
		insertAsChild(new player::Player(playerEntryPos_.x, playerEntryPos_.y, playerEntryDir_, info_.isPlayerDemo));

		//Flag off
		playerEntry_ = false;
	}

	void Map::setNowMapByFlag()
	{
		//Continue Map
		if (stageFlag_.lock()->isContinueFlagOn()) {
			nowMapName_ = info_.continueMap.name;
		}
		//Start Map
		else {
			nowMapName_ = info_.startMap.name;
		}
	}

	POINT Map::getPlayerEntryPos()
	{
		//Player登場位置を計算
		int posx;
		int posy;
		//Continue Map
		if (stageFlag_.lock()->isContinueFlagOn()) {
			posx = (int)(((info_.mapInfos.at(info_.continueMap.name).pos.x + info_.continueMap.pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
			posy = (int)(((info_.mapInfos.at(info_.continueMap.name).pos.y + info_.continueMap.pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
		}
		//Start Map
		else {
			posx = (int)(((info_.mapInfos.at(info_.startMap.name).pos.x + info_.startMap.pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
			posy = (int)(((info_.mapInfos.at(info_.startMap.name).pos.y + info_.startMap.pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
		}

		//カメラ設定
		keepCameraInScreen(ci_ext::Vec2f(posx, posy));

		return { posx, posy };
	}

	void Map::setPlayerEntryDir()
	{
		//Continue Map
		if (stageFlag_.lock()->isContinueFlagOn()) {
			playerEntryDir_ = info_.continueMap.dir;
		}
		//Start Map
		else {
			playerEntryDir_ = info_.startMap.dir;
		}
	}

	void Map::playerRebirth()
	{
		//<!-- 順番は大事！ -->

		//Clear this map joints
		joints_.clear();
		//Clear Joint Doors
		auto jointDoors = getObjectsFromChildren({ "joint_door_" });
		for (auto j : jointDoors) {
			j.lock()->kill();
		}
		//リセット先のマップ名を現在のマップ名にセット
		setNowMapByFlag();
		setNowMapIterator();

		//アイテム／敵などをクリア
		destroyAllEntitiesInMap();

		//ゲージをクリア
		uiManager_.lock()->destroyUI();

		//マップのJoints／アイテム／敵などを生成
		createAllEntitiesInMap(nowMapName_);

		//Player登場位置を計算
		playerEntryPos_ = getPlayerEntryPos();
		//Playerの方向をセット
		setPlayerEntryDir();

		//UI
		//現在のライフで立てたフラグをリセット
		ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info")).lock()->resetLife();

		//フラグ & Create Effect
		playerEntry_ = true;
		effectManager_.lock()->createEffect("ready", 0, readyPosY_);

		//HP & MP ゲージ
		uiManager_.lock()->createUI("hp_frame");
		uiManager_.lock()->createUI("hp_bar");
		uiManager_.lock()->createUI("mp_frame");
		uiManager_.lock()->createUI("mp_bar");
		uiManager_.lock()->createUI("avatar");
	}

	void Map::createAllEntitiesInMap(const std::string& mapName)
	{
		//現在のマップの接続判定を生成
		createJoints(mapName);
		//カメラの限界セット
		setCameraSetting(mapName);
		
		//アイテム生成
		createItems(mapName);
		//敵生成
		createEnemies(mapName);
		//風生成
		createWinds(mapName);
		//イベント生成
		createEvents(mapName);
	}
	
	void Map::destroyAllEntitiesInMap()
	{
		//Destroy
		itemManager_.lock()->destroyItems();
		enemyManager_.lock()->destroyEnemies();
		windManager_.lock()->destroyWinds();
		eventManager_.lock()->destroyAllEventAreas();
	}

	RECT Map::getCameraLimit(const std::string & mapName)
	{
		RECT rt;

		//Calculate limit position of camera in that map
		rt.left = LONG((info_.mapInfos.at(mapName).pos.x * sprite::TILE_SIZE * (float)config.GAME_SCALE) + (gplib::system::WINW / 2));
		rt.right = LONG(((info_.mapInfos.at(mapName).pos.x + info_.mapInfos.at(mapName).size.x) * sprite::TILE_SIZE * (float)config.GAME_SCALE) - (gplib::system::WINW / 2));

		rt.top = LONG((info_.mapInfos.at(mapName).pos.y * sprite::TILE_SIZE * (float)config.GAME_SCALE) + (gplib::system::WINH / 2));
		rt.bottom = LONG(((info_.mapInfos.at(mapName).pos.y + info_.mapInfos.at(mapName).size.y) * sprite::TILE_SIZE * (float)config.GAME_SCALE) - (gplib::system::WINH / 2));

		//If map is less than game screen
		//Adjust X
		if (rt.left > rt.right) {
			//Calculate tile size in game scale
			float tileWidth = (float)gplib::system::WINW / (sprite::TILE_SIZE * (float)config.GAME_SCALE);
			//Calculate half of white space
			float halfSpace = abs(info_.mapInfos.at(mapName).size.x - tileWidth) / 2;
			//Renew position
			rt.left = LONG(((info_.mapInfos.at(mapName).pos.x - halfSpace) * sprite::TILE_SIZE * (float)config.GAME_SCALE) + (gplib::system::WINW / 2));
			rt.right = LONG(((info_.mapInfos.at(mapName).pos.x + info_.mapInfos.at(mapName).size.x + halfSpace) * sprite::TILE_SIZE * (float)config.GAME_SCALE) - (gplib::system::WINW / 2));
		}

		//Adjust Y
		if (rt.top > rt.bottom) {
			//Calculate tile size in game scale
			float tileHeight = (float)gplib::system::WINH / (sprite::TILE_SIZE * (float)config.GAME_SCALE);
			//Calculate half of white space
			float halfSpace = abs(info_.mapInfos.at(mapName).size.y - tileHeight) / 2;
			//Renew position
			rt.top = LONG(((info_.mapInfos.at(mapName).pos.y - halfSpace) * sprite::TILE_SIZE * (float)config.GAME_SCALE) + (gplib::system::WINH / 2));
			rt.bottom = LONG(((info_.mapInfos.at(mapName).pos.y + info_.mapInfos.at(mapName).size.y + halfSpace) * sprite::TILE_SIZE * (float)config.GAME_SCALE) - (gplib::system::WINH / 2));
		}

		return rt;
	}

	void Map::setCameraSetting(const std::string& mapName)
	{
		stageCamera_.limitPos = getCameraLimit(mapName);
	}

	void Map::keepCameraInScreen(const ci_ext::Vec2f& pos)
	{
		//auto mouse = GetGameMousePosition();
		//gplib::debug::TToM("mouse(%d,%d) limitLR(%d,%d) limitUD(%d,%d)", mouse.x, mouse.y, stageCamera_.limitPos.left, stageCamera_.limitPos.right, stageCamera_.limitPos.top, stageCamera_.limitPos.bottom);

		float lookx, looky;

		//X
		if (pos.x() < (float)stageCamera_.limitPos.left) {
			lookx = (float)stageCamera_.limitPos.left;
		}
		else if (pos.x() > (float)stageCamera_.limitPos.right) {
			lookx = (float)stageCamera_.limitPos.right;
		}
		else {
			lookx = pos.x();
		}

		//Y
		if (pos.y() < (float)stageCamera_.limitPos.top) {
			looky = (float)stageCamera_.limitPos.top;
		}
		else if (pos.y() > (float)stageCamera_.limitPos.bottom) {
			looky = (float)stageCamera_.limitPos.bottom;
		}
		else {
			looky = pos.y();
		}

		//gplib::debug::TToM("p(%.2f,%.2f) look(%.2f,%.2f)", pos.x(), pos.y(), lookx, looky);
		gplib::camera::SetLookAt(lookx, looky);
	}

	void Map::initChangeMap(const JointGoing& joint)
	{
		auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		auto playerState = player.lock()->getPlayerState();
		if (playerState == player::State::START_FLY ||
			playerState == player::State::FLY_ATK ||
			playerState == player::State::FLY_ATK_END) {

			//If player's state is one of above, do not change the map
			return;
		}


		//Flag
		changingMap_ = true;
		changeMapCnt_ = 0;

		//次のマップ名
		nextMapName_ = joint.nextName;

		//Set Iterator to access tile chip
		info_.nextMapIt = info_.stageMaps.find(nextMapName_);
		//Not found -> Debug
		if (info_.nextMapIt == info_.stageMaps.end())
			info_.nextMapIt = info_.stageMaps.begin();

		//プレイヤーを動けないように
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromChildren("player"));
		player.lock()->setControlable(false);

		//Kill All Item & Enemies & etc....
		//auto item = ci_ext::weak_cast<Item>(getParentPtr());
		//item.lock()->kill();

		//Move Player
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));
		//auto player = ci_ext::weak_cast<player::Player>(getObjectFromChildren("player"));
		ci_ext::Vec2f playerPos = player.lock()->getPos();

		//Changing Map Velocity -> Apply to Player / Camera
		if (joint.dir == DIR::RIGHT) {
			//Player
			{
				//Next Map Landing Position
				float newPosX = (info_.mapInfos.at(nextMapName_).pos.x * sprite::TILE_SIZE * config.GAME_SCALE) + (((joint.nextBoffset * sprite::TILE_SIZE) + player::sprite::SPRITE_HALF_SIZE) * config.GAME_SCALE);
				changeMapLandPoint_.x(newPosX);
				changeMapLandPoint_.y(playerPos.y());

				float len = changeMapLandPoint_.x() - playerPos.x();
				float move = len / (float)config.TRANSITION_FRAME;

				changeMapVelocity_.set(move, 0, 0);
			}
			//Camera
			{
				POINT now = gplib::camera::GetLookAt();
				stageCamera_.nowLookAt = ci_ext::Vec2f((float)now.x, (float)now.y);
				//float newPosX = (mapInfos_.at(nextMapName_).pos.x * sprite::TILE_SIZE * config.GAME_SCALE) + (gplib::system::WINW / 2);
				
				float nextMapPosX = (info_.mapInfos.at(nextMapName_).pos.x * sprite::TILE_SIZE * config.GAME_SCALE);
				RECT nextCameraLimit = getCameraLimit(nextMapName_);
				//Next map is less than game screen
				if (nextMapPosX < (float)nextCameraLimit.left) {
					//Adjust
					nextMapPosX = (float)nextCameraLimit.left;
				}

				float diffX = (nextMapPosX - stageCamera_.nowLookAt.x()) / (float)config.TRANSITION_FRAME;
				stageCamera_.velocity = ci_ext::Vec2f(diffX, 0);
			}
		}
		else if (joint.dir == DIR::LEFT) {
			//Player
			{
				//Next Map Landing Position
				float newPosX = ((info_.mapInfos.at(nextMapName_).pos.x + info_.mapInfos.at(nextMapName_).size.x) * sprite::TILE_SIZE * config.GAME_SCALE) - (((joint.nextBoffset * sprite::TILE_SIZE) + player::sprite::SPRITE_HALF_SIZE) * config.GAME_SCALE);
				changeMapLandPoint_.x(newPosX);
				changeMapLandPoint_.y(playerPos.y());

				float len = changeMapLandPoint_.x() - playerPos.x();
				float move = len / (float)config.TRANSITION_FRAME;

				changeMapVelocity_.set(move, 0, 0);
			}
			//Camera
			{
				POINT now = gplib::camera::GetLookAt();
				stageCamera_.nowLookAt = ci_ext::Vec2f((float)now.x, (float)now.y);
				//float newPosX = ((mapInfos_.at(nextMapName_).pos.x + mapInfos_.at(nextMapName_).size.x) * sprite::TILE_SIZE * config.GAME_SCALE) - (gplib::system::WINW / 2);
				
				float nextMapPosX = ((info_.mapInfos.at(nextMapName_).pos.x + info_.mapInfos.at(nextMapName_).size.x) * sprite::TILE_SIZE * config.GAME_SCALE);
				RECT nextCameraLimit = getCameraLimit(nextMapName_);
				//Next map is less than game screen
				if (nextMapPosX > (float)nextCameraLimit.right) {
					//Adjust
					nextMapPosX = (float)nextCameraLimit.right;
				}
				
				float diffX = (nextMapPosX - stageCamera_.nowLookAt.x()) / (float)config.TRANSITION_FRAME;
				stageCamera_.velocity = ci_ext::Vec2f(diffX, 0);
			}
		}
		else if (joint.dir == DIR::DOWN) {
			//Player
			{
				//Next Map Landing Position
				float newPosY = (info_.mapInfos.at(nextMapName_).pos.y * sprite::TILE_SIZE * config.GAME_SCALE) + (((joint.nextBoffset * sprite::TILE_SIZE) + player::sprite::SPRITE_HALF_SIZE) * config.GAME_SCALE);
				changeMapLandPoint_.x(playerPos.y());
				changeMapLandPoint_.y(newPosY);

				float len = changeMapLandPoint_.y() - playerPos.y();
				float move = len / (float)config.TRANSITION_FRAME;

				changeMapVelocity_.set(0, move, 0);
			}
			//Camera
			{
				POINT now = gplib::camera::GetLookAt();
				stageCamera_.nowLookAt = ci_ext::Vec2f((float)now.x, (float)now.y);
				//float newPosY = (mapInfos_.at(nextMapName_).pos.y * sprite::TILE_SIZE * config.GAME_SCALE) + (gplib::system::WINH / 2);
				
				float nextMapPosY = (info_.mapInfos.at(nextMapName_).pos.y * sprite::TILE_SIZE * config.GAME_SCALE);
				RECT nextCameraLimit = getCameraLimit(nextMapName_);
				//Next map is less than game screen
				if (nextMapPosY < (float)nextCameraLimit.top) {
					//Adjust
					nextMapPosY = (float)nextCameraLimit.top;
				}
				
				float diffY = (nextMapPosY - stageCamera_.nowLookAt.y()) / (float)config.TRANSITION_FRAME;
				stageCamera_.velocity = ci_ext::Vec2f(0, diffY);
			}
		}
		else if (joint.dir == DIR::UP) {
			//Player
			{
				//Next Map Landing Position
				float newPosY = ((info_.mapInfos.at(nextMapName_).pos.y + info_.mapInfos.at(nextMapName_).size.y) * sprite::TILE_SIZE * config.GAME_SCALE) - (((joint.nextBoffset * sprite::TILE_SIZE) + player::sprite::SPRITE_HALF_SIZE) * config.GAME_SCALE);
				changeMapLandPoint_.x(playerPos.x());
				changeMapLandPoint_.y(newPosY);

				float len = changeMapLandPoint_.y() - playerPos.y();
				float move = len / (float)config.TRANSITION_FRAME;

changeMapVelocity_.set(0, move, 0);
			}
			//Camera
			{
				POINT now = gplib::camera::GetLookAt();
				stageCamera_.nowLookAt = ci_ext::Vec2f((float)now.x, (float)now.y);
				//float newPosY = ((mapInfos_.at(nextMapName_).pos.y + mapInfos_.at(nextMapName_).size.y) * sprite::TILE_SIZE * config.GAME_SCALE) - (gplib::system::WINH / 2);

				float nextMapPosY = ((info_.mapInfos.at(nextMapName_).pos.y + info_.mapInfos.at(nextMapName_).size.y) * sprite::TILE_SIZE * config.GAME_SCALE);
				RECT nextCameraLimit = getCameraLimit(nextMapName_);
				//Next map is less than game screen
				if (nextMapPosY > (float)nextCameraLimit.bottom) {
					//Adjust
					nextMapPosY = (float)nextCameraLimit.bottom;
				}

				float diffY = (nextMapPosY - stageCamera_.nowLookAt.y()) / (float)config.TRANSITION_FRAME;
				stageCamera_.velocity = ci_ext::Vec2f(0, diffY);
			}
		}

		//Clear this map joints
		joints_.clear();

		//Destroy All Entities
		destroyAllEntitiesInMap();

		//Opening Door Animation
		if (joint.doorType == info::JointDoorType::DOOR) {
			auto door = ci_ext::weak_cast<JointDoor>(getObjectFromChildren("joint_door_" + std::to_string(joint.doorId)));
			door.lock()->resumeAnim();
			openDoorId_ = joint.doorId;
		}
	}

	void Map::updateChangeMap()
	{
		if (closingDoorInProcess_) {
			return;
		}

		auto player = ci_ext::weak_cast<player::Player>(getObjectFromRoot("player"));

		//Player移動
		player.lock()->offsetPos(changeMapVelocity_);
		//Stand状態だったら、移動量を与える→WALK状態になるため
		//if (player.lock()->getPlayerState() == player::State::STAND)
			//player.lock()->setVelocity(stageCamera_.velocity.x(), stageCamera_.velocity.y());

		//カメラ移動
		stageCamera_.nowLookAt += stageCamera_.velocity;
		gplib::camera::SetLookAt(stageCamera_.nowLookAt.x(), stageCamera_.nowLookAt.y());

		changeMapCnt_++;
		if (changeMapCnt_ >= config.TRANSITION_FRAME) {
			finishChangeMap();
		}
	}

	void Map::finishChangeMap()
	{
		changingMap_ = false;
		nowMapName_ = nextMapName_;
		nextMapName_ = "";
		changeMapVelocity_.set(ci_ext::Vec3f::zero());
		stageCamera_.velocity.set(ci_ext::Vec2f::zero());
		info_.nextMapIt = info_.stageMaps.end(); //Reset iterator
		setNowMapIterator(); //Refresh now map iterator

		//Destroy All Closed Door
		auto doors = getObjectsFromChildren({ "joint_door_" });
		for (auto& d : doors) {
			d.lock()->kill();
		}

		//Check Flag
		checkRebirthMapFlag();

		//Create Next Map Joints
		//createJoints(nowMapName_);
		//Set Camera Setting
		//setCameraSetting(nowMapName_);

		//マップのJoints／アイテム／敵などを生成
		createAllEntitiesInMap(nowMapName_);

		//Change Player State
		{
			auto player = ci_ext::weak_cast<player::Player>(getObjectFromChildren("player"));
			auto state = player.lock()->getPlayerState();
			if (state == player::State::WALK)
				player.lock()->setPlayerStateJoint(player::State::STAND);
			if (!closingDoorInProcess_) {
				//プレイヤーを動けるように
				player.lock()->setControlable(true);
			}
		}
	}

	void Map::checkRebirthMapFlag()
	{
		std::string& n = nowMapName_;
		auto it = std::find_if(info_.continueMaps.begin(), info_.continueMaps.end(),
			[&n](const PlayerEntry& pe)
		{
			if (n == pe.name)
				return true;
			return false;
		});

		if (it != info_.continueMaps.end())
		{
			stageFlag_.lock()->setContinue(true);
			info_.continueMap = *it;
		}
	}

	void Map::setClosingDoorInProcess(bool flag)
	{
		closingDoorInProcess_ = flag;
	}

	void Map::finishClosingDoorProcess()
	{
		closingDoorInProcess_ = false;
		openDoorId_ = -1;

		//Check Clear Flag //--------------------------------------------------------------- DEBUGGING -----------------------------
		{
	
		/*		else {
					effectManager_.lock()->createEffect("clear", gplib::system::WINW / 2, gplib::system::WINH / 2);
				}*/
			//}
		}

		//Force deflag changingMap_ << use when going up
		changingMap_ = false;
	}

	void Map::createJoints(const std::string& mapName)
	{
		for (auto& j : info_.jointInfos) {
			if (nowMapName_ == j.map1) {
				//→と↓の次のマップへ行ける
				if (j.going == info::JointRBLT::RIGHTBOTTOM) {
					//Right
					if (j.type == info::JointType::VERTICAL) {
						//判定
						RECT rt;
						rt.left = LONG(((info_.mapInfos.at(mapName).pos.x + info_.mapInfos.at(mapName).size.x) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset1 * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset1) * sprite::TILE_SIZE * config.GAME_SCALE);
						rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
						rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

						JointGoing jg;
						jg.rt = rt;
						jg.dir = DIR::RIGHT;
						jg.nextName = j.map2;
						jg.nextBoffset = j.bOffset2;
						jg.doorType = j.doorType;

						if (j.doorType == info::JointDoorType::DOOR) {
							//Set joint's door id
							jg.doorId = j.doorId;
							//Create Door Graphic
							insertAsChild(new JointDoor(j.doorId, j.type, rt));
						}

						joints_.push_back(jg);
					}
					//Down
					else {
						//判定
						RECT rt;
						rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset1) * sprite::TILE_SIZE * config.GAME_SCALE);
						rt.top = LONG(((info_.mapInfos.at(mapName).pos.y + info_.mapInfos.at(mapName).size.y) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset1 * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

						JointGoing jg;
						jg.rt = rt;
						jg.dir = DIR::DOWN;
						jg.nextName = j.map2;
						jg.nextBoffset = j.bOffset2;
						jg.doorType = j.doorType;

						if (j.doorType == info::JointDoorType::DOOR) {
							//Set joint's door id
							jg.doorId = j.doorId;
							//Create Door Graphic
							insertAsChild(new JointDoor(j.doorId, j.type, rt));
						}

						joints_.push_back(jg);
					}
				}
				//else if (j.going == info::JointRBLT::LEFTTOP)
				//←と↑の次のマップへ行ける
				else {
					//Left
					if (j.type == info::JointType::VERTICAL) {
						//判定
						RECT rt;
						rt.left = LONG(((info_.mapInfos.at(mapName).pos.x - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset1 * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset1) * sprite::TILE_SIZE * config.GAME_SCALE);
						rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
						rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

						JointGoing jg;
						jg.rt = rt;
						jg.dir = DIR::LEFT;
						jg.nextName = j.map2;
						jg.nextBoffset = j.bOffset2;
						jg.doorType = j.doorType;

						if (j.doorType == info::JointDoorType::DOOR) {
							//Set joint's door id
							jg.doorId = j.doorId;
							//Create Door Graphic
							insertAsChild(new JointDoor(j.doorId, j.type, rt));
						}

						joints_.push_back(jg);
					}
					//Up
					else {
						//判定
						RECT rt;
						rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset1) * sprite::TILE_SIZE * config.GAME_SCALE);
						rt.top = LONG(((info_.mapInfos.at(mapName).pos.y - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset1 * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
						rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

						JointGoing jg;
						jg.rt = rt;
						jg.dir = DIR::UP;
						jg.nextName = j.map2;
						jg.nextBoffset = j.bOffset2;
						jg.doorType = j.doorType;

						if (j.doorType == info::JointDoorType::DOOR) {
							//Set joint's door id
							jg.doorId = j.doorId;
							//Create Door Graphic
							insertAsChild(new JointDoor(j.doorId, j.type, rt));
						}

						joints_.push_back(jg);
					}
				}
			}
			else if (nowMapName_ == j.map2) {
				//前のマップに戻れる
				if (j.dir == info::JointDIR::BOTH) {
					//←と↑の次のマップへ行ける
					if (j.going == info::JointRBLT::RIGHTBOTTOM) {
						//Left
						if (j.type == info::JointType::VERTICAL) {
							//判定
							RECT rt;
							rt.left = LONG(((info_.mapInfos.at(mapName).pos.x - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

							JointGoing jg;
							jg.rt = rt;
							jg.dir = DIR::LEFT;
							jg.nextName = j.map1;
							jg.nextBoffset = j.bOffset1;
							jg.doorType = j.doorType;

							joints_.push_back(jg);
						}
						//Up
						else {
							//判定
							RECT rt;
							rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.top = LONG(((info_.mapInfos.at(mapName).pos.y - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

							JointGoing jg;
							jg.rt = rt;
							jg.dir = DIR::UP;
							jg.nextName = j.map1;
							jg.nextBoffset = j.bOffset1;
							jg.doorType = j.doorType;

							joints_.push_back(jg);
						}
					}
					//else if (j.going == info::JointRBLT::LEFTTOP)
					//→と↓の次のマップへ行ける
					else {
						//Right
						if (j.type == info::JointType::VERTICAL) {
							//判定
							RECT rt;
							rt.left = LONG(((info_.mapInfos.at(mapName).pos.x + info_.mapInfos.at(mapName).size.x) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

							JointGoing jg;
							jg.rt = rt;
							jg.dir = DIR::RIGHT;
							jg.nextName = j.map1;
							jg.nextBoffset = j.bOffset1;
							jg.doorType = j.doorType;

							joints_.push_back(jg);
						}
						//Down
						else {
							//判定
							RECT rt;
							rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.top = LONG(((info_.mapInfos.at(mapName).pos.y + info_.mapInfos.at(mapName).size.y) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

							JointGoing jg;
							jg.rt = rt;
							jg.dir = DIR::DOWN;
							jg.nextName = j.map1;
							jg.nextBoffset = j.bOffset1;
							jg.doorType = j.doorType;

							joints_.push_back(jg);
						}
					}
				}

				//Create Closing/Closed Door
				if (j.doorType == info::JointDoorType::DOOR) {
					// FORWARD
					if (j.going == info::JointRBLT::RIGHTBOTTOM) {
						//Left
						if (j.type == info::JointType::VERTICAL) {
							//判定
							RECT rt;
							rt.left = LONG(((info_.mapInfos.at(mapName).pos.x - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

							//Opening specific door
							if (openDoorId_ == j.doorId) {
								//Create Closing Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true));

								//Closing Door Flag
								closingDoorInProcess_ = true;
							}
							else {
								//Create Closed Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true, false));
							}
						}
						//Up
						else {
							//判定
							RECT rt;
							rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.top = LONG(((info_.mapInfos.at(mapName).pos.y - 1) * sprite::TILE_SIZE * config.GAME_SCALE) + (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

							//Opening specific door
							if (openDoorId_ == j.doorId) {
								//Create Closing Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true));

								//Closing Door Flag
								closingDoorInProcess_ = true;
							}
							else {
								//Create Closed Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true, false));
							}
						}
					}
					// End FORWARD

					// BACKWARD
					else if (j.going == info::JointRBLT::LEFTTOP) {
						//Right
						if (j.type == info::JointType::VERTICAL) {
							//判定
							RECT rt;
							rt.left = LONG(((info_.mapInfos.at(mapName).pos.x + info_.mapInfos.at(mapName).size.x) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.top = LONG((info_.mapInfos.at(mapName).pos.y + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.right = LONG(rt.left + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));

							//Opening specific door
							if (openDoorId_ == j.doorId) {
								//Create Closing Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true));

								//Closing Door Flag
								closingDoorInProcess_ = true;
							}
							else {
								//Create Closed Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true, false));
							}
						}
						//Down
						else {
							//判定
							RECT rt;
							rt.left = LONG((info_.mapInfos.at(mapName).pos.x + j.offset2) * sprite::TILE_SIZE * config.GAME_SCALE);
							rt.top = LONG(((info_.mapInfos.at(mapName).pos.y + info_.mapInfos.at(mapName).size.y) * sprite::TILE_SIZE * config.GAME_SCALE) - (int)(j.bOffset2 * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.right = LONG(rt.left + (j.size * sprite::TILE_SIZE * config.GAME_SCALE));
							rt.bottom = LONG(rt.top + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE));

							//Opening specific door
							if (openDoorId_ == j.doorId) {
								//Create Closing Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true));

								//Closing Door Flag
								closingDoorInProcess_ = true;

								//Still in process of transition when player going up
								//*Debug of drop & dead / door's 違和感
								changingMap_ = true;
							}
							else {
								//Create Closed Door Graphic
								insertAsChild(new JointDoor(j.doorId, j.type, rt, true, false));
							}
						}
					}
					// End BACKWARD
				}
			}
		}
	}

	void Map::checkJoints(POINT pt)
	{
		for (auto& j : joints_) {
			//判定POINTが接続判定を触ったら -> Change Map OK
			if (isPointInRect(pt, j.rt)) {
				initChangeMap(j);
				return;
			}
		}
	}

	bool Map::isChangingMap() const
	{
		return changingMap_;
	}

	void Map::createItems(const std::string& mapName)
	{
		auto& items = info_.stageItems.at(mapName);
		for (auto& item : items) {
			//Check unique flag
			//Not taking yet
			if (!gameFlag_.lock()->isUniqueItemFlagOn(item.uid)) {
				//Check respawn flag
				//Not respawn
				if (!item.respawn) {
					//Not take yet
					if (!stageFlag_.lock()->isItemFlagOn(item.id)) {
						int posx = (int)(((item.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
						int posy = (int)(((item.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
						//Create
						itemManager_.lock()->createItem(item.name, posx, posy, item.id, item.uid);
					}
					//auto playerInfo = ci_ext::weak_cast<player::PlayerInfo>(getObjectFromRoot("player_info"));
					//In this life, not take yet
					//if (!playerInfo.lock()->isItemFlagOn(item.id)) {
					//	//Not take yet
					//	if (!stageFlag_.lock()->isItemFlagOn(item.id)) {
					//		int posx = ((item.pos.x + mapInfos_.at(mapName).pos.x) * sprite::TILE_SIZE) + (sprite::HALF_TILE_SIZE);
					//		int posy = ((item.pos.y + mapInfos_.at(mapName).pos.y) * sprite::TILE_SIZE) + (sprite::HALF_TILE_SIZE);
					//		//Create
					//		itemManager_.lock()->createItem(item.name, posx, posy, item.id, item.uniqueId);
					//	}
					//}
				}
				//Respawn every time player enter the room
				else {
					int posx = (int)(((item.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
					int posy = (int)(((item.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
					//Create
					itemManager_.lock()->createItem(item.name, posx, posy, item.id, item.uid);
				}
			}
		}
	}

	void Map::createEnemies(const std::string & mapName)
	{
		auto& enemies = info_.stageEnemies.at(mapName);
		for (auto& enemy : enemies) {
			//Check respawn flag
			//Not respawn
			if (!enemy.respawn) {
				//Not kill yet
				if (!stageFlag_.lock()->isEnemyFlagOn(enemy.id)) {
					int posx = (int)(((enemy.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
					int posy = (int)(((enemy.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
					//Create
					enemyManager_.lock()->createEnemy(enemy.name, posx, posy, enemy.id);
				}
			}
			//Respawn every time player enter the room
			else {
				int posx = (int)(((enemy.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
				int posy = (int)(((enemy.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
				//Create
				enemyManager_.lock()->createEnemy(enemy.name, posx, posy, enemy.id);
			}
		}
	}

	void Map::createWinds(const std::string & mapName)
	{
		auto& winds = info_.stageWinds.at(mapName);
		for (auto& wind : winds) {
			int posx;
			int posy;
			if (wind.dir == info::WindDIR::UP) {
				posx = (int)(((wind.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
				posy = (int)((wind.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE);
			}
			else if (wind.dir == info::WindDIR::DOWN) {
				posx = (int)(((wind.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
				posy = (int)(((wind.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::TILE_SIZE * config.GAME_SCALE));
			}
			else if (wind.dir == info::WindDIR::LEFT) {
				posx = (int)((wind.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE);
				posy = (int)(((wind.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
			}
			else {//if (wind.dir == info::WindDIR::RIGHT) {
				posx = (int)(((wind.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::TILE_SIZE * config.GAME_SCALE));
				posy = (int)(((wind.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE) + (sprite::HALF_TILE_SIZE * config.GAME_SCALE));
			}
			//Create
			windManager_.lock()->createWind(posx, posy, wind.dir, wind.move, wind.speed, wind.range);
		}
	}

	void Map::createEvents(const std::string& mapName)
	{
		//調べるボタンを押したら、発生のEvent
		{
			auto& events = info_.stageEvents.at(mapName);
			for (auto& event : events) {
				int posx = (int)((event.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE);
				int posy = (int)((event.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE);
				int sizex = posx + (int)(event.size.x * sprite::TILE_SIZE * config.GAME_SCALE);
				int sizey = posy + (int)(event.size.y * sprite::TILE_SIZE * config.GAME_SCALE);
				//Create
				eventManager_.lock()->createEventArea(event.name, event.cmd, posx, posy, sizex, sizey);
			}
		}
		//Auto Event
		{
			auto& events = info_.stageAutoEvents.at(mapName);
			for (auto& event : events) {
				//Check flag
				//Not taking yet
				if (!gameFlag_.lock()->isEventFlagOn(event.name)) {
					int posx = (int)((event.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE);
					int posy = (int)((event.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE);
					int sizex = posx + (int)(event.size.x * sprite::TILE_SIZE * config.GAME_SCALE);
					int sizey = posy + (int)(event.size.y * sprite::TILE_SIZE * config.GAME_SCALE);
					//Create
					eventManager_.lock()->createAutoEventArea(event.name, event.cmd, posx, posy, sizex, sizey);
				}
			}
		}

		//Tutorial Event
		{
			auto& events = info_.stageTutorialEvents.at(mapName);
			for (auto& event : events) {
				//Check flag
				//Not taking yet
				if (!gameFlag_.lock()->isEventFlagOn(event.name)) {
					int posx = (int)((event.pos.x + info_.mapInfos.at(mapName).pos.x) * sprite::TILE_SIZE * config.GAME_SCALE);
					int posy = (int)((event.pos.y + info_.mapInfos.at(mapName).pos.y) * sprite::TILE_SIZE * config.GAME_SCALE);
					int sizex = posx + (int)(event.size.x * sprite::TILE_SIZE * config.GAME_SCALE);
					int sizey = posy + (int)(event.size.y * sprite::TILE_SIZE * config.GAME_SCALE);
					//Create
					eventManager_.lock()->createTutorialEventArea(event.name, event.cmd, posx, posy, sizex, sizey);
				}
			}
		}
	}

	int Map::getChip(const std::string& mapName, int x, int y) const
	{
		//マップに対して現在の座標
		int newX = x - (int)((float)info_.mapInfos.at(mapName).pos.x * (float)sprite::TILE_SIZE * config.GAME_SCALE);
		int newY = y - (int)((float)info_.mapInfos.at(mapName).pos.y * (float)sprite::TILE_SIZE * config.GAME_SCALE);

		//マップ上の座標 -> Index
		int mapx = newX / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);
		int mapy = newY / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);

		//判定外 || 判定外 || Out of range || Out of range
		//mapx < 0 || mapy < 0 は「だめ」 -> (int)x / (int)SIZE -> たとえ x = -1 でも　計算結果は 0
		//size.y + 2は即死タイルを確認ため
		if (newX < 0 || newY < 0 || mapx >= info_.mapInfos.at(mapName).size.x || mapy >= info_.mapInfos.at(mapName).size.y + 2)
			return TILE_ID_COLLISION_NO_DISP; //Out of map

		return (info_.stageMaps.at(mapName)[mapy][mapx] % BG_TILE_MULTIPLE);
	}

	RECT Map::getTileRect(int x, int y) const
	{
		//マップ上の座標 -> Index
		int mapx = x / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);
		int mapy = y / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);

		//(-)座標のRECTバグを修正 <-- -0.99~0.99の間の計算は同じ結果
		if (x < 0) mapx--;
		if (y < 0) mapy--;
		//End 修正

		RECT rt = {
			(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)),
			(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)),
			(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE),
			(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE),
		};

		return rt;
	}

	bool Map::isCollision(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (std::find(info_.nonCollisionId.begin(), info_.nonCollisionId.end(), tileId) == info_.nonCollisionId.end())
			return true;
		return false;
	}

	bool Map::isCustomTile(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (info_.customTile.find(tileId) != info_.customTile.end())
			return true;
		return false;
	}

	RECT Map::getCustomTileSetting(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (info_.customTile.find(tileId) != info_.customTile.end()) {
			return info_.customTile.at(tileId);
		}
		return RECT{ 0, 0, (int)((float)sprite::TILE_SIZE * config.GAME_SCALE), (int)((float)sprite::TILE_SIZE * config.GAME_SCALE) };
	}

	RECT Map::getCustomTileRect(const std::string& mapName, int x, int y, int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;

		//現在の座標 -> Index
		int mapx = x / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);
		int mapy = y / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);

		RECT custom = getCustomTileSetting(tileId);

		//(-)座標のRECTバグを修正 <-- -0.99~0.99の間の計算は同じ結果
		if (x < 0) mapx--;
		if (y < 0) mapy--;
		//End 修正

		RECT rt = {
			(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)custom.left * config.GAME_SCALE),
			(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)custom.top * config.GAME_SCALE),
			(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)custom.right * config.GAME_SCALE),
			(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)custom.bottom * config.GAME_SCALE)
		};

		return rt;
	}

	bool Map::isSecretTile(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (std::find(info_.secretTileId.begin(), info_.secretTileId.end(), tileId) != info_.secretTileId.end())
			return true;
		return false;
	}

	bool Map::isDamageTile(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (std::find(info_.damageTileId.begin(), info_.damageTileId.end(), tileId) != info_.damageTileId.end())
			return true;
		return false;
	}

	bool Map::isDeadTile(int tileId) const
	{
		tileId %= BG_TILE_MULTIPLE;
		if (tileId == TILE_ID_DEAD)
			return true;
		return false;
	}

	//bool Map::isWindTile(int tileId) const
	//{
	//	tileId %= BG_TILE_MULTIPLE;
	//	if (std::find_if(windTiles_.begin(), windTiles_.end(),
	//			[&tileId](const WindInfo& w)
	//			{
	//				if (w.id == tileId)
	//					return true;
	//				return false;
	//			}
	//		) != windTiles_.end())
	//		return true;
	//	return false;
	//}

	//RECT Map::getWindTileRect(const std::string & mapName, int x, int y)
	//{
	//	//現在の座標 -> Index
	//	int mapx = x / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);
	//	int mapy = y / (int)((float)sprite::TILE_SIZE * config.GAME_SCALE);

	//	//(-)座標のRECTバグを修正 <-- -0.99~0.99の間の計算は同じ結果
	//	if (x < 0) mapx--;
	//	if (y < 0) mapy--;
	//	//End 修正

	//	int offset = 2;

	//	RECT rt = {
	//		(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + offset,
	//		(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + offset,
	//		(mapx * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE) - offset,
	//		(mapy * (int)((float)sprite::TILE_SIZE * config.GAME_SCALE)) + (int)((float)sprite::TILE_SIZE * config.GAME_SCALE) - offset,
	//	};

	//	return rt;
	//}

	//info::WindDIR Map::getWindDir(int tileId) const
	//{
	//	for (auto& w : windTiles_) {
	//		if (w.id == tileId) {
	//			return w.dir;
	//		}
	//	}
	//	return info::WindDIR();
	//}

	//ci_ext::Vec3f Map::getWindForce(info::WindDIR dir) const
	//{
	//	if (dir == info::WindDIR::LEFT) {
	//		return ci_ext::Vec3f(-config.WIND_FORCE_X, 0, 0);
	//		}
	//	else if (dir == info::WindDIR::RIGHT) {
	//		return ci_ext::Vec3f(config.WIND_FORCE_X, 0, 0);
	//	}
	//	else if (dir == info::WindDIR::UP) {
	//		return ci_ext::Vec3f(0, -config.WIND_FORCE_Y, 0);
	//	}
	//	else if (dir == info::WindDIR::DOWN) {
	//		return ci_ext::Vec3f(0, config.WIND_FORCE_Y, 0);
	//	}
	//	return ci_ext::Vec3f::zero();
	//}

	std::string Map::getNowMapName() const
	{
		return nowMapName_;
	}
	
}