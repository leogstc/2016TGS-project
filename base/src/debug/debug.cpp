#include "debug.h"
#include "../config.h"

namespace debug {
	void MouseMoveCamera()
	{
		static POINT mPos;
		if (gplib::input::CheckPush(gplib::input::KEY_MOUSE_LBTN)) {
			auto mouse = gplib::input::GetMousePosition();
			mPos = mouse;
		}
		else if (gplib::input::CheckPress(gplib::input::KEY_MOUSE_LBTN)) {
			auto mouse = gplib::input::GetMousePosition();
			if (mouse.x != mPos.x || mouse.y != mPos.y) {
				POINT nowCam = gplib::camera::GetLookAt();
				gplib::camera::SetLookAt((float)nowCam.x + (mPos.x - mouse.x), (float)nowCam.y + (mPos.y - mouse.y));
				mPos = mouse;
			}
		}
	}

	POINT GetGameMousePosition()
	{
		POINT mouse = gplib::input::GetMousePosition();
		POINT look = gplib::camera::GetLookAt();
		POINT pt;
		float cx = (float)(mouse.x - gplib::system::WINW / 2);
		float cy = (float)(mouse.y - gplib::system::WINH / 2);
		pt.x = (int)((cx + (float)look.x) / config.GAME_SCALE);
		pt.y = (int)((cy + (float)look.y) / config.GAME_SCALE);
		//gplib::debug::TToM("realMouse(%d,%d) gamePos(%d,%d) | look(%d,%d) scale(%.2f)", mouse.x, mouse.y, pt.x, pt.y, look.x, look.y, config.GAME_SCALE);

		return pt;
	}
}