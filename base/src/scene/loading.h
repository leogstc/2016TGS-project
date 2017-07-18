/*
---------- Reference ----------

Multiple Arguments Template
http://stackoverflow.com/questions/1657883/variable-number-of-arguments-in-c

Thread
http://bituse.info/winapi/14
http://stackoverflow.com/questions/1372967/

Multithread Flag for DirectX
http://okwave.jp/qa/q4219151.html
Add flag at CreateDevice

*/

#pragma once

#include "../lib/lib_Window.hpp"
#include "../lib/ci_ext/object.hpp"
#include "../stage/stageInfo.h"
#include <Windows.h>
#include <unordered_map>

namespace scene {
	enum class Scene {
		LOGO,
		TITLE,
		STAGE_SELECT,
		STAGE,
		GAME_OVER,
		END,
		
	};

	class Loading final : public ci_ext::Object {
	public:
		struct SendData {
			Scene nextScene;
			std::string cmd;
		};
		SendData sendData_;
		static DWORD WINAPI LoadNextScene(SendData* pSendData);

		//Stage Info Buffer
		static map::StageInfo stageInfo_;

	
		Loading(const Scene& nextScene, const std::string& command = "");
		~Loading();

		void init() override;
		void update() override;

	private:
		void nextScene(const SendData& sendData);

		static void loadResource(const SendData& sendData);
		static void loadResLogo();
		static void loadResTitle();
		//static void loadResStageSelect();
		static void loadResStage(const std::string& stageName);
		static void loadResGameOver();
		static void loadResEnd();

		void releaseResource();

	};

}