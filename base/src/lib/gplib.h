//**************************************************************************************//
//
//　GP　ライブラリ
//		gplib.h
//
//customized gplib
// written by S.Ueyama
//
//**************************************************************************************//

//**************************************************************************************//
//インクルード
//**************************************************************************************//
#pragma once

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_DEPRECATE 1
#pragma warning(disable : 4995)	

//DirectSound DorectShowを使わない場合はコメントアウト
#define USE_DIRECTSOUND
#define USE_DIRECTSHOW

#include <crtdbg.h>
#include <windows.h>
#include <d3dx9.h>

#include <string>
#include <random>
#include <memory>
#include <cassert>


//**************************************************************************************//
//マクロ　変数　宣言
//**************************************************************************************//

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//--------------------------------------------------
// ##### Customized by Sui
//--------------------------------------------------
#ifndef _Defined_Point2f
#define _Defined_Point2f
struct Point2f {
	float x, y;
	Point2f(){}
	Point2f(float x, float y) {
		this->x = x;
		this->y = y;
	}
	Point2f(POINT p) {
		this->x = (float)p.x;
		this->y = (float)p.y;
	}
	void operator=(const float& i) {
		x = y = i;
	}
};
#endif
//--------------------------------------------------

namespace gplib
{
	//キャスト関連
	// weak_ptr → shared_ptr(継承クラスが対象)
	template<typename T1, typename T2>
	std::shared_ptr<T1> weak_to_shared(const std::weak_ptr<T2>& ptr)
	{
		return std::static_pointer_cast<T1>(ptr.lock());
	}

	// weak_ptr → shared_ptr(同じクラス)
	template<typename T>
	std::shared_ptr<T> weak_to_shared(const std::weak_ptr<T>& ptr)
	{
		return std::static_pointer_cast<T>(ptr.lock());
	}

	// shared_ptr → shared_ptr(継承クラスが対象)
	template<typename T1, typename T2>
	std::shared_ptr<T1> shared_cast(const std::shared_ptr<T2>& ptr)
	{
		return std::static_pointer_cast<T1>(ptr);
	}

	namespace input
	{
		// JOYPADの定義
		enum JOYID{ JOY1, JOY2 };
		enum JOY4DIR
		{
			JOY_DOWN = 0x1000,
			JOY_LEFT = 0x1001,
			JOY_UP = 0x1002,
			JOY_RIGHT = 0x1004
		};

		// マウスホイールの定義
		enum WHEEL
		{
			MWHEEL_UP = -1,
			MWHEEL_NEUTRAL,
			MWHEEL_DOWN,
		};

		// キーの状態の定義
		enum KEYSTATE{ FREE_KEY, PUSH_KEY, HOLD_KEY, PULL_KEY };

		// キーの定義
		enum CODE_KEY
		{
			KEY_NEUTRAL = -1,				// 何も押されていない
			KEY_DOWN,						//下キー
			KEY_LEFT,						//左キー
			KEY_UP,							//上キー
			KEY_RIGHT,						//右キー
			KEY_BTN0,						//ボタン0
			KEY_BTN1,						//ボタン1
			KEY_BTN2,						//ボタン2
			KEY_SPACE,						//スペースキー
			KEY_F1,							//F1キー
			KEY_F2,							//F2キー
			KEY_F3,							//F1キー
			KEY_F4,							//F2キー

			KEY_BTNMAX,						//キーボードの最大数　キー追加の際はこの上に記述

			KEY_MOUSE_LBTN = KEY_BTNMAX,	//マウスの左ボタン
			KEY_MOUSE_RBTN,					//マウスの右ボタン
			KEY_MAX							//最大数
		};
	}
  namespace draw{
    // 半透明描画の定義
    enum RENDER_MODE
    {
      NORMAL,		//半透明描画
      ADD,		//加算合成
      HALFADD,	//半加算合成
      MULTIPL,	//乗算合成
      SUBTRACT,	//減算
      BY,

      BF_MAX
    };
  }
	// エラーダイアログの定義
	enum DLG_ERROR
	{
		DLG_ERROR_INVALIDBUFFER,	//getのポインタがNULLになっている
		DLG_ERROR_CANCEL,			//キャンセルが押された
		DLG_OK
	};

	namespace math
	{
		// PIの定義
		const double	PI_D = M_PI;
		const float		PAI = static_cast<float>(PI_D);
		const float		PI = PAI;
	}


