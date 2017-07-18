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

#pragma once

//XML
#include "xml.h"

#include <sstream>
#include <fstream>

#include "../stage/stageInfo.h"
#include "../util/util_wstringConvert.h"

#include "../debug/debug.h"

//Debug std::cout
//#include <iostream>

namespace io {
	class StageReader final {
	private:
		std::string stageName_;
		map::StageInfo& stageInfo_;


		//##################################################
		//-------------------- Buffer Info --------------------
		//##################################################
		//部屋内の情報を取得しているか
		bool insideMap_; //フラグ
		std::string nowReadMap_; //現在読んでいるマップ名
		//各部屋（マップ）の情報→バッファ | loadStageInfo()
		std::vector<map::MapInfo> readingMapInfos_;
		//各部屋（マップ）のアイテム→バッファ | loadStageInfo()
		std::vector<map::ItemInfo> readingMapItems_;
		//各部屋（マップ）の敵→バッファ | loadStageInfo()
		std::vector<map::EnemyInfo> readingMapEnemies_;
		//各部屋（マップ）の風→バッファ | loadStageInfo()
		std::vector<map::WindInfo> readingMapWinds_;
		//各部屋（マップ）のイベント→バッファ | loadStageInfo()
		std::vector<map::EventInfo> readingMapEvents_;
		std::vector<map::EventInfo> readingMapAutoEvents_;
		std::vector<map::EventInfo> readingMapTutorialEvents_;
		//このステージはカスタムタイルがあるか？
		bool hasCustomTile_;
		//-------------------- End Buffer Info --------------------


		//##################################################
		//-------------------- Buffer Map --------------------
		//##################################################

		//各部屋→バッファ | loadEachMap()
		std::vector<std::vector<int>> readingMap_;
		//-------------------- End Buffer Map --------------------

		void clear()
		{
			//Clear Buffer
			readingMapInfos_.clear();
			readingMapItems_.clear();
			readingMapEnemies_.clear();
			readingMapWinds_.clear();
			readingMapEvents_.clear();
			readingMapAutoEvents_.clear();
			readingMapTutorialEvents_.clear();
			readingMap_.clear();
		}

	public:
		StageReader(const std::string& stageName, map::StageInfo& stageInfo)
			:
			stageName_(stageName), stageInfo_(stageInfo),
			insideMap_(false), nowReadMap_("")
		{
			clear();
			
			//Clear All Infos
			stageInfo_.clear();

			loadStageInfo();
			loadEachMap();
			if (hasCustomTile_)
				loadCustomTile();
		}

		~StageReader() { clear(); }

	private:
		//Reading XML Node Type = End Element
		//Putting Every entities of that map into table
		void closeMap(const std::string& mapName)
		{
			//Insert to table
			stageInfo_.stageItems.insert({ mapName, readingMapItems_ });
			stageInfo_.stageEnemies.insert({ mapName, readingMapEnemies_ });
			stageInfo_.stageWinds.insert({ mapName, readingMapWinds_ });
			stageInfo_.stageEvents.insert({ mapName, readingMapEvents_ });
			stageInfo_.stageAutoEvents.insert({ mapName, readingMapAutoEvents_ });
			stageInfo_.stageTutorialEvents.insert({ mapName, readingMapTutorialEvents_ });

			//Clear Buffer
			readingMapItems_.clear();
			readingMapEnemies_.clear();
			readingMapWinds_.clear();
			readingMapEvents_.clear();
			readingMapAutoEvents_.clear();
			readingMapTutorialEvents_.clear();

			//Read Map = NULL
			nowReadMap_ = "";

			//DO NOT reset flag insideMap in this function
			//insideMap_ = false; <-- ダメ
			//because it will skip reading flag when no entity in that map
		}

		//Check Last Map Entities
		void checkLastMapEntities()
		{
			//Check only 1 entity is OK
			if (stageInfo_.stageItems.find(nowReadMap_) == stageInfo_.stageItems.end()) {
				closeMap(nowReadMap_);
			}
		}

