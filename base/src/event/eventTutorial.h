#pragma once

#include "eventBase.h"
#include "../player/player.h"
#include "../effect/effectManager.h"
#include <fstream>


namespace gameevent
{
  
	class EventTutorial : public EventBase
	{
		struct DIALOG
		{
			std::string resName_jp;
			std::string resName_en;
			int srcsize_jp_x, srcsize_jp_y;
			int srcsize_en_x, srcsize_en_y;
		};
		DIALOG dialogs_;

			
	private:
		bool release_;
		bool isgetText_;
		int type_;
		float text_jp_pos_x_, text_jp_pos_y_;
		float text_en_pos_x_, text_en_pos_y_;
		int srcsize_jp_x_, srcsize_jp_y_;
		int srcsize_en_x_, srcsize_en_y_;
		std::string resName_jp_;
		std::string resName_en_;
		int alpha_;
		float text_scale_;
		const std::string fileName_ = "res/tutorial/tutorial.txt";  

		std::weak_ptr<ci_ext::Object> map_;
		ci_ext::DrawObjf tutorial_box_;
		float tutorial_box_scale_x;
		float tutorial_box_scale_y;

		std::string mapName_;
		inline void getMapPtr();
		float ran;

	public:
		EventTutorial(const std::istringstream& command);
		virtual ~EventTutorial() {  };
		void init() override;
		void resume() override;
		void update() override;
		void render() override;
		void renderLater() override;
		void setTutorialText();
		void loadFile(std::string filename, std::string mapName);

	
	};


}