	//**************************************************************************************//
	//外部変数
	//**************************************************************************************//
	namespace system{
		extern	HWND		hWnd;				//ウインドウハンドル
		extern	const char	USERNAME[256];		//タイトル名
		extern	const int 	WINW;				//WINDOW幅
		extern	const int		WINH;				//WINDOW高さ
		extern	bool		WindowMode;			//ウインドウモード
		extern	float		FrameTime;			//フレームタイム
		//extern const int PLAYER_NUM;			//　プレイヤーの数
		const int	PLAYER_NUM = 5;

		extern	int			KeyboardMode;			//強制キーボードフラグ
		extern	int			nowStageNo;

		extern const int  REFRESHRATE;
		extern const float  ONEFRAME_TIME;
	}

	//**************************************************************************************//
	// 外部宣言
	//**************************************************************************************//
	namespace system
	{

  	void Quit();

		// 以下、アプリ側で用意する関数
		// アプリ起動時に一度だけ呼ばれる
		void OnCreate();
		// アプリ終了時に一度だけ呼ばれる
		void OnDestroy();
		// アプリの再描画の度に呼ばれる
		void GameLoop();
    void DelDx();
    void InitDx();

		// アプリの処理を実行
		int  DoWindow(HINSTANCE hInstance, HINSTANCE hPreInst,
			LPSTR lpszCmdLine, int nCmdShow);

		void GetIniFileDataStr(std::string section, std::string keyname, std::string &buff, std::string filename);
		int  GetIniFileDataInt(std::string section, std::string keyname, std::string filename);

	}

	namespace camera
	{
    void StepCamera();
		// カメラ制御
    void InitCamera(const float x, const float y, const float addpx, const float addpy, const int id = 0);
    void SetLookAt(const float x, const float y, const int id = 0);
    void AddCameraForce(const float x, const float y, const int id = 0);
    void SetCameraTarget(const float x, const float y, const int speed, const int id = 0);
    void SetScale(const float scale, const int id = 0);
    float GetScale(const int id = 0);
    void AddScale(const float add, const int id = 0);
    void AddScale(const float add, const int step, const int id = 0);
    POINT GetCameraPos(const int id = 0);
    POINT GetLookAt(const int id = 0);

    bool InScreenCamera(const int x, const int y, const int w, const int h, const int id = 0);
    bool InScreenCamera(const RECT& rt, const int id = 0);
    bool InScreenCameraCenter(const float x, const float y, const float w, const float h, const int id = 0);

    POINT GetCursorPosition(const int id = 0);
		POINT GetCursorPositionNC();
    void ChangeScreenMouse(POINT& pt, const int id = 0);
    void GetCursorPosition(POINT& pt, const int id = 0);
    void GetCursorPositionNC(POINT* pt_ptr, const int id = 0);

    RECT GetScreenRect(const int id = 0);

		// SetCameraTargetの別名(互換性用途)
		inline void SetLookAtTarget(const float x, const float y, const int speed) { SetCameraTarget(x, y, speed); }
    void SetUp();
    void SetCameraId(int id);
    int GetCameraId();
    void SetFrameSize(int id, int w, int h);
	}

	namespace stage{
		//ステージ管理関連
		void ChangeStage(const int);
		bool InitStage();
		void InitStageFlag();
		int  CheckStage();
	}

	namespace draw{

#define ARGB D3DCOLOR_ARGB
		
	struct CUSTOMVERTEX {
		FLOAT x, y, z;
		FLOAT rhw;
		DWORD color;
		FLOAT u, v;
	};

		void InitImgTable();
  		void DelImgTable();

//描画関連
		//void Clear();
		void Clear(int color = D3DCOLOR_XRGB(0, 0, 0));		void Refresh();

		//画像関連 
		void LoadObject(const std::string& resname, const std::string& filename,
			D3DCOLOR Transparent = ARGB(255, 0, 0, 0));
		void DeleteObject(const std::string& resname);
		void DeleteAll();
		//機能追加 渡加敷
		bool ExistObject(const std::string& resname);
		void SetRenderMode(int Mode);
		void EndRenderMode();