		//--------------------------------------------------
		//Load Stage Infomation
		//--------------------------------------------------
		void loadStageInfo()
		{
			CComPtr<IXmlReader> pReader;
			CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&pReader), 0);

			{
				// XMLファイルパス
				std::stringstream fileName;
				fileName << "res/stage/" << stageName_ << "/stage.xml";

				// ファイルストリーム作成
				CComPtr<IStream> pStream;
				if (FAILED(SHCreateStreamOnFile(fileName.str().c_str(), STGM_READ, &pStream))) {
					MessageBox(NULL, _T("CreateXmlReader失敗"), _T("警告"), MB_OK | MB_ICONWARNING);
					assert(0);
				}

				pReader->SetInput(pStream);
			}

			//Long Pointer Wide String (Unicode)
			//コンパイルエラー回避ために、nullptrを初期化する
			LPCWSTR pwszLocalName = nullptr;
			LPCWSTR pwszValue = nullptr;

			//----- Node Type -----
			//XmlNodeType_None					|	なし
			//XmlNodeType_Element				|	要素の開始
			//XmlNodeType_Attribute				|	属性
			//XmlNodeType_Text					|	テキスト
			//XmlNodeType_CDATA					|	CDATAセクション
			//XmlNodeType_ProcessingInstruction	|	XMLプロセッサ処理命令
			//XmlNodeType_Comment				|	コメント
			//XmlNodeType_DocumentType			|	ドキュメントの型宣言（DTD）
			//XmlNodeType_Whitespace			|	空白
			//XmlNodeType_EndElement			|	要素の終了
			//XmlNodeType_XmlDeclaration		|	XML宣言
			XmlNodeType nodeType;

			HRESULT hr;
			while (pReader->Read(&nodeType) == S_OK) {
				switch (nodeType) {
				case XmlNodeType_Element:
					pReader->GetLocalName(&pwszLocalName, NULL);

					//最初の属性に移動
					hr = pReader->MoveToFirstAttribute();

					//属性が無い
					if (hr == S_FALSE) {
						break; //break switch -> next node
					}

					//Stage
					if (lstrcmpW(pwszLocalName, L"Stage") == 0) {
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"custom") == 0) hasCustomTile_ = (lstrcmpW(pwszAttributeValue, L"true") == 0) ? true : false;
							if (lstrcmpW(pwszAttributeName, L"demo") == 0) stageInfo_.isPlayerDemo = (lstrcmpW(pwszAttributeValue, L"true") == 0) ? true : false;

						} while (pReader->MoveToNextAttribute() == S_OK); //次の属性があれば -> ループ
					}

					//NonCollision Tile
					if (lstrcmpW(pwszLocalName, L"NonCollision") == 0) {
						std::string list;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"list") == 0) list = ws2s(pwszAttributeValue);

						} while (pReader->MoveToNextAttribute() == S_OK);

						std::replace(list.begin(), list.end(), ',', ' ');
						std::stringstream ss(list);
						while (!ss.eof()) {
							int i;
							ss >> i;
							stageInfo_.nonCollisionId.push_back(i);
						}
					}

					//Secret Tile
					if (lstrcmpW(pwszLocalName, L"SecretTile") == 0) {
						std::string list;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"list") == 0) list = ws2s(pwszAttributeValue);

						} while (pReader->MoveToNextAttribute() == S_OK);

						std::replace(list.begin(), list.end(), ',', ' ');
						std::stringstream ss(list);
						while (!ss.eof()) {
							int i;
							ss >> i;
							stageInfo_.secretTileId.push_back(i);
						}
					}

					//Damage Tile
					if (lstrcmpW(pwszLocalName, L"DamageTile") == 0) {
						std::string list;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"list") == 0) list = ws2s(pwszAttributeValue);

						} while (pReader->MoveToNextAttribute() == S_OK);

						std::replace(list.begin(), list.end(), ',', ' ');
						std::stringstream ss(list);
						while (!ss.eof()) {
							int i;
							ss >> i;
							stageInfo_.damageTileId.push_back(i);
						}
					}

					//Wind Tile
					//if (lstrcmpW(pwszLocalName, L"WindTile") == 0) {
					//	WindInfo tmp;
					//	do {
					//		LPCWSTR pwszAttributeName = nullptr;
					//		LPCWSTR pwszAttributeValue = nullptr;

					//		pReader->GetLocalName(&pwszAttributeName, NULL);
					//		pReader->GetValue(&pwszAttributeValue, NULL);
					//		
					//		if (lstrcmpW(pwszAttributeName, L"id") == 0) tmp.id = stoi(ws2s(pwszAttributeValue));
					//		else if (lstrcmpW(pwszAttributeName, L"force") == 0) tmp.force = stof(ws2s(pwszAttributeValue));
					//		else if (lstrcmpW(pwszAttributeName, L"dir") == 0) {
					//			if (lstrcmpW(pwszAttributeValue, L"left") == 0)
					//				tmp.dir = map::info::WindDIR::LEFT;
					//			else if (lstrcmpW(pwszAttributeValue, L"right") == 0)
					//				tmp.dir = map::info::WindDIR::RIGHT;
					//			else if (lstrcmpW(pwszAttributeValue, L"up") == 0)
					//				tmp.dir = map::info::WindDIR::UP;
					//			else if (lstrcmpW(pwszAttributeValue, L"down") == 0)
					//				tmp.dir = map::info::WindDIR::DOWN;
					//		}

					//	} while (pReader->MoveToNextAttribute() == S_OK);

					//	windTiles_.push_back(tmp);
					//}

					//Animation Tile
					if (lstrcmpW(pwszLocalName, L"AnimTile") == 0) {
						std::string name;
						std::string strList;
						std::string strDur;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"name") == 0) name = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"list") == 0) strList = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"duration") == 0) strDur = ws2s(pwszAttributeValue);

						} while (pReader->MoveToNextAttribute() == S_OK);

						std::vector<int> ids;
						{
							std::replace(strList.begin(), strList.end(), ',', ' ');
							std::stringstream ss(strList);
							while (!ss.eof()) {
								int i;
								ss >> i;
								ids.push_back(i);
							}
						}

						std::vector<int> durs;
						{
							std::replace(strDur.begin(), strDur.end(), ',', ' ');
							std::stringstream ss(strDur);
							while (!ss.eof()) {
								int i;
								ss >> i;
								durs.push_back(i);
							}
						}

						//Auto Correction
						if (durs.size() <= 0)
							durs.push_back(1);

						std::vector<map::AnimTileInfo> info;
						//Size not equal -> All durations are equal to first one
						if (durs.size() == 1 || durs.size() != ids.size()) {
							for (auto& id : ids) {
								map::AnimTileInfo tmp;
								tmp.id = id;
								tmp.duration = durs[0];
								info.push_back(tmp);
							}
						}
						else {
							for (size_t i = 0; i < ids.size(); ++i) {
								map::AnimTileInfo tmp;
								tmp.id = ids[i];
								tmp.duration = durs[i];
								info.push_back(tmp);
							}
						}

						stageInfo_.animTilesIO.insert({ name, info });
					}

					//Music
					if (lstrcmpW(pwszLocalName, L"Music") == 0) {
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"track") == 0) {
								//--------------------------------------------------------------------
								//std::cout << ws2s(pwszLocalName) << ":" << ws2s(pwszAttributeName) << ":" << ws2s(pwszAttributeValue) << std::endl;
							}
						} while (pReader->MoveToNextAttribute() == S_OK);
					}

					//Start Map
					if (lstrcmpW(pwszLocalName, L"Start") == 0) {
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"map") == 0) stageInfo_.startMap.name = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"posx") == 0) stageInfo_.startMap.pos.x = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"posy") == 0) stageInfo_.startMap.pos.y = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"dir") == 0) stageInfo_.startMap.dir = (lstrcmpW(pwszAttributeValue, L"left") == 0) ? player::DIR::LEFT : player::DIR::RIGHT;

						} while (pReader->MoveToNextAttribute() == S_OK);
					}

					//Continue Map
					if (lstrcmpW(pwszLocalName, L"Continue") == 0) {
						map::PlayerEntry tmp;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"map") == 0) tmp.name = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"dir") == 0) tmp.dir = (lstrcmpW(pwszAttributeValue, L"left") == 0) ? player::DIR::LEFT : player::DIR::RIGHT;

						} while (pReader->MoveToNextAttribute() == S_OK);
						stageInfo_.continueMaps.push_back(tmp);
					}

					//Map Info
					if (lstrcmpW(pwszLocalName, L"Map") == 0) {
						insideMap_ = true;
						if (nowReadMap_ != "")
							closeMap(nowReadMap_); //Close Map

						map::MapInfo tmp;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"sizex") == 0) tmp.size.x = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"sizey") == 0) tmp.size.y = stoi(ws2s(pwszAttributeValue));

						} while (pReader->MoveToNextAttribute() == S_OK);
						readingMapInfos_.push_back(tmp);
						nowReadMap_ = tmp.name;
					}

					//Map内
					{
						//Item Info
						if (lstrcmpW(pwszLocalName, L"Item") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::ItemInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);

								if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"respawn") == 0) tmp.respawn = (lstrcmpW(pwszAttributeValue, L"true") == 0) ? true : false;
								else if (lstrcmpW(pwszAttributeName, L"uid") == 0) tmp.uid = stoi(ws2s(pwszAttributeValue));

							} while (pReader->MoveToNextAttribute() == S_OK);
							readingMapItems_.push_back(tmp);
						}

						//Enemy Info
						if (lstrcmpW(pwszLocalName, L"Enemy") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::EnemyInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);

								if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"respawn") == 0) tmp.respawn = (lstrcmpW(pwszAttributeValue, L"false") == 0) ? false : true;

							} while (pReader->MoveToNextAttribute() == S_OK);
							readingMapEnemies_.push_back(tmp);
						}

						//Wind Info
						if (lstrcmpW(pwszLocalName, L"Wind") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::WindInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);
								
								if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"dir") == 0) {
									if (lstrcmpW(pwszAttributeValue, L"left") == 0)
										tmp.dir = map::info::WindDIR::LEFT;
									else if (lstrcmpW(pwszAttributeValue, L"right") == 0)
										tmp.dir = map::info::WindDIR::RIGHT;
									else if (lstrcmpW(pwszAttributeValue, L"up") == 0)
										tmp.dir = map::info::WindDIR::UP;
									else if (lstrcmpW(pwszAttributeValue, L"down") == 0)
										tmp.dir = map::info::WindDIR::DOWN;
								}
								else if (lstrcmpW(pwszAttributeName, L"move") == 0) tmp.move = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"speed") == 0) tmp.speed = stof(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"range") == 0) tmp.range = stoi(ws2s(pwszAttributeValue));

							} while (pReader->MoveToNextAttribute() == S_OK);

							readingMapWinds_.push_back(tmp);
						}

						//Event Info
						if (lstrcmpW(pwszLocalName, L"Event") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::EventInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);

								if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"cmd") == 0) tmp.cmd = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizex") == 0) tmp.size.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizey") == 0) tmp.size.y = stoi(ws2s(pwszAttributeValue));

							} while (pReader->MoveToNextAttribute() == S_OK);
							readingMapEvents_.push_back(tmp);
						}

						//Auto Event Info
						if (lstrcmpW(pwszLocalName, L"AutoEvent") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::EventInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);

								if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"cmd") == 0) tmp.cmd = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizex") == 0) tmp.size.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizey") == 0) tmp.size.y = stoi(ws2s(pwszAttributeValue));
								//else if (lstrcmpW(pwszAttributeName, L"redead") == 0) tmp.reDead = (lstrcmpW(pwszAttributeValue, L"true") == 0) ? true : false;

							} while (pReader->MoveToNextAttribute() == S_OK);
							readingMapAutoEvents_.push_back(tmp);
						}

						//Tutorial Event Info
						if (lstrcmpW(pwszLocalName, L"TutorialEvent") == 0) {
							if (!insideMap_) break;
							if (nowReadMap_ == "") break;

							map::EventInfo tmp;
							do {
								LPCWSTR pwszAttributeName = nullptr;
								LPCWSTR pwszAttributeValue = nullptr;

								pReader->GetLocalName(&pwszAttributeName, NULL);
								pReader->GetValue(&pwszAttributeValue, NULL);

								if (lstrcmpW(pwszAttributeName, L"name") == 0) tmp.name = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"cmd") == 0) tmp.cmd = ws2s(pwszAttributeValue);
								else if (lstrcmpW(pwszAttributeName, L"posx") == 0) tmp.pos.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"posy") == 0) tmp.pos.y = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizex") == 0) tmp.size.x = stoi(ws2s(pwszAttributeValue));
								else if (lstrcmpW(pwszAttributeName, L"sizey") == 0) tmp.size.y = stoi(ws2s(pwszAttributeValue));

							} while (pReader->MoveToNextAttribute() == S_OK);
							readingMapTutorialEvents_.push_back(tmp);
						}

					}//End Map内

					 //Joint Info
					if (lstrcmpW(pwszLocalName, L"Joint") == 0) {
						map::JointInfo tmp;
						do {
							LPCWSTR pwszAttributeName = nullptr;
							LPCWSTR pwszAttributeValue = nullptr;

							pReader->GetLocalName(&pwszAttributeName, NULL);
							pReader->GetValue(&pwszAttributeValue, NULL);

							if (lstrcmpW(pwszAttributeName, L"type") == 0) tmp.type = (lstrcmpW(pwszAttributeValue, L"vertical") == 0) ? map::info::JointType::VERTICAL : map::info::JointType::HORIZONTAL;
							else if (lstrcmpW(pwszAttributeName, L"map1") == 0) tmp.map1 = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"map2") == 0) tmp.map2 = ws2s(pwszAttributeValue);
							else if (lstrcmpW(pwszAttributeName, L"offset1") == 0) tmp.offset1 = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"offset2") == 0) tmp.offset2 = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"boffset1") == 0) tmp.bOffset1 = stof(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"boffset2") == 0) tmp.bOffset2 = stof(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"size") == 0) tmp.size = stoi(ws2s(pwszAttributeValue));
							else if (lstrcmpW(pwszAttributeName, L"dir") == 0) tmp.dir = (lstrcmpW(pwszAttributeValue, L"forward") == 0) ? map::info::JointDIR::FORWARD : map::info::JointDIR::BOTH;
							else if (lstrcmpW(pwszAttributeName, L"going") == 0) tmp.going = (lstrcmpW(pwszAttributeValue, L"lefttop") == 0) ? map::info::JointRBLT::LEFTTOP : map::info::JointRBLT::RIGHTBOTTOM;
							else if (lstrcmpW(pwszAttributeName, L"door") == 0) tmp.doorType = (lstrcmpW(pwszAttributeValue, L"true") == 0) ? map::info::JointDoorType::DOOR : map::info::JointDoorType::NONE;

						} while (pReader->MoveToNextAttribute() == S_OK);
						stageInfo_.jointInfos.push_back(tmp);
					}


					/*do {
						LPCWSTR pwszAttributeName = nullptr;
						LPCWSTR pwszAttributeValue = nullptr;

						pReader->GetLocalName(&pwszAttributeName, NULL);
						pReader->GetValue(&pwszAttributeValue, NULL);

						if (lstrcmpW(pwszAttributeName, L"name") == 0) {
							std::wstring ws(pwszAttributeValue);
							std::wcout << ws.c_str() << std::endl;
							std::cout << ws2s(pwszLocalName) << ":" << ws2s(pwszAttributeName) << ":" << ws2s(pwszAttributeValue) << std::endl;
						}
					} while (pReader->MoveToNextAttribute() == S_OK);*/

					break;
					//case XmlNodeType_Text:
					//	pReader->GetValue(&pwszValue, NULL);
					//
					//	if (lstrcmpW(pwszLocalName, L"element") == 0) {
					//		std::wstring ws(pwszValue);
					//		std::wcout << ws.c_str() << std::endl;
					//	}
					//	break;
				case XmlNodeType_EndElement:
					pReader->GetLocalName(&pwszLocalName, NULL);

					if (lstrcmpW(pwszLocalName, L"Map") == 0) {
						closeMap(nowReadMap_);
						insideMap_ = false;
					}
					else if (lstrcmpW(pwszLocalName, L"Stage") == 0) {
						for (auto& m : readingMapInfos_) {
							stageInfo_.mapInfos.insert({ m.name, m });
						}

						//読んでいた最後のマップ名はまだcloseMap()呼ばれていないかもしれない
						//Check Last Map Entities
						checkLastMapEntities();
						//----- 確認完了

						//Clear Buffer
						readingMapInfos_.clear();
					}

					break;
				}
			}

			//--------------------------------------------------
			// Finally
			//--------------------------------------------------

			//Delete blank map (fake data)
			for (auto it = stageInfo_.mapInfos.begin(); it != stageInfo_.mapInfos.end();) {
				if ((*it).second.size.x == 0 || (*it).second.size.y == 0) {
					it = stageInfo_.mapInfos.erase(it);
				}
				else {
					it++;
				}
			}

			//Delete unaccessable map joint
			for (auto it = stageInfo_.jointInfos.begin(); it != stageInfo_.jointInfos.end();) {
				//Find name
				auto itMap1 = stageInfo_.mapInfos.find((*it).map1);
				//Not Found
				if (itMap1 == stageInfo_.mapInfos.end()) {
					//Delete
					it = stageInfo_.jointInfos.erase(it);
				}
				else {
					auto itMap2 = stageInfo_.mapInfos.find((*it).map2);
					//Not Found
					if (itMap2 == stageInfo_.mapInfos.end()) {
						//Delete
						it = stageInfo_.jointInfos.erase(it);
					}
					else {
						it++;
					}
				}
			}

			int doorId = 0;
			//Give id to doors & correct the direction
			for (auto it = stageInfo_.jointInfos.begin(); it != stageInfo_.jointInfos.end(); ++it) {
				if ((*it).doorType == map::info::JointDoorType::DOOR) {
					(*it).dir = map::info::JointDIR::FORWARD; //進むしかない
					(*it).doorId = doorId;
					doorId++;
				}
			}

			//Give id to items
			int itemId = 0;
			for (auto& vec : stageInfo_.stageItems) {
				for (auto& item : vec.second) {
					item.id = itemId;
					itemId++;
				}
			}

			//Give id to enemies
			int enemyId = 0;
			for (auto& vec : stageInfo_.stageEnemies) {
				for (auto& e : vec.second) {
					e.id = enemyId;
					enemyId++;
				}
			}

			/*
			//Debug
			std::cout << "--- remaining map after correction ---" << std::endl;
			for (auto& m : mapInfos_) {
				std::cout << m.first << std::endl;
			}
			std::cout << "--- remaining joint after correction ---" << std::endl;
			for (auto& j : jointInfos_) {
				std::cout << j.map1 << "::" << j.map2 << std::endl;
			}
			
			std::cout << "========================" << std::endl;
			std::cout << "Start:" << startMap_ << std::endl;
			std::cout << "Continue:" << continueMap_ << std::endl;
			for (const auto& m : mapInfos_) {
				std::cout << "----------- MAP --------------" << std::endl;
				std::cout << m.first << std::endl;
				std::cout << m.second.pos.x << "," << m.second.pos.y << std::endl;
				std::cout << m.second.size.x << "," << m.second.size.y << std::endl;
			}
			std::cout << "========================" << std::endl;
			for (const auto& mItem : stageItems_) {
				for (const auto& i : mItem.second) {
					std::cout << "--------- ITEM -------------" << std::endl;
					std::cout << mItem.first << ":" << i.name << std::endl;
					std::cout << mItem.first << ":" << i.pos.x << "," << i.pos.y << std::endl;
					std::cout << mItem.first << ":" << i.respawn << std::endl;
				}
			}
			for (const auto& j : jointInfos_) {
				std::cout << "----------- JOINT --------------" << std::endl;
				std::cout << j.type << std::endl;
				std::cout << j.map1 << "<->" << j.map2 << std::endl;
				std::cout << j.offset1 << "," << j.offset2 << std::endl;
				std::cout << j.bOffset1 << "," << j.bOffset2 << std::endl;
				std::cout << j.size << std::endl;
				std::cout << j.dir << std::endl;
			}*/
		}

		//--------------------------------------------------
		// 各部屋の形をロード
		//--------------------------------------------------
		void loadEachMap()
		{
			for (const auto& m : stageInfo_.mapInfos)
			{
				std::ifstream fin("res/stage/" + stageName_ + "/" + m.first + ".csv");

				if (fin.is_open()) {
					while (!fin.eof()) {
						std::string line;
						std::vector<int> tmp;
						fin >> line;

						if (line.empty()) break;

						std::replace(line.begin(), line.end(), ',', ' ');
						std::stringstream ss(line);
						while (!ss.eof()) {
							int i;
							ss >> i;
							if (i == -1) i = 0; //Convert blank to 0
							tmp.push_back(i);
						}
						readingMap_.push_back(tmp);
					}
					stageInfo_.stageMaps.insert({ m.first, readingMap_ });

					//Clear Buffer
					readingMap_.clear();

					//Read BG
					loadEachMapBgTile(m.first);
				}
				else {
					//Error処理
					//blah blah blah...

					std::cout << "Assert:: No room: " << m.first << std::endl;
					assert(0);
				}
				fin.close();
			}
		}

		//--------------------------------------------------
		// 各部屋の背景の形をロード
		//--------------------------------------------------
		void loadEachMapBgTile(const std::string& mapName)
		{
			std::ifstream fin("res/stage/" + stageName_ + "/" + mapName + "_bg.csv");

			//Have BG File
			if (fin.is_open()) {
				//Set Flag On
				stageInfo_.stageBgFlags.insert({ mapName, true });

				//Buffer
				std::vector<std::vector<int>> readingBg_;

				while (!fin.eof()) {
					std::string line;
					std::vector<int> tmp;
					fin >> line;

					if (line.empty()) break;

					std::replace(line.begin(), line.end(), ',', ' ');
					std::stringstream ss(line);
					while (!ss.eof()) {
						int i;
						ss >> i;
						if (i == -1) i = 0; //Convert blank to 0
						tmp.push_back(i);
					}
					readingBg_.push_back(tmp);
				}
				stageInfo_.stageBgs.insert({ mapName, readingBg_ });

				//Clear Buffer
				readingBg_.clear();
			}
			//No BG File
			else {
				//Set Flag Off
				stageInfo_.stageBgFlags.insert({ mapName, false });

				//Insert Blank Map
				stageInfo_.stageBgs.insert({ mapName, {} });

				//std::cout << "No BG room: " << mapName << std::endl;
			}
			fin.close();
		}

		//--------------------------------------------------
		// カスタムタイルがあれば、各タイルのRECTをロード
		//--------------------------------------------------
		void loadCustomTile()
		{
			std::ifstream fin("res/stage/" + stageName_ + "/customtile.csv");

			if (fin.is_open()) {
				while (!fin.eof()) {
					std::string line;
					std::vector<int> tmp;
					//fin >> line; //<-- Stop @ White space <-- NG
					std::getline(fin, line); //<-- Use this one

											 //Skip comment line
					if (line.c_str()[0] == '/') continue;

					if (line.empty()) break;


					std::replace(line.begin(), line.end(), ',', ' ');
					std::stringstream ss(line);
					while (!ss.eof()) {
						int i;
						ss >> i;
						tmp.push_back(i);
					}
					stageInfo_.customTile.insert({ tmp.at(0),{ tmp.at(1), tmp.at(2),tmp.at(3),tmp.at(4) } });
				}
			}
			fin.close();
		}
	};
}