		void Graph(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			float	degree = 0.f,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void GraphLeftTop(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
      		float	degree = 0.f, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void Graph(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
      		float	degree = 0.f,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void GraphLeftTop(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
      		float	degree = 0.f, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Line(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size
			);
    void GraphNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
      		float	degree = 0.f,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void GraphLeftTopNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
      		float	degree = 0.f, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void GraphNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			float	degree = 0, 
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
    void GraphLeftTopNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY,
			int sizeX, int sizeY,
			float	degree = 0, POINT *center = nullptr,
			float	scaleX = 1.0f, float scaleY = 1.0f,
			u_char a = 255,
			u_char r = 255,
			u_char g = 255,
			u_char b = 255
			);
		void Box(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void BoxCenter(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void BoxNC(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void BoxCenterNC(
			int left, int top, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri
			);
		void LineNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size
			);

		void Clear2D();
		void Clear3D();
		void Refresh2D();
		bool Refresh3D();

		void CkRect(const RECT& rt, D3DCOLOR color = ARGB(255, 255, 0, 0));
		void CkRectNC(const RECT& rt, D3DCOLOR color = ARGB(255, 255, 0, 0));
		D3DSURFACE_DESC Draw_GetImageSize(const std::string& resname);
		SIZE GetImageSize2(const std::string& resname);
		unsigned int GetImageWidth(const std::string& resname);
		unsigned int GetImageHeight(const std::string& resname);

    void SetViewport(RECT rt, float minZ, float maxZ);


	//2016 2/25 渡加敷
	void Polygon(const std::vector<CUSTOMVERTEX> &poly, const std::string& resname, DWORD blend_color);


	//----- Polygon 描画関連
	struct Vertex {
		float x, y, z;
		D3DCOLOR color;
		Vertex(){}
		Vertex(float x1, float y1, float z1, D3DCOLOR color = 0xFFFFFFFF)
			:
			x(x1), y(y1), z(z1), color(color)
		{}
	};

	//テクスチャ・画像　用
	struct VertexUV : Vertex
	{
		float u, v;
		VertexUV(){}
		VertexUV(float x1, float y1, float z1, float u1, float v1, D3DCOLOR color = 0xFFFFFFFF)
			:
			Vertex(x1, y1, z1, color),
			u(u1), v(v1)
		{
		}
	};

	inline std::vector<DWORD> backupTextureStageState(DWORD stage);
	inline void restoreTextureStageState(DWORD stage, const std::vector<DWORD>& data);
	
	void PolygonPic(
      std::vector<VertexUV> vertices,
      const std::string& resname,
	  float degree = 0.0f,
      const D3DCOLOR color = 0xFFFFFFFF
      );

	void PolygonPicGradient(
		std::vector<VertexUV> vertices,
		const std::string& resname,
		float degree = 0.0f
		);

	void Polygon(
		const std::vector<Vertex> vertices,
		D3DCOLOR color);
	void Polygon(
		POINT pos1, POINT pos2, POINT pos3,
		float z,
		D3DCOLOR color);
	void Polygon(
		float x1, float y1, float x2, float y2, float x3, float y3,
		float z,
		D3DCOLOR color);

	void PolygonGradient(const std::vector<Vertex> vertices);
	void PolygonGradient(
		POINT pos1, POINT pos2, POINT pos3,
		float z,
		D3DCOLOR color1, D3DCOLOR color2, D3DCOLOR color3);
	void PolygonGradient(
		float x1, float y1, float x2, float y2, float x3, float y3,
		float z,
		D3DCOLOR color1, D3DCOLOR color2, D3DCOLOR color3);

	void Circle(float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	void CircleNC(float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	
	void CircleGradient(float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR innerColor = 0xFFFFFFFF, D3DCOLOR outerColor = 0xFF000000);
	void CircleGradientNC(float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR innerColor = 0xFFFFFFFF, D3DCOLOR outerColor = 0xFF000000);
	
	void CirclePic(const std::string& resname, float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	void CirclePicNC(const std::string& resname, float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	
	//resname1 modulate resname2
	void CirclePicMod(const std::string& resname1, const std::string& resname2, float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	void CirclePicModNC(const std::string& resname1, const std::string& resname2, float x, float y, float z, float radius, float percent = 100.0f, float startAngle = 0.0f, float stepAngle = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF);
	
	//Draw picture as rectangle from 90deg to 450deg using less polygons
	void CirclePicRectUp(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF
		);
	void CirclePicRectUpNC(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f, bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF
		);
	void CirclePicRectUpCcwNC(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f, D3DCOLOR color = 0xFFFFFFFF
		);
	void CirclePicRectUpCwNC(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f, D3DCOLOR color = 0xFFFFFFFF
		);
	
	//Draw picture as rectangle
	void CirclePicRect(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float startAngle = 0.0f, float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f,
		bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF
		);
	void CirclePicRectNC(const std::string& resname,
		float x, float y, float z, float radius, float percent = 100.0f,
		float startAngle = 0.0f, float stepAngle = 1.0f, float scalex = 1.0f, float scaley = 1.0f,
		bool counterClockWise = true, D3DCOLOR color = 0xFFFFFFFF
		);
	}
	namespace font{
		void Init();
    	void Delete();

		//フォント関連	
		void Text(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void TextNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int  Text(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int  TextNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void TextXY(int x, int y, const std::string& msg, D3DCOLOR Textcolor, int fontID = 0);
		void TextXYNC(int x, int y, const std::string& msg, D3DCOLOR Textcolor, int fontID = 0);
		void Create(int Num, int size, LPCTSTR	fontname);
		void CreateItalic(int Num, int size, LPCTSTR	fontname);

		//追加
		void TextLeftMiddle(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void TextLeftMiddleNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextLeftMiddle(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextLeftMiddleNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void TextCenter(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		void TextCenterNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenterNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenter(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenterTopNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenterTop(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenterBottomNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
		int TextCenterBottom(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID);
	}
	namespace input{
		void InitPlayerInput();
    	void DeletePlayerInput();

		WHEEL GetWheelRoll();
		int GetWheelDelta();
		//入力関連
		void CheckKey();
		bool CheckPress(int keyno, int interval = 1, int player = 0);	//押されているかチェック
		bool CheckAnyPress(int keyno);
		bool CheckFree(int keyno, int interval = 1, int player = 0);		//はなされているかチェック
		bool CheckPush(int keyno, int player = 0);		//押したかをチェック
		bool CheckAnyPush(int keyno);
		bool CheckPull(int keyno, int player = 0);		//はなしたかをチェック
		int  GetKeyStatus(int keyno, int player = 0);	//キーの状態をチェック
		int  GetKeyFrameHold(int keyno, int player = 0);	//押されているフレーム数を取得
		int  GetKeyFrameFree(int keyno, int player = 0);	//はなされているフレーム数を取得
		void InitKeyInfo(int player = 0);				//ステータスとフレームを初期化
		void Key_GetKey(unsigned int Code, int* Flag);
		void SetKeyFormat(CODE_KEY key, DWORD keycode, int playernum);
		void SetKeysFormat(DWORD* keycode, int playernum);
		bool IsUsePad(int playerID);

		POINT GetMousePosition();				//マウスカーソルの座標を取得
		void  GetMousePosition(POINT* pt_ptr);//マウスカーソルの座標を取得
	}
	namespace debug{
		// デバッグ関連
		void Dbg_FileOut(const char *str, ...);
		void Dbg_ExitApp();  //アプリケーションを終了させる要求をWIndowsに送ります（メモリリークに注意！）
		void Dbg_DisplayToMessage(int x, int y, const char * str, ...);
		void Dbg_TilteToMessage(const char * str, ...);
		void Dbg_BoxToMessage(const char *str, ...);
		void Dbg_StringOutXY(int x, int y, char* str, ...);
		void Draw_ShowFps();
		DLG_ERROR Dbg_InputDialog(const char* get, int size, const char* title, const char* label);
		void Dbg_DisplayToMessageNC(int x, int y, const char * str, ...);

		// Releaseビルドでは無効になる
#ifdef _DEBUG

#define TToM(...)   Dbg_TilteToMessage(__VA_ARGS__)
#define FToM(...)   Dbg_FileOut(__VA_ARGS__)
#define BToM(...)   Dbg_BoxToMessage(__VA_ARGS__)
#define SToM(...)   Dbg_DisplayToMessage(__VA_ARGS__)
#define SToMNC(...)	Dbg_DisplayToMessageNC(__VA_ARGS__)
		inline void DFPS() { Draw_ShowFps(); }

#else
		inline void TToM(...) {}
		inline void FToM(...) {}
		inline void BToM(...) {}
		inline void SToM(...) {}
		inline void SToMNC(...) {}
		inline void DFPS() {}
#endif

		// Releaseビルドでも有効
#define TToMR(...)		Dbg_TilteToMessage(__VA_ARGS__)
#define FToMR(...)		Dbg_FileOut(__VA_ARGS__)
#define BToMR(...)		Dbg_BoxToMessage(__VA_ARGS__)
#define SToMR(...)		Dbg_DisplayToMessage(__VA_ARGS__)
#define SToMNCR(...)	Dbg_DisplayToMessageNC(__VA_ARGS__)
	}

	namespace bgm{
#ifdef USE_DIRECTSHOW
		typedef int DSHOWOBJ;
		//BGM再生関数
		void Init();
		void Del();
		void Delete(const std::string& resname);
		void DeleteAll(); //2016-06-12 追加
		void LoadFile(const std::string& resname, const std::string& filename);
		bool Play(const std::string& resname);
		void RateControl(const std::string& resname, float rate);
		void EndCheck();
		void Stop(const std::string& resname);
		void AllStop();
		LONGLONG GetCurrentPos(const std::string& resname);
		void SetStartPos(const std::string& resname);
		void VolumeControl(const std::string& resname, int volume);
		void Pause(const std::string& resname);
		LONGLONG GetEndPosition(const std::string& resname);
#endif
	}
	namespace se{
#ifdef USE_DIRECTSOUND
		typedef int DSOUNDOBJ;
		//SE再生関数
		void Init();
		bool Create();
		void CreateSecondaryBuffer();
		bool CreatePrimaryBuffer();
		bool Del();
		void Delete(const std::string& resname);
		void DeleteAll(); //2016-06-12 追加
		void Play(const std::string& resname);
		void PlayLoop(const std::string& resname);
		void LoadFile(const std::string& resname, const std::string& filename);
		void AllStop();
		void Stop(const std::string& resname);
		void EndCheck();
		void SetFrequency(const std::string& resname, int Fre);
		void SetVolume(const std::string& resname, float Vol);
#endif
	}
	namespace math{
		//数学関連
		bool RectCheck(const RECT& rt1, const RECT& rt2);
		bool RectCheck(const RECT *rt1, const RECT *rt2);
		bool CircleCheck(float x1, float y1, float r1, float x2, float y2, float r2);
		bool IsFrameOut_Center(float x, float y, float width, float height);
		bool IsFrameOut_LeftTop(float x, float y, float width, float height);

		float Calc_XYToRad(int x, int y);		//座標XYへの角度を求める
		float Calc_XYToSinCos(int x, int y, float *addx, float *addy);	//座標XYへの移動量XYを求める
		float Calc_RadToDegree(float rad);		//ラジアンから角度へ
		float Calc_DegreeToRad(float degree);		//角度からラジアンへ

		extern std::mt19937 engine;

		void RandomInit();

		// [min, max]の一様な乱数を取得
		template <typename T>
		T GetRandom(const T min, const T max) {
			std::uniform_real_distribution<T> range(min, max);
			return range(engine);
		}

		// 特殊化
		// ヘッダに実装を書く場合はinlineをつける
		template <>
		inline int GetRandom<int>(const int min, const int max) {
			std::uniform_int_distribution<> range(min, max);
			return range(engine);
		}

		// 点と点の角度を求める（-180~180）
		float RadianOfPoints2(float basisX, float basisY, float targetX, float targetY);
		float DegreeOfPoints2(float basisX, float basisY, float targetX, float targetY);

		// 二点間の距離をもとめる
		float Distance2(float x1, float y1, float x2, float y2);

		float ROUND_X(float angle, float length, float center = 0.f);
		float ROUND_Y(float angle, float length, float center = 0.f);
		float ROUND_Z(float angle, float length, float center = 0.f);

		void reverseRect(RECT& baseRect, int width);



		Point2f RotateXY(Point2f pos, Point2f center, float degree);
		Point2f RotateXY(float x, float y, float centerx, float centery, float degree);
		
		float VectorInnerProduct(float x1, float y1, float x2, float y2);
		float VectorOuterProduct(float x1, float y1, float x2, float y2);
		float VectorNorm2(float x, float y);

		//indexOrder = Polygon's Border
		Point2f CalcPolygonCenter2D(const std::vector<draw::Vertex> vertices, const int*const indexOrder);
		Point2f CalcPolygonCenterPic(const std::vector<draw::VertexUV> vertices);
	}

	namespace time{
		//高精度タイマー関連
		void  Reset(int id);
		void  Start(int id);
		bool  MarkOf1Sec(int id);
		bool  MarkOfTime(int id, float marktime);
		float GetCount(int id);
		float Update();
		float Get1FrameCount();
		void  StartTimerCount();
    bool  isCheck();
	}

	//拡張機能を使わない場合はコメントアウトすること
	//#include "ci_ext/ci_ext.hpp"
}

#ifdef CI_EXT
bool IsFrameOut_Center(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size);
bool IsFrameOut_LeftTop(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size);
#endif

