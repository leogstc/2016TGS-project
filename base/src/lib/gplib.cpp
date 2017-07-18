//**************************************************************************************//
//
//　GP　ライブラリ
//		gplib.cpp
//
// customized gplib written by S.Ueyama
// customized again by Sui
//
//**************************************************************************************//

#include "GpLib.h"
#include <dxerr.h>

#ifdef USE_DIRECTSOUND
#	include <dsound.h>
#endif

#include <mmsystem.h>
//#include <stdio.h>

#ifdef USE_DIRECTSHOW
#	include <dshow.h>
#endif

#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <random>
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>


// リンクするライブラリの指示
#pragma	comment(lib,"dxguid.lib")
#pragma	comment(lib,"d3dxof.lib")
#pragma	comment(lib,"dxguid.lib")
#pragma	comment(lib,"d3d9.lib")
#pragma	comment(lib,"d3dx9.lib")
#pragma	comment(lib,"winmm")
#pragma	comment(lib,"dinput8.lib")

#ifdef USE_DIRECTSHOW
#	pragma comment(lib,"strmiids")
#endif

#ifdef USE_DIRECTSOUND
#	pragma	comment(lib,"dsound.lib")
#endif


//**************************************************************************************//
//マクロ　変数　宣言
//**************************************************************************************//

// FIXME:他のライブラリで使われているので、名前空間に属さずに定義
//D3Draw 関連
LPDIRECT3D9				pD3D;
D3DPRESENT_PARAMETERS	D3DPP;
LPDIRECT3DDEVICE9		pD3DDevice;
LPD3DXSPRITE			pSprite;
LPD3DXFONT				pD3DXFont;
//2016 2/27
LPDIRECTDRAWCLIPPER pDDClipper;

namespace gplib {

	namespace system {

		HWND		hWnd;
		float		FrameTime;
		const int	REFRESHRATE = 60;
		const float	ONEFRAME_TIME = (1.0f / static_cast<float>(REFRESHRATE));

	}

	// DirectXのオブジェクトを安全に破棄する
	template<typename T>
	void RELEASE(T* ptr) {
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}

	///*
	//イメージ管理構造体
	struct ImgObj final
	{
		LPDIRECT3DTEXTURE9 tex;
		std::string filename;
		int color;
		D3DXIMAGE_INFO info;

		void init()
		{
			tex = nullptr;
			filename = "";
			color = 0;
			D3DXIMAGE_INFO temp = {};
			info = temp;
		}

		ImgObj()
		{
			init();
		}
	};
	//*/

	// "リソース名"により管理を行う。
	std::unordered_map<std::string, ImgObj>	ImgTable;

	//-------------------------------------------------------
	//BGM再生関連
#ifdef USE_DIRECTSHOW

	struct DxShow
	{
		IGraphBuilder*	pBuilder;
		IMediaControl*	pMediaControl;
		IMediaEvent*	pMediaEvent;
		IMediaSeeking*	pMediaSeeking;
		IBasicAudio*	pBasicAudio;

		int use;
	};

	struct BgmObj
	{
		DxShow		bgm;
		std::string	filename;
	};

	// BGMを名前で管理
	std::unordered_map< std::string, BgmObj> BgmTable;

#endif

	//-------------------------------------------------------
	//SE再生関連
#ifdef USE_DIRECTSOUND
	// DirectSound8
	LPDIRECTSOUND8          lpDS;
	// プライマリサウンドバッファ
	LPDIRECTSOUNDBUFFER		lpPrimary;

	//同時発音できる最大数	
	enum { DSNONE, DSSTART, DSPLAY, DSSTOP, DSEND };
	struct PlayBuffer {
		LPDIRECTSOUNDBUFFER     pBuffer;		// プライマリサウンドバッファ
		int						ID;				//ベースになったバッファのID
		WORD					BufferSize;		//ベースになったバッファのサイズ
		int						State;			//バッファの状態
		bool					Loop;			//ループフラグ
	};

	const int				DSPLAYMAX = 10;
	struct SoundBuffer {
		LPDIRECTSOUNDBUFFER     sBuffer;	//セカンダリバッファ
		WORD										BufferSize;		//バッファのサイズ
		DWORD										Fre;
		struct PlayBuffer	PlayBuffer[DSPLAYMAX];
	};
	struct SeObj
	{
		SoundBuffer		se;
		std::string		filename;
	};

	std::unordered_map< std::string, SeObj>		SeTable;

#endif

	//##### Customized by S.Ueyama
	//INIファイルから文字列を取得する。
	void system::GetIniFileDataStr(std::string section, std::string keyname, std::string &buff, std::string filename)
	{
		char temp[256];

		GetPrivateProfileString((section.c_str()), (keyname.c_str()),
			"this string not get", temp, 1024, filename.c_str());
		buff = temp;
	}

	//INIファイルから整数を取得する。
	int system::GetIniFileDataInt(std::string section, std::string keyname, std::string filename)
	{
		return GetPrivateProfileInt(section.c_str(), keyname.c_str(), 0, filename.c_str());
	}
	//#####
	//**************************************************************************************//
	//　カメラ関連の定義
	//**************************************************************************************//
	namespace camera {

		//********************************************************************//
		//
		//				スクロール　関連関数
		//
		//********************************************************************//
		class Camera
		{
			float cx_, cy_;				//画面中央
			float addPx_, addPy_;	//1フレームあたりの移動量
			float tx_, ty_;				//注視点切り替え時目標
			int	  moveStep_;			//注視点切り替え時分割数 大きいほど遅くなる
			float scale_;					//拡大縮小率
			float addScale_;			//拡大率加算値
			int	  scalingStep_;	  //拡大率変化値
			float tScale_;				//目的拡大率
			int   w_, h_;         //カメラの描画範囲　幅　高

			float sx(const float x) const { return (x - viewLeft()) * scale_; }
			float sy(const float y) const { return (y - viewTop()) * scale_; }
			float offsetX() const { return w_ / 2.0f / scale_; };
			float offsetY() const { return h_ / 2.0f / scale_; };
		public:
			int  getMousePosX(const int x) { return static_cast<int>((x - w_ / 2) / scale() + cx()); }
			int  getMousePosY(const int y) { return static_cast<int>((y - h_ / 2) / scale() + cy()); }
			POINT getMousePos(const POINT& pt)
			{
				POINT r = { getMousePosX(pt.x), getMousePosY(pt.y) };
				return r;
			}

			float viewLeft()   const { return cx_ - offsetX(); }
			float viewTop()    const { return cy_ - offsetY(); }
			float viewRight()  const { return cx_ + offsetX(); }
			float viewBottom() const { return cy_ + offsetY(); }
			int   iviewLeft()  const { return static_cast<int>(viewLeft()); }
			int   iviewTop()   const { return static_cast<int>(viewTop()); }
			int   iviewRight() const { return static_cast<int>(viewRight()); }
			int   iviewBottom()const { return static_cast<int>(viewBottom()); }

			Camera(const float cx = system::WINW / 2, const float cy = system::WINH / 2,
				const float addpx = 0, const float addpy = 0, const float scale = 1.f)
				:
				cx_(cx),
				cy_(cy),
				addPx_(addpx),
				addPy_(addpy),
				scale_(scale)
			{
				tx_ = cx_;
				ty_ = cy_;
				tScale_ = scale_;
				w_ = system::WINW;
				h_ = system::WINH;
			}
			void setSize(int w, int h) {
				w_ = w;
				h_ = h;
			}
			//---------------------------------------------------------------------------------------
			//引数座標がウインドウの中心になるようにカメラ位置を変更する
			//---------------------------------------------------------------------------------------
			void setLookAt(const float x, const float y)
			{
				//				if (addPx_ == 0 && addPy_ == 0){
				tx_ = cx_ = (x);
				ty_ = cy_ = (y);
				//				}
			}

			//---------------------------------------------------------------------------------------
			//引数座標をカメラ位置に変更する
			//---------------------------------------------------------------------------------------
			//void setPos(float x, float y)
			//{
			//	tx_ = cx_ = x + offsetX();
			//	ty_ = cy_ = y + offsetY();
			//}

			//---------------------------------------------------------------------------------------
			//カメラ処理（毎フレーム呼ぶこと）
			//---------------------------------------------------------------------------------------
			void step()
			{
				//X軸移動
				if (fabs(cx_ - tx_) >= 2.f) {
					addPx_ = (tx_ - cx_) / moveStep_;
					if (fabs(addPx_) < 1.f) {
						if (addPx_ < 1.f) addPx_ = 1.f;
						if (addPx_ > -1.f) addPx_ = -1.f;
					}
				}
				else if (fabs(cx_ - tx_) < 2.f) {
					addPx_ = 0;
					cx_ = tx_;
				}
				else {
					addPx_ = 0;
					cx_ = tx_;
				}
				//Y軸移動
				if (fabs(cy_ - ty_) >= 2.f) {
					addPy_ = (ty_ - cy_) / moveStep_;
					if (fabs(addPy_) < 1.f) {
						if (addPy_ < 1.f) addPy_ = 1.f;
						if (addPy_ > -1.f) addPy_ = -1.f;
					}
				}
				else if (fabs(cy_ - ty_) < 2.f) {
					addPy_ = 0;
					cy_ = ty_;
				}
				else {
					addPy_ = 0;
					cy_ = ty_;
				}
				cx_ += addPx_;
				cy_ += addPy_;

				if (abs(addScale_) <= 0.005f) {
					scalingStep_ = 0;
					addScale_ = 0;
				}
				else {
					addScale_ = (float)((tScale_ - scale_) / scalingStep_);
					scale_ += addScale_;
				}
			}

			//---------------------------------------------------------------------------------------
			//1フレームあたりのカメラ移動量を設定
			//---------------------------------------------------------------------------------------
			void addForce(const float x, const float y)
			{
				addPx_ = x;
				addPy_ = y;
			}

			//---------------------------------------------------------------------------------------
			//指定フレーム分のカメラ移動量を設定
			//---------------------------------------------------------------------------------------
			void setTagetLength(const float x, const float y, const int step)
			{
				tx_ = x;
				ty_ = y;
				moveStep_ = step;
				addForce((tx_ - cx_) / moveStep_, (ty_ - cy_) / moveStep_);
			}

			//---------------------------------------------------------------------------------------
			//拡大率変更
			//---------------------------------------------------------------------------------------
			void setScale(const float scale)
			{
				scale_ = scale;
			}

			void addScale(const float offset)
			{
				scale_ += offset;
			}

			void addScaleSteps(const float newScale, const int step)
			{
				scalingStep_ = step;
				tScale_ = newScale;
				addScale_ = (float)((newScale - scale_) / step);
			}

			float cx()  const { return cx_; }
			float cy()  const { return cy_; }
			int   icx() const { return static_cast<int>(cx_); }
			int   icy() const { return static_cast<int>(cy_); }
			float scale() const { return scale_; }

			RECT getCameraViewRt() const
			{
				RECT rc = { iviewLeft(), iviewTop(), iviewRight(), iviewBottom() };
				return rc;
			}

			//入力スケールを、カメラを考慮したスケールに変換
			float changeScale(const float s) const { return s * scale_; }

			//入力座標に対して、ウインドウの座標(float)に変換
			float xchange(const float x) const { return sx(x); }
			float ychange(const float y) const { return sy(y); }
			float xchange(const int x)   const { return sx(static_cast<float>(x)); }
			float ychange(const int y)   const { return sy(static_cast<float>(y)); }

			//入力座標に対して、ウインドウの座標(int)に変換
			int   ixchange(const float x) const { return static_cast<int>(sx(x)); }
			int   iychange(const float y) const { return static_cast<int>(sy(y)); }
			int   ixchange(const int x)   const { return static_cast<int>(sx(static_cast<float>(x))); }
			int   iychange(const int y)   const { return static_cast<int>(sy(static_cast<float>(y))); }
		};

#define CAMERA_MAX 10
		static std::unique_ptr<Camera> cameras[CAMERA_MAX];
		static int nowCameraId = 0;

		void SetCameraId(int id)
		{
			nowCameraId = id;
		}
		int GetCameraId()
		{
			return nowCameraId;
		}

		void SetFrameSize(int id, int w, int h)
		{
			cameras[id]->setSize(w, h);
		}
		void SetUp()
		{
			for (auto& camera : cameras) {
				camera = std::unique_ptr<Camera>(new Camera(0.f, 0.f, 0.f, 0.f));
			}
		}
		//---------------------------------------------------------------------------------------
		// 初期化
		// x, y         視点の位置
		// addpx, addpy 視点が変わった時の、１フレームあたりの移動量
		//---------------------------------------------------------------------------------------
		void InitCamera(const float x, const float y, const float addpx, const float addpy, const int id)
		{
			SetLookAt(x, y, id);
		}

		//---------------------------------------------------------------------------------------
		// カメラ更新
		//---------------------------------------------------------------------------------------
		void StepCamera()
		{
			for (auto& camera : cameras) {
				camera->step();
			}
		}

		//---------------------------------------------------------------------------------------
		// 視点指定
		// x, y 視点
		//---------------------------------------------------------------------------------------
		void SetLookAt(const float x, const float y, const int id)
		{
			cameras[id]->setLookAt(x, y);
		}

		//---------------------------------------------------------------------------------------
		//カメラ左上点指定　→　基本的にSetLookAtで問題なし
		//---------------------------------------------------------------------------------------
		//void SetPos(float x, float y)
		//{
		//	camera->setPos(x, y);
		//}

		//---------------------------------------------------------------------------------------
		// １フレームあたりのカメラの視点移動量を指定
		// x, y 視点移動量
		//---------------------------------------------------------------------------------------
		void AddCameraForce(const float x, const float y, const int id)
		{
			cameras[id]->addForce(x, y);
		}

		//---------------------------------------------------------------------------------------
		// 視点の変更
		// x, y   移動先の視点
		// speed １フレームあたりの移動量
		//---------------------------------------------------------------------------------------
		void SetCameraTarget(const float x, const float y, const int speed, const int id)
		{
			cameras[id]->setTagetLength(x, y, speed);
		}

		//---------------------------------------------------------------------------------------
		// 倍率の変更
		//---------------------------------------------------------------------------------------
		void SetScale(const float scale, const int id)
		{
			cameras[id]->setScale(scale);
		}

		//---------------------------------------------------------------------------------------
		// 倍率の取得
		//---------------------------------------------------------------------------------------
		float GetScale(const int id)
		{
			return cameras[id]->scale();
		}

		//---------------------------------------------------------------------------------------
		// 倍率を
		//---------------------------------------------------------------------------------------
		void AddScale(const float add, const int id)
		{
			cameras[id]->addScaleSteps(add, 0);
		}

		//---------------------------------------------------------------------------------------
		// 倍率
		//---------------------------------------------------------------------------------------
		void AddScale(const float add, const int step, const int id)
		{
			cameras[id]->addScaleSteps(add, step);
		}

		//---------------------------------------------------------------------------------------
		// 視点を取得
		//---------------------------------------------------------------------------------------
		POINT GetLookAt(const int id)
		{
			POINT pt = { cameras[id]->icx(), cameras[id]->icy() };
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// 位置(ウィンドウ左上座標)を取得
		//---------------------------------------------------------------------------------------
		POINT GetCameraPos(const int id)
		{
			POINT pt = { cameras[id]->iviewLeft(), cameras[id]->iviewTop() };
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// カメラ位置を加えたマウスのカーソル座標を取得
		//---------------------------------------------------------------------------------------
		POINT GetCursorPosition(const int id)
		{
			return cameras[id]->getMousePos(input::GetMousePosition());
		}

		//---------------------------------------------------------------------------------------
		// マウスのカーソル座標を取得
		// 左上が(0,0)
		//---------------------------------------------------------------------------------------
		POINT GetCursorPositionNC()
		{
			POINT pt = input::GetMousePosition();
			return pt;
		}

		//---------------------------------------------------------------------------------------
		// マウスの座標にカメラ位置を加える
		//---------------------------------------------------------------------------------------
		void ChangeScreenMouse(POINT& pt, const int id)
		{
			pt = cameras[id]->getMousePos(pt);
		}

		//---------------------------------------------------------------------------------------
		// カメラ位置を加えたマウスのカーソル座標を取得
		//---------------------------------------------------------------------------------------
		void GetCursorPosition(POINT& pt, const int id)
		{
			pt = cameras[id]->getMousePos(input::GetMousePosition());
		}

		//---------------------------------------------------------------------------------------
		// マウスのカーソル座標を取得
		// 左上が(0,0)
		//---------------------------------------------------------------------------------------
		void GetCursorPositionNC(POINT* pt_ptr)
		{
			input::GetMousePosition(pt_ptr);
		}

		//---------------------------------------------------------------------------------------
		// カメラを通した画面サイズを取得
		// カメラの倍率でサイズが変化します
		//---------------------------------------------------------------------------------------
		RECT GetScreenRect(const int id)
		{
			RECT cameraRt = cameras[id]->getCameraViewRt();
			//	debug::TToM("%d:%d:%d:%d",cameraRt.left,cameraRt.top,cameraRt.right,cameraRt.bottom);
			return cameraRt;
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		//---------------------------------------------------------------------------------------
		bool InScreenCamera(const RECT& rt, const int id)
		{
			return math::RectCheck(&rt, &GetScreenRect(id));
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		// x, y 矩形の中心位置
		// w, h 矩形の幅と高さ
		//---------------------------------------------------------------------------------------
		bool InScreenCameraCenter(const float x, const float y, const float w, const float h, const int id)
		{
			RECT ObjRect = {
				static_cast<LONG>(x - w / 2.0f),
				static_cast<LONG>(y - h / 2.0f),
				static_cast<LONG>(x + w / 2.0f),
				static_cast<LONG>(y + h / 2.0f)
			};
			return InScreenCamera(ObjRect, id);
		}

		//---------------------------------------------------------------------------------------
		// 矩形がカメラを通した画面内にあるかどうか判定
		// x, y 矩形の左上座標
		// w, h 矩形の幅と高さ
		//---------------------------------------------------------------------------------------
		bool InScreenCamera(const int x, const int y, const int w, const int h, const int id)
		{
			RECT ObjRect = {
				x, y,
				x + w, y + h
			};
			return InScreenCamera(ObjRect, id);
		}

	}


	namespace math {

		//---------------------------------------------------------------------------------------
		//ランダム利用
		//---------------------------------------------------------------------------------------
		//int i = math::GetRandom(10,100); //10から100までのランダムな整数
		//float f = math::GetRandom(10.0f,100.0f); //10.0fから100までのランダムな実数
		//---------------------------------------------------------------------------------------
		//// [min, max]の一様な乱数を取得
		//template <typename T>
		//inline T GetRandom(const T min, const T max) {
		//	std::uniform_real_distribution<T> range(min, max);
		//	return range(engine);
		//}
		//// 特殊化はこっちに書く…
		//template <>
		//inline int GetRandom<int>(const int min, const int max) {
		//	std::uniform_int_distribution<> range(min, max);
		//	return range(engine);
		//}

		std::mt19937 engine;

		//---------------------------------------------------------------------------------------
		//ランダムの初期化
		//---------------------------------------------------------------------------------------
		void RandomInit() {
			std::random_device device;
			engine.seed(device());
		}


		//***************************************************************************************
		// 数学関連
		//***************************************************************************************
		//---------------------------------------------------------------------------------------
		//２点間の角を求める：弧度法
		//---------------------------------------------------------------------------------------
		float RadianOfPoints2(float basisX, float basisY, float targetX, float targetY)
		{
			float x = targetX - basisX;
			float y = -(targetY - basisY);  //ディスプレイ座標に合わせるために、マイナスにする
			return std::atan2(y, x);
		}

		//---------------------------------------------------------------------------------------
		//２点間の角度を求める：度数法
		//---------------------------------------------------------------------------------------
		float DegreeOfPoints2(float basisX, float basisY, float targetX, float targetY)
		{
			return Calc_RadToDegree(RadianOfPoints2(basisX, basisY, targetX, targetY));
		}

		//---------------------------------------------------------------------------------------
		//２点間の距離を求める
		//---------------------------------------------------------------------------------------
		float Distance2(float x1, float y1, float x2, float y2)
		{
			float x = pow(x1 - x2, 2);
			float y = pow(y1 - y2, 2);
			return std::sqrt(x + y);
		}

		//-------------------------------------------------------------//
		//矩形同士の重なりを調べる
		//-------------------------------------------------------------//
		bool RectCheck(const RECT& rt1, const RECT& rt2)
		{
			return (rt1.left < rt2.right &&
				rt2.left < rt1.right &&
				rt1.top < rt2.bottom &&
				rt2.top < rt1.bottom);
		}

		bool RectCheck(const RECT *rt1, const RECT *rt2)
		{
			return RectCheck(*rt1, *rt2);
		}

		//--------------------------------------------------
		// ○と○の当たり判定
		//--------------------------------------------------
		bool CircleCheck(float x1, float y1, float r1, float x2, float y2, float r2)
		{
			float x = abs(x1 - x2);
			float y = abs(y1 - y2);
			float r = r1 + r2;

			//現在の位置 <= 当たり判定Range
			if ((x*x) + (y*y) <= r*r)
				return true;
			else
				return false;
		}

		//-------------------------------------------------------------//
		//指定座標xyが画面外にあるかを調べます
		//-------------------------------------------------------------//
		bool IsFrameOut_Center(float x, float y, float width, float height, const int id)
		{
			width /= 2.0f;
			height /= 2.0f;
			x -= camera::cameras[id]->viewLeft();
			y -= camera::cameras[id]->viewTop();
			return ((x - width) < 0 || (y - height) < 0 || (x + width) > system::WINW || (y + height) > system::WINH);
		}
		bool IsFrameOut_LeftTop(float x, float y, float width, float height, const int id)
		{
			x -= camera::cameras[id]->viewLeft();
			y -= camera::cameras[id]->viewTop();
			return (x < 0 || y < 0 || (x + width) > system::WINW || (y + height) > system::WINH);
		}
#ifdef CI_EXT
		bool IsFrameOut_Center(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size)
		{
			return IsFrameOut_Center(pos.x(), pos.y(), size.x(), size.y());
		}
		bool IsFrameOut_LeftTop(const ci_ext::Vec3f& pos, const ci_ext::Vec3f& size)
		{
			return IsFrameOut_LeftTop(pos.x(), pos.y(), size.x(), size.y());
		}

#endif

		//-------------------------------------------------------------//
		//幅X及び高さYの値から角度θを求め、CosSinの値を求める
		//戻り値　ラジアン
		//-------------------------------------------------------------//
		// 例
		//float rad = CalcXYToSinCos(30,30);
		//-------------------------------------------------------------//
		float Calc_XYToRad(int x, int y)
		{
			return static_cast<float>(atan2((double)y, (double)x));
		}

		//-------------------------------------------------------------//
		//幅X及び高さYの値から角度θを求め、CosSinの値を求める
		//戻り値　ラジアン
		//-------------------------------------------------------------//
		// 例
		//float rad = CalcXYToSinCos(30,30,&AddPx,&AddPy);
		//-------------------------------------------------------------//
		float Calc_XYToSinCos(int x, int y, float *addx, float *addy)
		{
			float rad = atan2(static_cast<float>(y), static_cast<float>(x));
			*addx = cos(rad);
			*addy = -sin(rad);

			return rad;
		}

		//-------------------------------------------------------------//
		//ラジアン値から角度に変換する
		//戻り値　float 角度
		//-------------------------------------------------------------//
		//float deg = RadToDegree(r);
		//-------------------------------------------------------------//
		float Calc_RadToDegree(float rad)
		{
			float d = (float)(rad * 180.0f / PAI);
			if (d < 0) d += 360;
			return d;
		}
		//-------------------------------------------------------------//
		//角度からラジアン値に変換する
		//戻り値　float ラジアン
		//-------------------------------------------------------------//
		//float rad = DegreeToRad(180);
		//-------------------------------------------------------------//
		float Calc_DegreeToRad(float degree)
		{
			return static_cast<float>(degree * 2.0f * M_PI / 360.0f);
		}
		//---------------------------------------------------------------------------------------
		//コサインを求める　角度に対してのX軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_X(float angle, float length, float center)
		{
			return (float)(cos(angle * M_PI * 2 / 360) * length) + center;
		}
		//---------------------------------------------------------------------------------------
		//サインを求める　角度に対してのY軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_Y(float angle, float length, float center)
		{
			return (float)(sin(-angle * M_PI * 2 / 360) * length) + center;
		}
		//---------------------------------------------------------------------------------------
		//コサインを求める：角度に対してのZ軸の長さを求める
		//---------------------------------------------------------------------------------------
		float ROUND_Z(float angle, float length, float center)
		{
			return (float)(tan(angle * M_PI * 2 / 360) * length) + center;
		}

		//---------------------------------------------------------------------------------------
		//四角形を反転する。
		//---------------------------------------------------------------------------------------
		void reverseRect(RECT& baseRect, int width)
		{
			RECT temp;
			SetRect(&temp, width - (baseRect.right), baseRect.top,
				width - (baseRect.left), baseRect.bottom);
			CopyRect(&baseRect, &temp);
		}

		//--------------------------------------------------
		// ##### Customized by Sui
		//--------------------------------------------------
		// Rotate
		Point2f RotateXY(Point2f pos, Point2f center, float degree)
		{
			Point2f p;
			float dx = pos.x - center.x;
			float dy = -(pos.y - center.y);
			p.x = (float)((dx * cos(degree * M_PI / 180)) - (dy * sin(degree * M_PI / 180))) + center.x;
			p.y = (float)(-((dx * sin(degree * M_PI / 180)) + (dy * cos(degree * M_PI / 180)))) + center.y;
			return p;
		}

		Point2f RotateXY(float x, float y, float centerx, float centery, float degree)
		{
			Point2f p;
			p = RotateXY(Point2f(x, y), Point2f(centerx, centery), degree);
			return p;
		}

		// Vector
		float VectorInnerProduct(float x1, float y1, float x2, float y2)
		{
			return x1*x2 + y1*y2;
		}
		float VectorOuterProduct(float x1, float y1, float x2, float y2)
		{
			return x1*y2 - y1*x2;
		}
		float VectorNorm2(float x, float y)
		{
			return VectorInnerProduct(x, y, x, y); //u・u = |u|^2
		}

		// Polygon 重心計算
		//http://www.not-enough.org/abe/manual/argo/center-of-mass.html
		//http://homepage1.nifty.com/gfk/polygon-G.htm
		//http://www3.pf-x.net/~chopper/home2/DirectX/CDX33.html
		Point2f CalcPolygonCenter2D(const std::vector<draw::Vertex> vertices, const int*const indexOrder)
		{
			if (vertices.size() < 3) return{ -1, -1 };

			const int* order = indexOrder;

			//ポリゴン
			float area = 0;
			float sum = 0;
			Point2f center(0, 0);

			Point2f vec1;
			Point2f vec2;
			Point2f pc; //三角形の中心点

			for (size_t i = 1; i < vertices.size() - 1; ++i) {


				vec1 = { vertices[*(order + i)].x - vertices[0].x, vertices[*(order + i)].y - vertices[0].y };
				vec2 = { vertices[*(order + i + 1)].x - vertices[0].x, vertices[*(order + i + 1)].y - vertices[0].y };

				// Outer = 四角形の面積
				// /2 = 三角形の面積
				area = VectorOuterProduct(vec1.x, vec1.y, vec2.x, vec2.y) / 2;
				sum += area;

				pc.x = (vertices[0].x + vertices[*(order + i)].x + vertices[*(order + i + 1)].x) / 3;
				pc.y = (vertices[0].y + vertices[*(order + i)].y + vertices[*(order + i + 1)].y) / 3;

				center.x += area * pc.x;
				center.y += area * pc.y;
			}

			center.x /= sum;
			center.y /= sum;

			return center;
		}

		Point2f CalcPolygonCenterPic(const std::vector<draw::VertexUV> vertices)
		{
			if (vertices.size() > 4) return Point2f(-1, -1);


			int order[] = { 0, 1, 3, 2 };
			std::vector<draw::Vertex> ver;

			for (size_t i = 0; i < vertices.size() && i < 4; ++i) {
				if (vertices[i].u == 0 && vertices[i].v == 0) {
					order[i] = 0;
				}
				else if (vertices[i].u == 0 && vertices[i].v == 1) {
					order[i] = 3;
				}
				else if (vertices[i].u == 1 && vertices[i].v == 0) {
					order[i] = 1;
				}
				else if (vertices[i].u == 1 && vertices[i].v == 1) {
					order[i] = 2;
				}

				draw::Vertex tmp = { vertices[i].x, vertices[i].y, vertices[i].z };
				ver.push_back(tmp);
			}
			return CalcPolygonCenter2D(ver, order);
		}
		//--------------------------------------------------
	}

	//********************************************************************//
	//
	//				高精度タイマー　関連関数
	//
	//********************************************************************//
	namespace time {

		const int	_TIMER_MAX = 10;

		//経過時間観測用
		LARGE_INTEGER _start[_TIMER_MAX];
		LARGE_INTEGER _systimer;
		LARGE_INTEGER _now;
		LARGE_INTEGER _sys;
		LARGE_INTEGER _lastFrame;

		//---------------------------------------------------------------------------------------
		//　経過時間のリセット
		//========================================
		//	//一秒毎に真を返す
		//	if(time::MarkOf1Sec(0)){
		//		処理
		//		time::Reset(0);
		//	}
		//	//５秒毎に真を返す
		//	if(time::MarkOfTime(0,5)){
		//		処理
		//		time::Reset(0);
		//	}
		//	time::Update();
		//---------------------------------------------------------------------------------------

		void Reset(int id)
		{
			QueryPerformanceCounter(&_start[id]);
		}
		//---------------------------------------------------------------------------------------
		//　経過時間の計測開始
		//---------------------------------------------------------------------------------------
		void Start(int id)
		{
			QueryPerformanceCounter(&_start[id]);
		}

		//---------------------------------------------------------------------------------------
		//　一秒経過マーカー
		//---------------------------------------------------------------------------------------
		bool MarkOf1Sec(int id)
		{
			return (static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart) >= 1.0f);
		}

		//---------------------------------------------------------------------------------------
		//　時間経過マーカー
		//---------------------------------------------------------------------------------------
		bool MarkOfTime(int id, float marktime)
		{
			return (static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart) >= marktime);
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得
		//---------------------------------------------------------------------------------------
		float GetCount(int id)
		{
			return  static_cast<float>(_now.QuadPart - _start[id].QuadPart) / static_cast<float>(_sys.QuadPart);
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得
		//---------------------------------------------------------------------------------------
		float GetCount()
		{
			return  static_cast<float>(_now.QuadPart - _systimer.QuadPart) / static_cast<float>(_sys.QuadPart);
		}


		//---------------------------------------------------------------------------------------
		//	タイマーの更新	1Frameにつき一回の呼び出し
		//	内部利用
		//---------------------------------------------------------------------------------------
		float Update()
		{
			QueryPerformanceCounter(&_now);
			system::FrameTime = static_cast<float>(_now.QuadPart - _lastFrame.QuadPart) / static_cast<float>(_sys.QuadPart);
			QueryPerformanceCounter(&_lastFrame);
			return system::FrameTime;
		}

		//---------------------------------------------------------------------------------------
		//　１フレームあたりの経過時間
		//	static float cnt;
		//  cnt += TimeGetOneFrameCount();
		//	if((int)cnt >= 1) //1秒経過
		//---------------------------------------------------------------------------------------
		float Get1FrameCount()
		{
			return system::FrameTime;
		}

		//---------------------------------------------------------------------------------------
		//　経過時間の取得開始
		//---------------------------------------------------------------------------------------
		void StartTimerCount()
		{
			QueryPerformanceCounter(&_systimer);
		}

		//---------------------------------------------------------------------------------------
		//　高精度タイマーが利用可能か調べる
		//  内部リセット込み
		//---------------------------------------------------------------------------------------
		bool isCheck()
		{
			if (!QueryPerformanceFrequency(&_sys))
				return false;
			QueryPerformanceCounter(&_lastFrame);
			StartTimerCount();
			Update();
			return true;
		}

	}

	//********************************************************************//
	//
	//				デバッグ　関連関数
	//
	//********************************************************************//
	namespace debug {


		//--------------------------------------------------------------------------------------------
		// FPSの表示
		//--------------------------------------------------------------------------------------------
		//　DD_Clear();
		//	DD_ShowFPS();
		//	DFPS();		でも可
		//　DD_Refresh();
		//--------------------------------------------------------------------------------------------
		void Draw_ShowFps()
		{
			static double FpsTime = (time::GetCount() + 1.0);
			static double FpsData = 0.0;
			static int FpsCnt = 1;
			if (time::GetCount() >= (unsigned)FpsTime) {
				FpsData = FpsCnt;
				FpsCnt = 0;
				FpsTime = (time::GetCount() + 1.0);
			}
			//FPSカウント
			FpsCnt++;
			char Buffer[1024];
			sprintf_s(Buffer, "fps=%3.2f:ft=%0.3f", FpsData, system::FrameTime);
			font::TextNC(0, 0, 0.0f, Buffer, ARGB(255, 255, 0, 0), 0);
		}

		//---------------------------------------------------------------------------------------
		//ウインドウへメッセージの表示
		//	引数
		//		x,y			表示位置
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_DisplayToMessage(int x, int y, const char * str, ...) {

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			font::TextXY(x, y,
				buf, ARGB(255, 255, 0, 0));
		}

		//---------------------------------------------------------------------------------------
		//ウインドウへメッセージの表示
		//	引数
		//		x,y			表示位置
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_DisplayToMessageNC(int x, int y, const char * str, ...) {

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			font::TextXYNC(x, y,
				buf, ARGB(255, 255, 0, 0));
		}

		//---------------------------------------------------------------------------------------
		//タイトルへメッセージの表示
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_TilteToMessage(const char * str, ...) {

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			SetWindowText(system::hWnd, (LPCSTR)buf);
		}

		//---------------------------------------------------------------------------------------
		//メッセージボックスへの表示
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_BoxToMessage(const char *str, ...) {

			char buf[256];
			vsprintf(buf, str, (char*)(&str + 1));
			MessageBox(NULL, (LPCSTR)buf, "Message", MB_OK);
		}

		//---------------------------------------------------------------------------------------
		//ファイルOUT関数
		//	引数
		//		str			フォーマット前文字列
		//		...			可変個引数指定
		//---------------------------------------------------------------------------------------
		void Dbg_FileOut(const char *str, ...) {

			FILE*	fp;
			char buff[128];
			vsprintf(buff, (char*)str, (char*)(&str + 1));
			strcat(buff, "\n");
			if ((fp = fopen("debug.txt", "at")) != NULL) {
				fprintf(fp, "%s", buff);
				fclose(fp);
			}
		}

		//---------------------------------------------------------------------------------------
		//アプリケーションを終了させる要求をWIndowsに送ります（メモリリークに注意！）
		//---------------------------------------------------------------------------------------
		void Dbg_ExitApp()
		{
			DestroyWindow(system::hWnd);
		}

		//-------------------------------------------------------------//
		// Dialog 関連define
#define IDD_DIALOG1                     101
#define ID_DLG_INPUT                    101
#define ID_EDIT                         1001
#define ID_LABEL                        1002
		//-------------------------------------------------------------//
		//	入力関数
		//-------------------------------------------------------------//
		struct DLG_DATA
		{
			char title[MAX_PATH];
			char label[MAX_PATH];
			char get[MAX_PATH];
			int size;
		};

		//-------------------------------------------------------------//
		//ダイアログプロシージャ
		//	外部ファイルから呼び出し出来ません
		//-------------------------------------------------------------//
		static int CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			static DLG_DATA* d_ptr = NULL;

			switch (msg)
			{
				//ダイアログ初期化処理
			case WM_INITDIALOG:
				d_ptr = (DLG_DATA*)lParam;	//呼び出し元からデータへのポインタを受け取る
				if (d_ptr->title[0] != '\0') SetWindowText(hDlg, d_ptr->title);	//タイトル変更
				if (d_ptr->label[0] != '\0') SetWindowText(GetDlgItem(hDlg, ID_LABEL), d_ptr->label);//ラベル変更
				SetFocus(hDlg);
				break;
				//ボタン処理
			case WM_COMMAND:
				switch (LOWORD(wParam))
				{
					//「ok」ボタンが押された
				case IDOK:
					GetDlgItemText(hDlg, ID_EDIT, d_ptr->get, MAX_PATH);
					EndDialog(hDlg, 1);
					break;
					//「キャンセル」ボタンもしくは「x」ボタンが押された
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
				default:
					return 0;	//Windowsに任せるので0を返す
				}
				break;
				//既定の処理：Windowsに任せる
			default:
				return 0;	//Windowsに任せるので0を返す
			}
			return 1;		//自分でメッセージを処理したので1を返す
		}

		//-------------------------------------------------------------//
		//文字列入力関数
		//	引数
		//		get			取得した文字の格納先
		//		size		getのサイズ
		//		title		タイトルの文字列
		//		label		エディットボックスに表示する文字列
		//-------------------------------------------------------------//
		//	使用例
		//		char str[100];
		//		Dbg_InputDialog(str, 100, "確認", "名前を入力してください");
		//-------------------------------------------------------------//
		DLG_ERROR Dbg_InputDialog(char* get, int size, const char* title, const char* label)
		{
			DLG_DATA data;

			if (get == NULL) return DLG_ERROR_INVALIDBUFFER;
			//タイトルコピー
			if (title != NULL)
			{
				memcpy(&data.title, title, MAX_PATH);
			}
			else
			{
				data.title[0] = '\0';
			}
			//ラベルコピー
			if (label != NULL)
			{
				memcpy(&data.label, label, MAX_PATH);
			}
			else
			{
				data.label[0] = '\0';
			}
			//ダイアログ表示
			if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(ID_DLG_INPUT),
				system::hWnd, (DLGPROC)DlgProc, (LPARAM)&data) == 0)
			{
				//「キャンセル」ボタンもしくは「x」ボタンが押された
				return DLG_ERROR_CANCEL;
			}
			//「ok」が押された（入力されていなくてもココにくる）
			memcpy(get, data.get, size - 1);
			get[size - 1] = '\0';
			return DLG_OK;
		}

	}


	//********************************************************************//
	//
	//				ステージ管理　関連関数
	//
	//********************************************************************//
	namespace stage {

		//ステージ番号
		int	nowStageNo;

		int _stageNo;
		int	_initFlag;

		//---------------------------------------------------------------------------------------
		//ステージを引数で指定されたステージに切り替える
		// ---------------------------------------------------------------------------------------
		void ChangeStage(const int stage)
		{
			_stageNo = stage;
			_initFlag = 1;
		}

		//---------------------------------------------------------------------------------------
		//ステージ切り替え時にステージ初期化を行うかを調べる
		//---------------------------------------------------------------------------------------
		bool InitStage()
		{
			if (_initFlag == 1)
			{
				_initFlag = 0;
				return true;
			}
			return false;
		}

		//---------------------------------------------------------------------------------------
		//ステージ管理関数の初期化
		//---------------------------------------------------------------------------------------
		void InitStageFlag()
		{
			_initFlag = 1;
			_stageNo = 0;
		}

		//---------------------------------------------------------------------------------------
		//ステージ番号を調べる
		//---------------------------------------------------------------------------------------
		int CheckStage()
		{
			return _stageNo;
		}

	}


	//********************************************************************//
	//
	//				キー　関連関数
	//
	//********************************************************************//
	namespace input {

		//ホイール値取得
		int delta;

		//キー情報構造体
		struct KEYINFO
		{
			int code;		// 仮想キーコード
			int status;		// キーの状態(KEYSTATE)
			int frameHold;	// 押されているフレーム数
			int frameFree;	// はなされているフレーム数
		};
		//キー情報
		static KEYINFO g_key[system::PLAYER_NUM][KEY_MAX];

		static KEYINFO padbase[KEY_MAX] =
		{
			//  仮想キーコード, KEYSTATE, 押されているフレーム数
			{ JOY_DOWN, FREE_KEY, 0 }, //DOWN,
			{ JOY_LEFT, FREE_KEY, 0 }, //LEFT,
			{ JOY_UP, FREE_KEY, 0 }, //UP,
			{ JOY_RIGHT, FREE_KEY, 0 }, //RIGHT,
			{ JOY_BUTTON1, FREE_KEY, 0 }, //BTN_0,
			{ JOY_BUTTON2, FREE_KEY, 0 }, //BTN_1,
			{ JOY_BUTTON3, FREE_KEY, 0 }, //BTN_2,
			{ JOY_BUTTON4, FREE_KEY, 0 }, //SPACE,
			{ VK_F1, FREE_KEY, 0 }, //F1,
			{ VK_F2, FREE_KEY, 0 }, //F2,
			{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
			{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
		};

		//キー割り当て
		//２Pでキーボード使用の際は書き換え必須
		static KEYINFO keybase[system::PLAYER_NUM][KEY_MAX] =
		{
			{
				//1P
				//  仮想キーコード, KEYSTATE, 押されているフレーム数
				{ VK_DOWN, FREE_KEY, 0 }, //DOWN,
				{ VK_LEFT, FREE_KEY, 0 }, //LEFT,
				{ VK_UP, FREE_KEY, 0 }, //UP,
				{ VK_RIGHT, FREE_KEY, 0 }, //RIGHT,
				{ 'A', FREE_KEY, 0 }, //BTN_0,
				{ 'S', FREE_KEY, 0 }, //BTN_1,
				{ 'D', FREE_KEY, 0 }, //BTN_2,
				{ VK_SPACE, FREE_KEY, 0 }, //SPACE,
				{ VK_F1, FREE_KEY, 0 }, //F1,
				{ VK_F2, FREE_KEY, 0 }, //F2,
				{ VK_F3, FREE_KEY, 0 }, //F1,
				{ VK_F4, FREE_KEY, 0 }, //F2,
				{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
				{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
			},
			{
				//2P
				//  仮想キーコード, KEYSTATE, 押されているフレーム数
				{ VK_DOWN, FREE_KEY, 0 }, //DOWN,
				{ VK_LEFT, FREE_KEY, 0 }, //LEFT,
				{ VK_UP, FREE_KEY, 0 }, //UP,
				{ VK_RIGHT, FREE_KEY, 0 }, //RIGHT,
				{ 'A', FREE_KEY, 0 }, //BTN_0,
				{ 'S', FREE_KEY, 0 }, //BTN_1,
				{ 'D', FREE_KEY, 0 }, //BTN_2,
				{ VK_SPACE, FREE_KEY, 0 }, //SPACE,
				{ VK_F1, FREE_KEY, 0 }, //F1,
				{ VK_F2, FREE_KEY, 0 }, //F2,
				{ MK_LBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_LBTN,
				{ MK_RBUTTON, FREE_KEY, 0 }, //KEY_MOUSE_RBTN,
			}
		};

		JOYINFO* JoyInfo;
		int padcount;
		bool usePad[system::PLAYER_NUM]; // trueならパッド使用


		//ホイールの状態を戻す
		int GetWheelDelta()
		{
			return delta;
		}

		//ホイールの状態を戻す
		WHEEL GetWheelRoll()
		{
			if (delta < 0) {
				return MWHEEL_UP;
			}
			else if (delta > 0) {
				return MWHEEL_DOWN;
			}
			return MWHEEL_NEUTRAL;
		}

		//デルタ値のクリア
		void ClearWheelDelta()
		{
			delta = 0;
		}

		bool IsUsePad(int playerID)
		{
			if (playerID < 0 || playerID >= system::PLAYER_NUM) assert(0);
			return usePad[playerID];
		}

		static void SetKeysInfo()
		{
			for (int i = 0; i < system::PLAYER_NUM; i++)
			{
				//パッド優先
				if (i < padcount) {
					usePad[i] = true;
					memcpy(g_key[i], padbase, sizeof(KEYINFO)* KEY_MAX);
				}
				else {
					usePad[i] = false;
					memcpy(g_key[i], keybase, sizeof(KEYINFO)* KEY_MAX);
				}
			}
		}

		void InitPlayerInput()
		{
			if (system::KeyboardMode == 0) {
				//パッドカウント
				for (int i = 0; i < system::PLAYER_NUM; i++) {
					JOYINFO		JoyInfoTemp;
					if (joyGetPos(i, &JoyInfoTemp) == JOYERR_NOERROR) {
						padcount++;
					}
				}
			}
			else {	//1時は強制的に0個にする。
				padcount = 0;
			}
			//メモリ確保
			JoyInfo = (JOYINFO*)malloc(sizeof(JOYINFO)*padcount);
			//パッド生成
			SetKeysInfo();
		}

		//--------------------------------------------------------------------------------------------
		//	パッドデータ消去
		//--------------------------------------------------------------------------------------------
		void DeletePlayerInput()
		{
			free(JoyInfo);
		}

		//--------------------------------------------------------------------------------------------
		//	パッドデータ取得
		//--------------------------------------------------------------------------------------------
		static BOOL JoyGet(int JoyID)
		{
			//ジョイスティック状態取得
			if (joyGetPos(JoyID, &JoyInfo[JoyID]) != JOYERR_NOERROR) {
				return 0;
			}
			return 1;
		}

		//-------------------------------------------------------------//
		//　ジョイスティックのボタン押下状態判定
		//		引数：	Joy　ジョイスティックID　JOY１　JOY2のどちらか
		//				Code ボタン番号　BUTTON1　から順に２，３，４
		//				Flag　状態を受け取るポインタ変数
		//-------------------------------------------------------------//
		static void JoyPressButton(int Joy, UINT Code, int *Flag)
		{
			if ((JoyInfo[Joy].wButtons	&	Code)) {
				*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
			}
			else {
				*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
			}
		}

		//-------------------------------------------------------------//
		//　ジョイスティックの十字キー押下状態判定
		//		引数：	Joy　ジョイスティックID　JOY１　JOY2のどちらか
		//				Direct 方向　JOY_UP,JOY_DOWN,JOY_LEFT,JOY_RIGHTのどれか
		//				Flag　状態を受け取るポインタ変数
		//-------------------------------------------------------------//
		static void JoyPressXY(int Joy, int Direct, int *Flag)
		{
			switch (Direct) {
			case JOY_DOWN:
				if (JoyInfo[Joy].wYpos > 40000) {
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else {
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_UP:
				if (JoyInfo[Joy].wYpos < 2000) {
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else {
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_RIGHT:
				if (JoyInfo[Joy].wXpos > 40000) {
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else {
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;

			case JOY_LEFT:
				if (JoyInfo[Joy].wXpos < 2000) {
					*Flag = (*Flag == FREE_KEY) ? PUSH_KEY : HOLD_KEY;
				}
				else {
					*Flag = (*Flag == PUSH_KEY || *Flag == HOLD_KEY) ? PULL_KEY : FREE_KEY;
				}
				break;
			}
		}

		//--------------------------------------------------------------------------------------------
		//押されたタイミングをチェックします
		//--------------------------------------------------------------------------------------------
		static bool IsHoldTiming(int now, int prev)
		{
			return now == PUSH_KEY ||					//今押された　もしくは
				(prev != PUSH_KEY && prev != now);	//1フレーム前に押されてなく、1フレーム前とステータスが違うとき
		}

		//--------------------------------------------------------------------------------------------
		//はなされたタイミングをチェックします
		//--------------------------------------------------------------------------------------------
		static bool IsPullTiming(int now, int prev)
		{
			return now == PULL_KEY ||					//今はなされた　もしくは
				(prev != PULL_KEY && prev != now);	//1フレーム前にはなされてなく、1フレーム前とステータスが違うとき
		}

		//--------------------------------------------------------------------------------------------
		//フレーム開始時に呼び、ゲームで使うキーの状態を調べます
		//毎フレーム必ず呼ばなければなりません
		//--------------------------------------------------------------------------------------------
		void CheckKey()
		{
			int i, prevStatus;

			for (int playernum = 0; playernum < system::PLAYER_NUM; playernum++)
			{
				if (playernum < padcount) {
					for (i = 0; i < KEY_MAX; i++)
					{
						//パッドでもマウスのボタンは認識させる
						if (i < KEY_BTNMAX) {
							//フレームカウンタチェック
							if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
							else						g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

							//前回のキーを保持し、次に押されたキーと判別する
							prevStatus = g_key[playernum][i].status;
							JoyGet(playernum);
							if (g_key[playernum][i].code == JOY_DOWN || g_key[playernum][i].code == JOY_UP ||
								g_key[playernum][i].code == JOY_RIGHT || g_key[playernum][i].code == JOY_LEFT) {
								JoyPressXY(playernum, g_key[playernum][i].code, &g_key[playernum][i].status);
							}
							else {
								JoyPressButton(playernum, g_key[playernum][i].code, &g_key[playernum][i].status);
							}
							//押されたタイミングとはなされたタイミングでカウンタを0クリアする
							if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
							else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
						}
						else {
							//フレームカウンタチェック
							if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
							else						g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

							//前回のキーを保持し、次に押されたキーと判別する
							prevStatus = g_key[playernum][i].status;
							Key_GetKey(g_key[playernum][i].code, &g_key[playernum][i].status);

							//押されたタイミングとはなされたタイミングでカウンタを0クリアする
							if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
							else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
						}
					}
				}
				else {
					for (i = 0; i < KEY_MAX; i++)
					{
						//フレームカウンタチェック
						if (CheckPress(i, 1, playernum)) g_key[playernum][i].frameHold++;	//押されているフレームをカウント
						else			  g_key[playernum][i].frameFree++;	//はなされているフレームをカウント

						//前回のキーを保持し、次に押されたキーと判別する
						prevStatus = g_key[playernum][i].status;
						Key_GetKey(g_key[playernum][i].code, &g_key[playernum][i].status);

						//押されたタイミングとはなされたタイミングでカウンタを0クリアする
						if (IsHoldTiming(g_key[playernum][i].status, prevStatus))		g_key[playernum][i].frameHold = 0;	//ステータスが違うのでフレームを0に戻す
						else if (IsPullTiming(g_key[playernum][i].status, prevStatus))	g_key[playernum][i].frameFree = 0;	//ステータスが違うのでフレームを0に戻す
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		//キーが「おされている」かをチェックします
		//intervalで与えたフレーム単位でtrueがかえってきます
		//【例】
		//intervalが1なら毎フレームtrue
		//intervalが2なら2フレームに一回true
		//intervalが5なら5フレームに一回true
		//--------------------------------------------------------------------------------------------
		bool CheckPress(int keyno, int interval, int playernum)
		{
			interval = interval > 0 ? interval : 1;	//0除算対策

			//キーステータスが「おされた」「おされている」のどちらかで
			//キーが押されたフレームが希望設定と一致しているか？
			if ((g_key[playernum][keyno].status == HOLD_KEY ||
				g_key[playernum][keyno].status == PUSH_KEY) &&
				g_key[playernum][keyno].frameHold % interval == 0)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//いずれかのデバイスのキーが押されているかをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckAnyPress(int keyno)
		{
			for (int i = 0; i < system::PLAYER_NUM; ++i)
				if (CheckPress(keyno, 1, i)) return true;

			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーが「はなされている」かをチェックします
		//intervalで与えたフレーム単位でtrueがかえってきます
		//【例】
		//intervalが1なら毎フレームtrue
		//intervalが2なら2フレームに一回true
		//intervalが5なら5フレームに一回true
		//--------------------------------------------------------------------------------------------
		bool CheckFree(int keyno, int interval, int playernum)
		{
			interval = interval > 0 ? interval : 1;	//0除算対策

			//キーステータスが「はなされた」「はなされている」のどちらかで
			//キーが押されたフレームが希望設定と一致しているか？
			if ((g_key[playernum][keyno].status == FREE_KEY ||
				g_key[playernum][keyno].status == PULL_KEY) &&
				g_key[playernum][keyno].frameFree % interval == 0)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーがこのフレームで「おされた」かをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckPush(int keyno, int playernum)
		{
			if (g_key[playernum][keyno].status == PUSH_KEY)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//いずれかのデバイスのキーが押されているかをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckAnyPush(int keyno)
		{
			for (int i = 0; i < system::PLAYER_NUM; ++i)
				if (CheckPush(keyno, i)) return true;

			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーがこのフレームで「はなされた」かをチェックします
		//--------------------------------------------------------------------------------------------
		bool CheckPull(int keyno, int playernum)
		{
			if (g_key[playernum][keyno].status == PULL_KEY)
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------
		//キーのステータスを返します
		//--------------------------------------------------------------------------------------------
		int GetKeyStatus(int keyno, int playernum)
		{
			return g_key[playernum][keyno].status;
		}

		//--------------------------------------------------------------------------------------------
		//キーのフレームを返します
		//--------------------------------------------------------------------------------------------
		int GetKeyFrameHold(int keyno, int playernum)
		{
			return g_key[playernum][keyno].frameHold;
		}

		int GetKeyFrameFree(int keyno, int playernum)
		{
			return g_key[playernum][keyno].frameFree;
		}

		//--------------------------------------------------------------------------------------------
		//キーのフレームを初期化します
		//--------------------------------------------------------------------------------------------
		void InitKeyInfo(int playernum)
		{
			for (int i = 0; i < KEY_MAX; i++)
			{
				g_key[playernum][i].status = FREE_KEY;
				g_key[playernum][i].frameHold = 0;
				g_key[playernum][i].frameFree = 0;
			}
		}

		//--------------------------------------------------------------------------------------------
		//キーコンフィグ
		//--------------------------------------------------------------------------------------------
		void SetKeyFormat(CODE_KEY key, DWORD keycode, int playernum)
		{
			g_key[playernum][key].code = keycode;
			g_key[playernum][key].status = FREE_KEY;
			g_key[playernum][key].frameHold = 0;
			g_key[playernum][key].frameFree = 0;
		}

		//--------------------------------------------------------------------------------------------
		//キーコンフィグ
		//--------------------------------------------------------------------------------------------
		void SetKeysFormat(DWORD* keycode, int playernum)
		{
			for (int i = 0; i < KEY_MAX; ++i)
			{
				g_key[playernum][i].code = keycode[i];
				g_key[playernum][i].status = FREE_KEY;
				g_key[playernum][i].frameHold = 0;
				g_key[playernum][i].frameFree = 0;
			}
		}

		//---------------------------------------------------------------------------------------
		//マウスカーソルの座標を取得(戻り値)
		//---------------------------------------------------------------------------------------
		POINT GetMousePosition()
		{
			POINT pt;

			GetCursorPos(&pt);
			ScreenToClient(system::hWnd, &pt);
			return pt;
		}

		//---------------------------------------------------------------------------------------
		//マウスカーソルの座標を取得(引数)
		//---------------------------------------------------------------------------------------
		void GetMousePosition(POINT* pt_ptr)
		{
			*pt_ptr = GetMousePosition();
		}

		//---------------------------------------------------------------------------------------
		//キー状態取得
		//Code　取得したいキーの仮想キーコード、ＡＳＣＩＩコード
		//Flag　キーの状態を取得したいバッファへのポインタ
		//---------------------------------------------------------------------------------------
		void Key_GetKey(unsigned int Code, int* Flag)
		{
			if ((GetKeyState(Code) & 0x80) == 0x80) {
				if (*Flag == FREE_KEY) {
					*Flag = PUSH_KEY;
				}
				else {
					*Flag = HOLD_KEY;
				}
			}
			else {
				if (*Flag == PUSH_KEY || *Flag == HOLD_KEY) {
					*Flag = PULL_KEY;
				}
				else {
					*Flag = FREE_KEY;
				}
			}
		}

	}


	//********************************************************************//
	//
	//				描画　関連関数
	//
	//********************************************************************//
	namespace draw {
		//=================================
		//ビューポートのデータを定義
		//=================================
		void SetViewport(RECT rt, float minZ, float maxZ) {
			D3DVIEWPORT9 viewData = { (DWORD)rt.left, (DWORD)rt.top,
				(DWORD)(rt.right - rt.left),
				(DWORD)(rt.bottom - rt.top),
				minZ, maxZ };
			//左上
			pD3DDevice->SetViewport(&viewData);
			pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(55, 0, 100, 255), 1.0f, 0);
		}
		//---------------------------------------------------------------------------------------
			//　半透明描画時に再描画を行う
			//---------------------------------------------------------------------------------------
		static void ReDraw()
		{
			Refresh2D();
			Clear2D();
			//pSprite->End();
			//pD3DDevice->EndScene();
			//pD3DDevice->BeginScene();
			//pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		}

		static void ReDrawCheck(const D3DCOLOR color)
		{
			//if ((color & 0xff000000) != 0xff000000) ReDraw();
			ReDraw();
		}

		//--------------------------------------------------------------------------------------------
		//画像データサイズ取得
		//--------------------------------------------------------------------------------------------
		D3DSURFACE_DESC GetImageSize(const std::string& resname)
		{
			D3DSURFACE_DESC TexData;
			ImgTable.at(resname).tex->GetLevelDesc(0, &TexData);
			return TexData;
		}

		unsigned int GetImageWidth(const std::string& resname)
		{
			return ImgTable.at(resname).info.Width;
		}

		unsigned int GetImageHeight(const std::string& resname)
		{
			return ImgTable.at(resname).info.Height;
		}

		SIZE GetImageSize2(const std::string& resname)
		{
			SIZE sz = { (LONG)ImgTable.at(resname).info.Width, (LONG)ImgTable.at(resname).info.Height };
			return sz;
		}

		//-------------------------------------------------------------//
		//特殊合成開始
		//-------------------------------------------------------------//
		//使用例
		//	Draw_SetRenderMode(ADD);
		//	描画
		//	Draw_EndRenderMode();
		//-------------------------------------------------------------//
		void SetRenderMode(int Mode)
		{
			if (Mode < 0 || Mode >= BF_MAX) return;

			//Clear時に一度Beginやっているので一回終わらせる
			pSprite->End();
			pSprite->Begin(D3DXSPRITE_ALPHABLEND);

			//アルファブレンディングの有効化はInitDx()へ移動
			//	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);

			DWORD flag[6][3] =
			{
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_ONE },
				{ D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_INVSRCALPHA },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_SRCCOLOR },
				{ D3DBLENDOP_REVSUBTRACT, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCCOLOR },
				{ D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCCOLOR }
			};

			pD3DDevice->SetRenderState(D3DRS_BLENDOP, flag[Mode][0]);
			pD3DDevice->SetRenderState(D3DRS_SRCBLEND, flag[Mode][1]);  //SRCの設定
			pD3DDevice->SetRenderState(D3DRS_DESTBLEND, flag[Mode][2]);//DESTの設定
		}


		//-------------------------------------------------------------//
		//特殊効果終了
		//-------------------------------------------------------------//
		//使用例
		//	Draw_SetRenderMode参照
		//-------------------------------------------------------------//
		void EndRenderMode()
		{
			SetRenderMode(NORMAL);
		}



		//---------------------------------------------------------------------------------------
		// ImgTableを初期化する。
		//---------------------------------------------------------------------------------------
		void InitImgTable()
		{
			ImgTable.clear();
		}

		//---------------------------------------------------------------------------------------
		// ImgTableを破棄する。
		//---------------------------------------------------------------------------------------
		void DelImgTable()
		{
			for (auto& img : ImgTable)
			{
				RELEASE(img.second.tex);
			}
		}

		//-------------------------------------------------------------//
		//　ポリゴン描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//-------------------------------------------------------------//
		static void PolygonNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color)
		{
			struct CUSTOMVERTEX {
				FLOAT x, y, z;
				FLOAT rhw;
				DWORD color;
			};

			CUSTOMVERTEX v[4] = {
				{ static_cast<float>(endX), static_cast<float>(startY), z, 1.0f, color },
				{ static_cast<float>(endX), static_cast<float>(endY), z, 1.0f, color },
				{ static_cast<float>(startX), static_cast<float>(startY), z, 1.0f, color },
				{ static_cast<float>(startX), static_cast<float>(endY), z, 1.0f, color }
			};
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(CUSTOMVERTEX));
		}

		///*
		//2016 2/25 渡加敷
		void Polygon(
			const std::vector<CUSTOMVERTEX> &poly, const std::string& resname, DWORD blend_color)
		{
			if (ImgTable.count(resname) == 0)
			{
				pD3DDevice->SetTexture(0, NULL);
			}
			else {
				pD3DDevice->SetTexture(0, ImgTable[resname].tex);
			}

			DWORD data[4];
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &data[0]);
			pD3DDevice->GetTextureStageState(0, D3DTSS_ALPHAOP, &data[1]);
			pD3DDevice->GetTextureStageState(0, D3DTSS_ALPHAARG1, &data[2]);
			pD3DDevice->GetTextureStageState(0, D3DTSS_ALPHAARG2, &data[3]);

			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, blend_color);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			pD3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

			UINT size = poly.size() - 2;
			if (size == 0)
				size = 1;
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, size, poly.data(), sizeof(CUSTOMVERTEX));

			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, data[0]);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, data[1]);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, data[2]);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, data[3]);
		}
		//*/

		//-------------------------------------------------------------//
		//矩形描画
		//	引数
		//		left,top			矩形の開始座標
		//		right,bottom			矩形の幅高さ
		//		fillColor			塗り色
		//		frameColor			枠色
		//		lineSize			ライン幅
		//		nakanuri		塗りつぶすか？
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void BoxNC(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			ReDrawCheck(fillColor);
			if (nakanuri)
			{
				PolygonNC(left, top, right, bottom, z, fillColor);
			}

			POINT vec[] =
			{
				{ left, top },
				{ left, bottom },
				{ right, bottom },	// ライン
				{ right, top },	// ライン
				{ left, top },	// ラインの終点
			};
			ReDrawCheck(frameColor);
			for (int i = 0; i < 4; i++)
			{
				LineNC(
					vec[i].x, vec[i].y,
					vec[i + 1].x, vec[i + 1].y,
					z, frameColor, lineSize);
			}
		}
		void BoxCenterNC(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			int sx = sizeX / 2,
				sy = sizeY / 2;
			BoxNC(x - sx, y - sy, x + sx, y + sy, z,
				fillColor, frameColor, lineSize, nakanuri);
		}
		//-------------------------------------------------------------//
		//直線描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color2			色
		//		Size			ライン幅
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void LineNC(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size)
		{
			struct CUSTOMVERTEX {
				FLOAT x, y, z;
				FLOAT rhw;
				DWORD color;
			};

			D3DCOLOR c = color;
			for (int i = 0; i < size; i++) {
				int tx, ty;
				if (abs(startX - endX) > abs(startY - endY)) {
					tx = 0;
					ty = i - size / 2;
				}
				else {
					tx = i - size / 2;
					ty = 0;
				}
				CUSTOMVERTEX v[2] = {
					{ static_cast<float>(startX + tx), static_cast<float>(startY + ty), z, 1.0f, c },
					{ static_cast<float>(endX + tx), static_cast<float>(endY + ty), z, 1.0f, c },
				};
				pD3DDevice->SetTexture(0, NULL);
				pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
				pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(CUSTOMVERTEX));
			}
		}

		//-------------------------------------------------------------//
		//RECTを表示する。
		//-------------------------------------------------------------//
		void		CkRectNC(const RECT& rt, D3DCOLOR color)
		{
			BoxNC(rt.left, rt.top, rt.right, rt.bottom,
				0.0f, ARGB(254, 0, 0, 0), color, 1, 0);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応　指定した座標の中心に描画
		//	引数
		//		X1,Y1			表示中心開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graph(100,100,1.0f,TestImg,0,0,100,100,
		//				90,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void GraphNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			//描画矩形計算
			RECT rt =
			{
				srcX,
				srcY,
				srcX + sizeX,
				srcY + sizeY
			};

			D3DXVECTOR2 centerPos(static_cast<float>(sizeX) / 2.0f, static_cast<float>(sizeY) / 2.0f);

			//	if(a != 255)DD_ReDraw();
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 ptCenter(centerPos.x, centerPos.y, 0.0f);	//	描画の基準値の設定
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXVECTOR2 draw(static_cast<float>(x), static_cast<float>(y));	//	描画先座標（演算前）
			D3DXVECTOR2 scale(scaleX, scaleY);	//	スケーリング係数（倍率を指定）

			D3DXVECTOR2	 sCenter;
			sCenter = D3DXVECTOR2(0, 0);

			D3DXMatrixTransformation2D(
				&matDraw,
				&sCenter,
				0.0f,
				&scale,
				NULL,
				D3DXToRadian(-degree),
				&draw);
			pSprite->SetTransform(&matDraw);
			//描画
			pSprite->Draw(
				ImgTable.at(resname).tex,
				&rt,
				&ptCenter,
				&position,
				D3DCOLOR_ARGB(a, r, g, b));
			//ワールドのリセット
			//	D3DXMatrixRotationZ(&matDraw,0.0f);
			//	pSprite->SetTransform(&matDraw);
		}
		void GraphNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			GraphNC(
				static_cast<int>(x),
				static_cast<int>(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, scaleX, scaleY,
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応
		//	引数
		//		X1,Y1			表示開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		*pCenter		回転の中心
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graph(100,100,1.0f,TestImg,0,0,100,100,
		//				90,&pt,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void GraphLeftTopNC(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			//描画矩形計算
			RECT rt =
			{
				srcX,
				srcY,
				srcX + sizeX,
				srcY + sizeY
			};

			D3DXVECTOR2 centerPos;
			if (center == nullptr)
			{
				centerPos = D3DXVECTOR2(0, 0);
			}
			else
			{
				centerPos = D3DXVECTOR2(
					static_cast<float>(center->x),
					static_cast<float>(center->y));
			}
			//	if(a != 255)DD_ReDraw();

			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 ptCenter(0, 0, 0.0f);	//	描画の基準値の設定
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXVECTOR2 draw(static_cast<float>(x), static_cast<float>(y));	//	描画先座標（演算前）
			D3DXVECTOR2 scale(scaleX, scaleY);	//	スケーリング係数（倍率を指定）

			D3DXMatrixTransformation2D(
				&matDraw,
				NULL,
				0.0f,
				&scale,
				&centerPos,
				D3DXToRadian(-degree),
				&draw);
			pSprite->SetTransform(&matDraw);
			//描画
			pSprite->Draw(
				ImgTable.at(resname).tex,
				&rt,
				&ptCenter,
				&position,
				D3DCOLOR_ARGB(a, r, g, b));
			//ワールドのリセット
			//	D3DXMatrixRotationZ(&matDraw,0.0f);
			//	pSprite->SetTransform(&matDraw);
		}
		void GraphLeftTopNC(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			GraphLeftTopNC(
				static_cast<int>(x),
				static_cast<int>(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center, scaleX, scaleY,
				a, r, g, b);
		}
		//-------------------------------------------------------------//
		// ポリゴン描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//-------------------------------------------------------------//
		static void Polygon2D(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color)
		{
			int id = camera::GetCameraId();
			PolygonNC(
				camera::cameras[id]->ixchange(startX), camera::cameras[id]->iychange(startY),
				camera::cameras[id]->ixchange(endX), camera::cameras[id]->iychange(endY),
				z, color);
		}

		//-------------------------------------------------------------//
		//矩形描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color1			塗り色
		//		Color2			枠色
		//		Size			ライン幅
		//		Nakanuri		塗りつぶすか？
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Box(
			int left, int top, int right, int bottom,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			int id = camera::GetCameraId();
			BoxNC(
				camera::cameras[id]->ixchange(left), camera::cameras[id]->iychange(top),
				camera::cameras[id]->ixchange(right), camera::cameras[id]->iychange(bottom),
				z, fillColor, frameColor,
				lineSize, nakanuri);
		}
		void BoxCenter(
			int x, int y, int sizeX, int sizeY,
			float z,
			D3DCOLOR fillColor, D3DCOLOR frameColor,
			int lineSize, bool nakanuri)
		{
			int sx = sizeX / 2,
				sy = sizeY / 2;
			int id = camera::GetCameraId();
			BoxNC(
				camera::cameras[id]->ixchange(x) - sx, camera::cameras[id]->iychange(y) - sy,
				camera::cameras[id]->ixchange(x) + sx, camera::cameras[id]->iychange(y) + sy,
				z,
				fillColor, frameColor,
				lineSize, nakanuri);
		}

		//-------------------------------------------------------------//
		//直線描画
		//	引数
		//		X1,Y1			矩形の開始座標
		//		X2,Y2			矩形の幅高さ
		//		Color2			色
		//		Size			ライン幅
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//-------------------------------------------------------------//
		void Line(
			int startX, int startY,
			int endX, int endY,
			float z, D3DCOLOR color, int size)
		{
			int id = camera::GetCameraId();
			LineNC(
				camera::cameras[id]->ixchange(startX), camera::cameras[id]->iychange(startY),
				camera::cameras[id]->ixchange(endX), camera::cameras[id]->iychange(endY),
				z, color, size);
		}

		//-------------------------------------------------------------//
		//RECTを表示する。
		//-------------------------------------------------------------//
		void		CkRect(const RECT& rt, D3DCOLOR color)
		{
			RECT rc = rt;

			Box(rc.left, rc.top, rc.right, rc.bottom,
				0.0f, ARGB(254, 0, 0, 0), color, 1, 0);
		}
		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応　指定した座標の中心に描画
		//	引数
		//		X1,Y1			表示中心開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graph(100,100,1.0f,TestImg,0,0,100,100,
		//				90,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void Graph(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			int id = camera::GetCameraId();
			GraphNC(
				camera::cameras[id]->ixchange(x), camera::cameras[id]->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree,
				camera::cameras[id]->changeScale(scaleX),
				camera::cameras[id]->changeScale(scaleY),
				a, r, g, b);
		}
		void Graph(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			int id = camera::GetCameraId();
			GraphNC(
				camera::cameras[id]->ixchange(x), camera::cameras[id]->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree,
				camera::cameras[id]->changeScale(scaleX),
				camera::cameras[id]->changeScale(scaleY),
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//IMGOBJを表示する。回転、拡大、色変更対応
		//	引数
		//		X1,Y1			表示開始座標
		//		z				表示Z軸座標0.0 から1.0の間で指定　0.0に近いほど手前になる
		//		IMGOBJ			描画したいIMGNO
		//		TempX1,TempY1	IMGNOからの参照座標
		//		TempX2,TempY2	TempX1,TempY1からの幅、高さ
		//		deg				回転角度
		//		*pCenter		回転の中心
		//		ScaleX			X軸の倍率 1.0が等倍
		//		ScaleY			X軸の倍率 1.0が等倍
		//		a				αチャンネル
		//		r				赤の強さ
		//		g				緑の強さ
		//		b				青の強さ
		//---------------------------------------------------------------------------------------
		//Draw_Graph(100,100,1.0f,TestImg,0,0,100,100,
		//				90,&pt,1.0f,1.0f,255,255,255,255);
		//---------------------------------------------------------------------------------------
		void GraphLeftTop(
			int x, int y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			int id = camera::GetCameraId();
			GraphLeftTopNC(
				camera::cameras[id]->ixchange(x), camera::cameras[id]->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center,
				camera::cameras[id]->changeScale(scaleX),
				camera::cameras[id]->changeScale(scaleY),
				a, r, g, b);
		}
		void GraphLeftTop(
			float x, float y, float z,
			const std::string& resname,
			int srcX, int srcY, int sizeX, int sizeY,
			float	degree, POINT *center,
			float	scaleX, float scaleY,
			u_char a, u_char r, u_char g, u_char b)
		{
			int id = camera::GetCameraId();
			GraphLeftTopNC(
				camera::cameras[id]->ixchange(x), camera::cameras[id]->iychange(y),
				z, resname,
				srcX, srcY, sizeX, sizeY,
				degree, center,
				camera::cameras[id]->changeScale(scaleX),
				camera::cameras[id]->changeScale(scaleY),
				a, r, g, b);
		}

		//---------------------------------------------------------------------------------------
		//画像データを読み込む
		//---------------------------------------------------------------------------------------
		//戻り値　
		//　読み込まれたファイルを扱うIMGNO(int)
		//
		//string resname	//登録するリソース名
		//string filename	//読み込むファイル名
		//int Transparent	//透過色の設定
		//
		//---------------------------------------------------------------------------------------
		void LoadObject(const std::string& resname, const std::string& filename, D3DCOLOR Transparent)
		{
			//同じリソース名が指定された場合
			if (ImgTable.count(resname) != 0) {
				//assert(!"同じ名前のリソースが登録済みです。名前の確認をしてください。");
				return;
			}

			ImgObj img;
			//読み込み開始
			if (FAILED(D3DXCreateTextureFromFileEx(
				pD3DDevice,
				filename.c_str(),	//パス
				D3DX_DEFAULT,	    //幅
				D3DX_DEFAULT,		//高さ
				D3DX_DEFAULT,		//ミップマップ
				0,					//レンダリングターゲットとしてつかうかどうか
				D3DFMT_UNKNOWN,		//テクスチャファイルフォーマット
				D3DPOOL_MANAGED,	//メモリの取り方
				D3DX_FILTER_NONE,	//フィルタリング方法（拡大縮小の演算方法）
				D3DX_DEFAULT,		//mip時のフィルタリング方法
				Transparent,		//抜け色（0指定で抜け色無効）
				&img.info,			//読み込んだ画像の情報
				nullptr,			//パレットへのポインタ（使う際に面倒なので、無効）
				&img.tex)			//テクスチャオブジェクトへのポインタ
				))
			{
				debug::BToMR("%sの読み込みに失敗しました", filename.c_str());
				assert(0);
				return;
			}
			img.filename = filename;
			ImgTable[resname] = img;
			//			ImgTable.insert(std::unordered_map< std::string, ImgObj&>::value_type(resname, img));
		}

		void DeleteObject(const std::string& resname)
		{
			ImgTable.erase(resname);
		}

		//2016-06-12 追加
		void DeleteAll()
		{
			DelImgTable();
			InitImgTable();
		}

		bool ExistObject(const std::string& resname)
		{
			return ImgTable.count(resname) != 0;
		}

		void Clear2D()
		{
			if (pSprite != NULL)
				pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		}

		void Clear3D()
		{
			//Scene Clear Black
			if (pD3DDevice != NULL)
				pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			//Draw Start
			if (pD3DDevice != NULL)
				pD3DDevice->BeginScene();
		}
		//##### Customized by S.Ueyama
		void Clear3D(int color)
		{
			//Scene Clear Black
			if (pD3DDevice != NULL)
				pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
			//Draw Start
			if (pD3DDevice != NULL)
				pD3DDevice->BeginScene();

		}
		//#####

		void Refresh2D()
		{
			if (pSprite != NULL)
				pSprite->End();
		}

		bool Refresh3D()
		{
			//Draw End
			if (pD3DDevice != NULL)
				pD3DDevice->EndScene();
			//ReDraw
			HRESULT hr;
			if (pD3DDevice != NULL)
				hr = pD3DDevice->Present(NULL, NULL, NULL, NULL);

			if (hr == D3DERR_DEVICELOST) {
				//状態チェック
				if (pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				{
					pSprite->OnLostDevice();
					//リセット
					hr = pD3DDevice->Reset(&D3DPP);
					if (hr != D3D_OK) {
						return false;
					}
					//再取得
					pSprite->OnResetDevice();
				}
			}
			return true;
		}

		//---------------------------------------------------------------------------------------
		//描画終了
		//---------------------------------------------------------------------------------------
		void Refresh()
		{
			Refresh2D();
			if (Refresh3D() == false)
			{
				return;
			}
		}
		//---------------------------------------------------------------------------------------
		//描画開始
		//---------------------------------------------------------------------------------------
		/*void Clear()
		{
			Clear3D();
			Clear2D();
		}*/
		//##### Customized by S.Ueyama
		void Clear(int color)
		{
			Clear3D(color);
			Clear2D();
		}
		//#####


//---------------------------------------------------------------------------------------
// Polygon
// ##### Customized by Sui
// Based on S.Ueyama / K.Koyanagi Source Code
//
// http://rudora7.blog81.fc2.com/blog-entry-422.html
// CreateImageFromFileEx
// http://ohwhsmm7.blog28.fc2.com/blog-entry-37.html
// FVF
// https://www.c3.club.kyutech.ac.jp/gamewiki/index.php?%C4%BA%C5%C0
//---------------------------------------------------------------------------------------
		void SetMtrl(int a, int r, int g, int b)
		{
			D3DLIGHT9 light = {};
			light.Type = D3DLIGHT_DIRECTIONAL;
			light.Diffuse.r = 1.0f;
			light.Diffuse.g = 1.0f;
			light.Diffuse.b = 1.0f;
			light.Diffuse.a = 1.0f;

			D3DXVECTOR3 v(0.0f, 1.0f, 0.0f);
			light.Range = 1000.0f;
			D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &v);
			pD3DDevice->SetLight(0, &light);											// ソケットに豆電球をON
			pD3DDevice->LightEnable(0, TRUE);											// ソケットに繋がっているスイッチON

			D3DMATERIAL9 mtrl = {};
			mtrl.Ambient.r = mtrl.Diffuse.r = r / 255.0f;
			mtrl.Ambient.g = mtrl.Diffuse.g = g / 255.0f;
			mtrl.Ambient.b = mtrl.Diffuse.b = b / 255.0f;
			mtrl.Ambient.a = mtrl.Diffuse.a = a / 255.0f;
			pD3DDevice->SetMaterial(&mtrl);

			//	pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00ffffff);// 世の中をちょっと白く照らす
			pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xff808080);// 世の中をちょっと白く照らす
		}

		struct VertexWC
		{
			float x, y, z, rhw;
			DWORD color;
			VertexWC() {}
			VertexWC(float x, float y, float z, float rhw, DWORD color = 0xFFFFFFFF)
				:
				x(x), y(y), z(z), rhw(rhw), color(color)
			{}
		};
		struct VertexWUV
		{
			float x, y, z, rhw;
			float u, v;
			VertexWUV() {}
			VertexWUV(float x, float y, float z, float rhw, float u, float v)
				:
				x(x), y(y), z(z), rhw(rhw), u(u), v(v)
			{}
		};
		struct VertexWUV2
		{
			float x, y, z, rhw;
			float u1, v1, u2, v2;
			VertexWUV2() {}
			VertexWUV2(float x, float y, float z, float rhw, float u1, float v1, float u2, float v2)
				:
				x(x), y(y), z(z), rhw(rhw), u1(u1), v1(v1), u2(u2), v2(v2)
			{}
		};

		inline std::vector<DWORD> backupTextureStageState(DWORD stage)
		{
			std::vector<DWORD> data(6);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_COLORARG1, &data[0]);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_COLOROP, &data[1]);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_COLORARG2, &data[2]);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_ALPHAARG1, &data[3]);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_ALPHAOP, &data[4]);
			pD3DDevice->GetTextureStageState(stage, D3DTSS_ALPHAARG2, &data[5]);
			return data;
		}

		inline void restoreTextureStageState(DWORD stage, const std::vector<DWORD>& data)
		{
			pD3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG1, data[0]);
			pD3DDevice->SetTextureStageState(stage, D3DTSS_COLOROP, data[1]);
			pD3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG2, data[2]);
			pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG1, data[3]);
			pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAOP, data[4]);
			pD3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG2, data[5]);
		}

		void PolygonPic(
			std::vector<VertexUV> vertices,
			const std::string& resname,
			float degree,
			const D3DCOLOR color
			)
		{
			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			if (degree != 0) {
				Point2f cen = math::CalcPolygonCenterPic(vertices);
				for (auto& v : vertices) {
					Point2f pos = math::RotateXY(v.x, v.y, cen.x, cen.y, degree);
					v.x = pos.x;
					v.y = pos.y;
				}
			}

			struct Itapoly3
			{
				/*
				//順番
				D3DFVF_XYZ:float	x, y, z;
				D3DFVF_XYZRHW:float	x, y, z, rhw;
				D3DFVF_NORMAL:float	nx, ny, nz;
				D3DFVF_DIFFUSE:DWORD	color;
				D3DFVF_SPECULAR:DWORD	specular;
				D3DFVF_TEX0 - 8    :float	u1, v1;
				//*/
				float x, y, z, rhw;
				float u, v;
			};
			std::vector<Itapoly3> pp;
			int id = camera::GetCameraId();
			for (const auto& v : vertices)
			{
				Itapoly3 tmp = { (float)camera::cameras[id]->ixchange(v.x), (float)camera::cameras[id]->iychange(v.y), v.z, 1.0f, v.u, v.v };
				pp.push_back(tmp);
			}

			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			//SetTextureStageState
			//ARG1 = 上
			//ARG2 = 下・ポリゴン・背景
			//http://sky.geocities.jp/kmaedam/directx9/texcol.htm
			//http://marupeke296.com/DXG_TextureStage.html
			//http://www.c3.club.kyutech.ac.jp/gamewiki/index.php?%A5%C6%A5%AF%A5%B9%A5%C1%A5%E3%A5%B9%A5%C6%A1%BC%A5%B8%A5%B9%A5%C6%A1%BC%A5%C8
			//http://d.hatena.ne.jp/sanoh/20071224/1198429419

			//SetRenderState D3DRS_BLENDOP
			//https://www.c3.club.kyutech.ac.jp/gamewiki/index.php?%A5%A2%A5%EB%A5%D5%A5%A1%A5%D6%A5%EC%A5%F3%A5%C7%A5%A3%A5%F3%A5%B0
			//http://www3.pf-x.net/~chopper/home2/DirectX/CDX16.html
			//http://labo.xo-ox.net/article/83005077.html

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data = backupTextureStageState(0);

			//RenderState設定
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			//D3DRS_TEXTUREFACTOR = D3DTA_TFACTOR テクスチャ ブレンディング引数または D3DTOP_BLENDFACTORALPHA テクスチャ ブレンディング処理による
			//						マルチテクスチャ ブレンディングで使われる色。関連する値は D3DCOLOR 変数である。デフォルト値は不透明の白(0xFFFFFFFF) である。

			//Stage	= テクスチャステージ
			//Type	= 「合成対象」や「合成演算」を決める部分で、D3DTEXTURESTAGESTATETYPE列挙型にその全てが定義されています。
			//			沢山あるので、詳しくは後述しますが、ここで「何の設定をしたいのか」を決めます。
			//Value	= Typeによって意味が変わってくる引数です。
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //テクスチャのカラー値
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //ARG1とARG2のカラー値を乗算してカラー値を取得
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR); //動的のカラー値（D3DRS_TEXTUREFACTORの値）
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); //テクスチャのカラー値
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); //ARG1とARG2のα値を乗算してα値を取得
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR); //動的のカラー値（D3DRS_TEXTUREFACTORの値）

			/*
			//Enabling transparency
			pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			//*/

			/*
			//Alpha設定のみ / カラーを設定できない
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA); //D3DRS_TEXTUREFACTORの値
			//*/

			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			//D3DFVF_TEX1 がテクスチャの枚数になります
			//D3DFVF_TEX0 ～ D3DFVF_TEX8 まであり D3DFVF_TEX1 だと当然１枚です
			//D3DFVF_TEX0 は０枚なのでなくても同じです
			//テクスチャのＵＶ値

			//描画
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, pp.size() - 2, pp.data(), sizeof(Itapoly3));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data);

			//Default = TEXTURE MOD DIFFUSE
			//pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			//pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			//pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			//pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			//pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			//pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		}

		void PolygonPicGradient(
			std::vector<VertexUV> vertices,
			const std::string& resname,
			float degree
			)
		{
			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			if (degree != 0) {
				Point2f cen = math::CalcPolygonCenterPic(vertices);
				for (auto& v : vertices) {
					Point2f pos = math::RotateXY(v.x, v.y, cen.x, cen.y, degree);
					v.x = pos.x;
					v.y = pos.y;
				}
			}

			struct Itapoly3
			{
				float x, y, z, rhw;
				DWORD color;
				float u, v;
			};
			std::vector<Itapoly3> pp;
			int id = camera::GetCameraId();
			for (const auto& v : vertices)
			{
				Itapoly3 tmp = { (float)camera::cameras[id]->ixchange(v.x), (float)camera::cameras[id]->iychange(v.y), v.z, 1.0f, v.color, v.u, v.v };
				pp.push_back(tmp);
			}

			DWORD color = 0xFFFFFFFF;
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);


			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE); //テクスチャのカラー値
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE); //ARG1とARG2のカラー値を乗算してカラー値を取得
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); //ポリゴンの色の値
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); //テクスチャのカラー値
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE); //ARG1とARG2のα値を乗算してα値を取得
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE); //ポリゴンの色の値

			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, pp.size() - 2, pp.data(), sizeof(Itapoly3));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
		}

		void Polygon(
			const std::vector<Vertex> vertices,
			D3DCOLOR color)
		{
			std::vector<VertexWC> pp;
			int id = camera::GetCameraId();
			for (const auto& v : vertices)
			{
				VertexWC tmp = { (float)camera::cameras[id]->ixchange(v.x), (float)camera::cameras[id]->iychange(v.y), v.z, 1.0f, color };
				pp.push_back(tmp);
			}

			pD3DDevice->SetTexture(0, NULL);

			//頂点フォーマットの内容を伝達する。
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
			//D3DFVF_XYZ		= 頂点座標
			//D3DFVF_XYZRHW		= 頂点座標+RHW
			//D3DFVF_DIFFUSE	= 頂点の色 / DIFFUSEは「ポリゴンの色」
			//D3DFVF_SPECULAR	= 鏡面反射色（スペキュラ色）


			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, pp.size() - 2, pp.data(), sizeof(VertexWC));
			//PrimitiveType				= プリミティブタイプ（頂点を結びつける順番を決めたりする）を指定する。
			//PrimitiveCount			= 三角ポリゴンの数。
			//*pVertexStreamZeroData	= 頂点データのポインタを渡す。
			//VertexStreamZeroStride	= 頂点データ間のストライド（1個のデータの幅）を指定。
		}

		void Polygon(
			POINT pos1, POINT pos2, POINT pos3,
			float z,
			D3DCOLOR color)
		{
			std::vector<Vertex> v =
			{
				Vertex((float)pos1.x, (float)pos1.y, z),
				Vertex((float)pos2.x, (float)pos2.y, z),
				Vertex((float)pos3.x, (float)pos3.y, z)
			};
			Polygon(v, color);
		}
		void Polygon(
			float x1, float y1, float x2, float y2, float x3, float y3,
			float z,
			D3DCOLOR color)
		{
			std::vector<Vertex> v =
			{
				Vertex(x1, y1, z),
				Vertex(x2, y2, z),
				Vertex(x3, y3, z)
			};
			Polygon(v, color);
		}

		//----- 各頂点の色でGradient画像を描画
		void PolygonGradient(const std::vector<Vertex> vertices)
		{
			struct CustomVertex
			{
				float x, y, z, rhw;
				DWORD color;
			};
			std::vector<VertexWC> pp;
			int id = camera::GetCameraId();
			for (const auto& v : vertices)
			{
				VertexWC tmp = { (float)camera::cameras[id]->ixchange(v.x), (float)camera::cameras[id]->iychange(v.y), v.z, 1.0f, v.color };
				pp.push_back(tmp);
			}

			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, pp.size() - 2, pp.data(), sizeof(VertexWC));
		}

		void PolygonGradient(
			POINT pos1, POINT pos2, POINT pos3,
			float z,
			D3DCOLOR color1, D3DCOLOR color2, D3DCOLOR color3)
		{
			std::vector<Vertex> v =
			{
				Vertex((float)pos1.x, (float)pos1.y, z, color1),
				Vertex((float)pos2.x, (float)pos2.y, z, color2),
				Vertex((float)pos3.x, (float)pos3.y, z, color3)
			};
			PolygonGradient(v);
		}

		void PolygonGradient(
			float x1, float y1, float x2, float y2, float x3, float y3,
			float z,
			D3DCOLOR color1, D3DCOLOR color2, D3DCOLOR color3)
		{
			std::vector<Vertex> v =
			{
				Vertex(x1, y1, z, color1),
				Vertex(x2, y2, z, color2),
				Vertex(x3, y3, z, color3)
			};
			PolygonGradient(v);
		}

		void Circle(float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color)
		{
			int id = camera::GetCameraId();
			CircleNC(camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, startAngle, stepAngle, counterClockWise, color);
		}

		void CircleNC(float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color)
		{
			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 360.0f / 100.0f * percent;

			pD3DDevice->SetTexture(0, NULL);
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

			std::vector<VertexWC> vertices;
			vertices.push_back(VertexWC(x, y, z, 1.0f, color));

			if (counterClockWise) {
				for (float i = startAngle; i <= maxAngle + startAngle; i += stepAngle) {
					vertices.push_back(VertexWC(
						(x + cosf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						(y - sinf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						z, 1.0f, color
						));
				}
			}
			else {
				for (float i = startAngle; i >= startAngle - maxAngle; i -= stepAngle) {
					vertices.push_back(VertexWC(
						(x + cosf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						(y - sinf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						z, 1.0f, color
						));
				}
			}
			//Draw
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWC));
			//Restore
			pD3DDevice->SetFVF(fvf);
		}

		void CircleGradient(float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise,
			D3DCOLOR innerColor, D3DCOLOR outerColor)
		{
			int id = camera::GetCameraId();
			CircleGradientNC(camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, startAngle, stepAngle, counterClockWise, innerColor, outerColor);
		}

		void CircleGradientNC(float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise,
			D3DCOLOR innerColor, D3DCOLOR outerColor)
		{
			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 360.0f / 100.0f * percent;

			pD3DDevice->SetTexture(0, NULL);
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

			std::vector<VertexWC> vertices;
			vertices.push_back(VertexWC(x, y, z, 1.0f, innerColor));

			if (counterClockWise) {
				for (float i = startAngle; i <= maxAngle + startAngle; i += stepAngle) {
					vertices.push_back(VertexWC(
						(x + cosf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						(y - sinf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						z, 1.0f, outerColor
						));
				}
			}
			else {
				for (float i = startAngle; i >= startAngle - maxAngle; i -= stepAngle) {
					vertices.push_back(VertexWC(
						(x + cosf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						(y - sinf(i * static_cast<float>(M_PI) / 180.0f) * radius),
						z, 1.0f, outerColor
						));
				}
			}
			//Draw
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWC));
			//Restore
			pD3DDevice->SetFVF(fvf);
		}

		void CirclePic(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color
			)
		{
			int id = camera::GetCameraId();
			CirclePicNC(resname, camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, startAngle, stepAngle, counterClockWise, color);
		}

		void CirclePicNC(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color
			)
		{
			if (percent <= 0.0f || percent > 100.0f) return;

			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 360.0f / 100.0f * percent;

			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			//Backup State
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data0 = backupTextureStageState(0);

			//Set
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			std::vector<VertexWUV> vertices;
			vertices.push_back(VertexWUV(x, y, z, 1.0f, 0.5f, 0.5f));

			if (counterClockWise) {
				for (float i = startAngle; i <= maxAngle + startAngle; i += stepAngle) {
					float co = cosf(i * static_cast<float>(M_PI) / 180.0f);
					float si = sinf(i * static_cast<float>(M_PI) / 180.0f);
					vertices.push_back(VertexWUV(
						(x + co * radius),
						(y - si * radius),
						z, 1.0f,
						0.5f + co * 0.5f,
						0.5f - si * 0.5f
						));
				}
			}
			else {
				for (float i = startAngle; i >= startAngle - maxAngle; i -= stepAngle) {
					float co = cosf(i * static_cast<float>(M_PI) / 180.0f);
					float si = sinf(i * static_cast<float>(M_PI) / 180.0f);
					vertices.push_back(VertexWUV(
						(x + co * radius),
						(y - si * radius),
						z, 1.0f,
						0.5f + co * 0.5f,
						0.5f - si * 0.5f
						));
				}
			}
			//Draw
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data0);
		}

		void CirclePicMod(const std::string& resname1, const std::string& resname2,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color
			)
		{
			int id = camera::GetCameraId();
			CirclePicModNC(resname1, resname2,
				camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, startAngle, stepAngle, counterClockWise, color);
		}

		void CirclePicModNC(const std::string& resname1, const std::string& resname2,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, bool counterClockWise, D3DCOLOR color
			)
		{
			if (percent <= 0.0f || percent > 100.0f) return;

			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 360.0f / 100.0f * percent;

			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname1].tex);
			pD3DDevice->SetTexture(1, ImgTable[resname2].tex);

			//Backup State
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data0 = backupTextureStageState(0);
			std::vector<DWORD> data1 = backupTextureStageState(1);

			//Set
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX2);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

			std::vector<VertexWUV2> vertices;
			vertices.push_back(VertexWUV2(x, y, z, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f));

			if (counterClockWise) {
				for (float i = startAngle; i <= maxAngle + startAngle; i += stepAngle) {
					float co = cosf(i * static_cast<float>(M_PI) / 180.0f);
					float si = sinf(i * static_cast<float>(M_PI) / 180.0f);
					float u = 0.5f + co * 0.5f;
					float v = 0.5f - si * 0.5f;
					vertices.push_back(VertexWUV2(
						(x + co * radius),
						(y - si * radius),
						z, 1.0f,
						u, v, u, v
						));
				}
			}
			else {
				for (float i = startAngle; i >= startAngle - maxAngle; i -= stepAngle) {
					float co = cosf(i * static_cast<float>(M_PI) / 180.0f);
					float si = sinf(i * static_cast<float>(M_PI) / 180.0f);
					float u = 0.5f + co * 0.5f;
					float v = 0.5f - si * 0.5f;
					vertices.push_back(VertexWUV2(
						(x + co * radius),
						(y - si * radius),
						z, 1.0f,
						u, v, u, v
						));
				}
			}
			//Draw
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV2));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetTexture(1, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data0);
			restoreTextureStageState(1, data1);
		}

		void CirclePicRectUp(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float stepAngle, float scalex, float scaley, bool counterClockWise, D3DCOLOR color
			)
		{
			int id = camera::GetCameraId();
			CirclePicRectUpNC(resname, camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, stepAngle, scalex, scaley, counterClockWise, color);
		}

		void CirclePicRectUpNC(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float stepAngle, float scalex, float scaley, bool counterClockWise, D3DCOLOR color
			)
		{
			//高速化のため、板ポリゴンの数を減りましょう
			if (counterClockWise)
				CirclePicRectUpCcwNC(resname, x, y, z, radius, percent, stepAngle, scalex, scaley, color);
			else
				CirclePicRectUpCwNC(resname, x, y, z, radius, percent, stepAngle, scalex, scaley, color);
		}

		void CirclePicRectUpCcwNC(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float stepAngle, float scalex, float scaley, D3DCOLOR color
			)
		{
			if (percent <= 0.0f || percent > 100.0f) return;

			float radiusx = radius * scalex;
			float radiusy = radius * scaley;

			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = (360.0f / 100.0f * percent) + 90.0f;

			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			//Backup State
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data0 = backupTextureStageState(0);

			//Set
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			std::vector<VertexWUV> vertices;

			VertexWUV centerIta(x, y, z, 1.0f, 0.5f, 0.5f);
			float uv1deg = 0.5f / 45.0f;
			float pos1degx = radiusx / 45.0f;
			float pos1degy = radiusy / 45.0f;

			//---------- Draw -----
			//Full
			if (percent == 100.0f) {
				vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
				vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
			}
			//Over Half
			else if (percent >= 50.0f) {
				//Draw Half
				{
					vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
					vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
					vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
					vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
					pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
					vertices.clear();
				}
				//75~100%
				if (percent >= 75.0f) {
					//25% of Circle
					{
						vertices.push_back(centerIta);
						vertices.push_back(VertexWUV(x + radiusx, y, z, 1.0f, 1.0f, 0.5f));
						vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
						vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
						pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
						vertices.clear();
					}
					//87.5~100%
					if (percent >= 87.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x + radiusx, y, z, 1.0f, 1.0f, 0.5f));
							vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 405; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x + (450.0f - i) * pos1degx;
								posy = y - radiusy;
								u = 0.5f + (450.0f - i) * uv1deg;
								v = 0.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//75~87.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 360; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x + radiusx;
								posy = y - (45.0f - (405.0f - i)) * pos1degy;
								u = 1.0f;
								v = 0.5f - (45.0f - (405.0f - i)) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
				//50~75%
				else {
					//62.5~75%
					if (percent >= 62.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
							vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 315; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x + radiusx;
								posy = y + (360.0f - i) * pos1degy;
								u = 1.0f;
								v = 0.5f + (360.0f - i) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//50~62.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 270; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x + (45.0f - (315.0f - i)) * pos1degx;
								posy = y + radiusy;
								u = 0.5f + (45.0f - (315.0f - i)) * uv1deg;
								v = 1.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}

				}
			}
			//Below Half
			else {
				//25~50%
				if (percent >= 25.0f) {
					//25% of Circle
					{
						vertices.push_back(centerIta);
						vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
						vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
						vertices.push_back(VertexWUV(x - radiusx, y, z, 1.0f, 0.0f, 0.5f));
						pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
						vertices.clear();
					}
					//37.5~50%
					if (percent >= 37.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x - radiusx, y, z, 1.0f, 0.0f, 0.5f));
							vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 225; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x - (270.0f - i) * pos1degx;
								posy = y + radiusy;
								u = 0.5f - (270.0f - i) * uv1deg;
								v = 1.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//25~37.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 180; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x - radiusx;
								posy = y + (45 - (225.0f - i)) * pos1degy;
								u = 0.0f;
								v = 0.5f + (45 - (225.0f - i)) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
				//0~25%
				else {
					//12.5~25%
					if (percent >= 12.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
							vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 135; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x - radiusx;
								posy = y - (180.0f - i) * pos1degy;
								u = 0.0f;
								v = 0.5f - (180.0f - i) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//0~12.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 90; i <= maxAngle; i += stepAngle) {
								float posx, posy, u, v;
								posx = x - (45 - (135.0f - i)) * pos1degx;
								posy = y - radiusy;
								u = 0.5f - (45 - (135.0f - i)) * uv1deg;
								v = 0.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
			}
			//Draw Remaining
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data0);
		}

		void CirclePicRectUpCwNC(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float stepAngle, float scalex, float scaley, D3DCOLOR color
			)
		{
			if (percent <= 0.0f || percent > 100.0f) return;

			float radiusx = radius * scalex;
			float radiusy = radius * scaley;

			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 90 - (360.0f / 100.0f * percent);

			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			//Backup State
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data0 = backupTextureStageState(0);

			//Set
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			std::vector<VertexWUV> vertices;

			VertexWUV centerIta(x, y, z, 1.0f, 0.5f, 0.5f);
			float uv1deg = 0.5f / 45.0f;
			float pos1degx = radiusx / 45.0f;
			float pos1degy = radiusy / 45.0f;

			//---------- Draw -----
			//Full
			if (percent == 100.0f) {
				vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
				vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
			}
			//Over Half
			else if (percent >= 50.0f) {
				//Draw Half
				{
					vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
					vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
					vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
					vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
					pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
					vertices.clear();
				}
				//75~100%
				if (percent >= 75.0f) {
					//25% of Circle
					{
						vertices.push_back(centerIta);
						vertices.push_back(VertexWUV(x - radiusx, y, z, 1.0f, 0.0f, 0.5f));
						vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
						vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
						pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
						vertices.clear();
					}
					//87.5~100%
					if (percent >= 87.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x - radiusx, y, z, 1.0f, 0.0f, 0.5f));
							vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = -225; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x - (270.0f + i) * pos1degx;
								posy = y - radiusy;
								u = 0.5f - (270.0f + i) * uv1deg;
								v = 0.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//75~87.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = -180; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x - radiusx;
								posy = y - (45.0f - (225.0f + i)) * pos1degy;
								u = 0.0f;
								v = 0.5f - (45.0f - (225.0f + i)) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
				//50~75%
				else {
					//62.5~75%
					if (percent >= 62.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x, y + radiusy, z, 1.0f, 0.5f, 1.0f));
							vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = -135; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x - radiusx;
								posy = y + (180.0f + i) * pos1degy;
								u = 0.0f;
								v = 0.5f + (180.0f + i) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//50~62.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = -90; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x - (45.0f - (135.0f + i)) * pos1degx;
								posy = y + radiusy;
								u = 0.5f - (45.0f - (135.0f + i)) * uv1deg;
								v = 1.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}

				}
			}
			//Below Half
			else {
				//25~50%
				if (percent >= 25.0f) {
					//25% of Circle
					{
						vertices.push_back(centerIta);
						vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
						vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
						vertices.push_back(VertexWUV(x + radiusx, y, z, 1.0f, 1.0f, 0.5f));
						pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
						vertices.clear();
					}
					//37.5~50%
					if (percent >= 37.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x + radiusx, y, z, 1.0f, 1.0f, 0.5f));
							vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = -45; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x + (90.0f + i) * pos1degx;
								posy = y + radiusy;
								u = 0.5f + (90.0f + i) * uv1deg;
								v = 1.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//25~37.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 0; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x + radiusx;
								posy = y + (45 - (45.0f + i)) * pos1degy;
								u = 1.0f;
								v = 0.5f + (45 - (45.0f + i)) * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
				//0~25%
				else {
					//12.5~25%
					if (percent >= 12.5f) {
						//12.5% of Circle
						{
							vertices.push_back(centerIta);
							vertices.push_back(VertexWUV(x, y - radiusy, z, 1.0f, 0.5f, 0.0f));
							vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
							pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));
							vertices.clear();
						}
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 45; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x + radiusx;
								posy = y - i * pos1degy;
								u = 1.0f;
								v = 0.5f - i * uv1deg;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
					//0~12.5%
					else {
						//Remaining
						{
							vertices.push_back(centerIta);
							for (float i = 90; i >= maxAngle; i -= stepAngle) {
								float posx, posy, u, v;
								posx = x + (45 - (i - 45.0f)) * pos1degx;
								posy = y - radiusy;
								u = 0.5f + (45 - (i - 45.0f)) * uv1deg;
								v = 0.0f;
								vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
							}
						}
					}
				}
			}
			//Draw Remaining
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data0);
		}

		void CirclePicRect(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, float scalex, float scaley, bool counterClockWise, D3DCOLOR color
			)
		{
			int id = camera::GetCameraId();
			CirclePicRectNC(resname, camera::cameras[id]->xchange(x), camera::cameras[id]->ychange(y),
				z, radius, percent, startAngle, stepAngle, scalex, scaley, counterClockWise, color);
		}

		void CirclePicRectNC(const std::string& resname,
			float x, float y, float z, float radius, float percent,
			float startAngle, float stepAngle, float scalex, float scaley, bool counterClockWise, D3DCOLOR color
			)
		{
			if (percent <= 0.0f || percent > 100.0f) return;

			float radiusx = radius * scalex;
			float radiusy = radius * scaley;

			if (stepAngle < 0.1f) stepAngle = 0.1f;
			float maxAngle = 360.0f / 100.0f * percent;

			// テクスチャをパイプラインにセット
			pD3DDevice->SetTexture(0, ImgTable[resname].tex);

			//Backup State
			DWORD fvf = 0;
			pD3DDevice->GetFVF(&fvf);
			auto a = ((color >> 24) & 0x00000000FF);
			auto r = ((color >> 16) & 0x00000000FF);
			auto g = ((color >> 8) & 0x00000000FF);
			auto b = (color & 0x00000000FF);
			SetMtrl(a, r, g, b);

			DWORD factor;
			pD3DDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &factor);
			std::vector<DWORD> data0 = backupTextureStageState(0);

			//Set
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, color);
			pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

			std::vector<VertexWUV> vertices;

			VertexWUV centerIta(x, y, z, 1.0f, 0.5f, 0.5f);
			float uv1deg = 0.5f / 45.0f;
			float pos1degx = radiusx / 45.0f;
			float pos1degy = radiusy / 45.0f;

			//---------- Draw -----
			//Full
			if (percent == 100.0f) {
				vertices.push_back(VertexWUV(x - radiusx, y - radiusy, z, 1.0f, 0.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y - radiusy, z, 1.0f, 1.0f, 0.0f));
				vertices.push_back(VertexWUV(x + radiusx, y + radiusy, z, 1.0f, 1.0f, 1.0f));
				vertices.push_back(VertexWUV(x - radiusx, y + radiusy, z, 1.0f, 0.0f, 1.0f));
			}
			//Not Full
			else {
				//CCW
				if (counterClockWise) {
					//Round to 0~360
					while (startAngle >= 360)
						startAngle -= 360;
					while (startAngle < 0)
						startAngle += 360;

					vertices.push_back(centerIta);
					for (float i = startAngle; i < maxAngle + startAngle; i += stepAngle) {
						float posx = 0, posy = 0, u = 0, v = 0;
						if (i <= 405) {
							if (i <= 45) {
								posx = x + radiusx;
								posy = y - (45.0f - (45.0f - i)) * pos1degy;
								u = 1.0f;
								v = 0.5f - (45.0f - (45.0f - i)) * uv1deg;
							}
							else if (i <= 135) {
								posx = x + ((135.0f - i) - 45.0f) * pos1degx;
								posy = y - radiusy;
								u = 0.5f + ((135.0f - i) - 45.0f) * uv1deg;
								v = 0.0f;
							}
							else if (i <= 225) {
								posx = x - radiusx;
								posy = y - ((225.0f - i) - 45.0f) * pos1degy;
								u = 0.0f;
								v = 0.5f - ((225.0f - i) - 45.0f) * uv1deg;
							}
							else if (i <= 315) {
								posx = x + (45.0f - (315.0f - i)) * pos1degx;
								posy = y + radiusx;
								u = 0.5f + (45.0f - (315.0f - i)) * uv1deg;
								v = 1.0f;
							}
							else {// if (i <= 405) {
								posx = x + radiusx;
								posy = y - (45.0f - (405.0f - i)) * pos1degy;
								u = 1.0f;
								v = 0.5f - (45.0f - (405.0f - i)) * uv1deg;
							}
						}
						else if (i <= 720) {
							if (i <= 495) {
								posx = x + ((495.0f - i) - 45.0f) * pos1degx;
								posy = y - radiusy;
								u = 0.5f + ((495.0f - i) - 45.0f) * uv1deg;
								v = 0.0f;
							}
							else if (i <= 585) {
								posx = x - radiusx;
								posy = y - ((585.0f - i) - 45.0f) * pos1degy;
								u = 0.0f;
								v = 0.5f - ((585.0f - i) - 45.0f) * uv1deg;
							}
							else if (i <= 675) {
								posx = x + (45.0f - (675.0f - i)) * pos1degx;
								posy = y + radiusx;
								u = 0.5f + (45.0f - (675.0f - i)) * uv1deg;
								v = 1.0f;
							}
							else {// if (i <= 720) {
								posx = x + radiusx;
								posy = y + (720.0f - i) * pos1degy;
								u = 1.0f;
								v = 0.5f + (720.0f - i) * uv1deg;
							}
						}
						else
							break;
						vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
					}
				}
				//CW
				else {
					//Round to 360~720
					while (startAngle >= 720)
						startAngle -= 360;
					while (startAngle < 360)
						startAngle += 360;

					vertices.push_back(centerIta);
					for (float i = startAngle; i > startAngle - maxAngle; i -= stepAngle) {
						float posx = 0, posy = 0, u = 0, v = 0;
						if (i <= 360) {
							if (i >= 315) {
								posx = x + radiusx;
								posy = y + (45.0f - (i - 315.0f)) * pos1degy;
								u = 1.0f;
								v = 0.5f + (45.0f - (i - 315.0f)) * uv1deg;
							}
							else if (i >= 225) {
								posx = x + ((i - 225.0f) - 45.0f) * pos1degx;
								posy = y + radiusy;
								u = 0.5f + ((i - 225.0f) - 45.0f) * uv1deg;
								v = 1.0f;
							}
							else if (i >= 135) {
								posx = x - radiusx;
								posy = y + ((i - 135.0f) - 45.0f) * pos1degy;
								u = 0.0f;
								v = 0.5f + ((i - 135.0f) - 45.0f) * uv1deg;
							}
							else if (i >= 45) {
								posx = x + (45.0f - (i - 45.0f)) * pos1degx;
								posy = y - radiusx;
								u = 0.5f + (45.0f - (i - 45.0f)) * uv1deg;
								v = 0.0f;
							}
							else if (i >= 0) {
								posx = x + radiusx;
								posy = y - i * pos1degy;
								u = 1.0f;
								v = 0.5f - i * uv1deg;
							}
						}
						else if (i <= 720) {
							if (i >= 675) {
								posx = x + radiusx;
								posy = y + (45.0f - (i - 675.0f)) * pos1degy;
								u = 1.0f;
								v = 0.5f + (45.0f - (i - 675.0f)) * uv1deg;
							}
							else if (i >= 585) {
								posx = x + ((i - 585.0f) - 45.0f) * pos1degx;
								posy = y + radiusy;
								u = 0.5f + ((i - 585.0f) - 45.0f) * uv1deg;
								v = 1.0f;
							}
							else if (i >= 495) {
								posx = x - radiusx;
								posy = y + ((i - 495.0f) - 45.0f) * pos1degy;
								u = 0.0f;
								v = 0.5f + ((i - 495.0f) - 45.0f) * uv1deg;
							}
							else if (i >= 405) {
								posx = x + (45.0f - (i - 405.0f)) * pos1degx;
								posy = y - radiusx;
								u = 0.5f + (45.0f - (i - 405.0f)) * uv1deg;
								v = 0.0f;
							}
							else if (i >= 315) {
								posx = x + radiusx;
								posy = y + (45.0f - (i - 315.0f)) * pos1degy;
								u = 1.0f;
								v = 0.5f + (45.0f - (i - 315.0f)) * uv1deg;
							}
						}
						else
							break;
						vertices.push_back(VertexWUV(posx, posy, z, 1.0f, u, v));
					}
				}
			}

			//Draw
			pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, vertices.size() - 2, vertices.data(), sizeof(VertexWUV));

			//Restore everything
			pD3DDevice->SetFVF(fvf);
			pD3DDevice->SetTexture(0, NULL);
			pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
			restoreTextureStageState(0, data0);
		}
		//---------------------------------------------------------------------------------------
	}


	//---------------------------------------------------------------------------------------
	// フォント関連
	//---------------------------------------------------------------------------------------
	namespace font {

		// FONT管理用配列
		const int DDFONTMAX = 10;
		LPD3DXFONT pFontData[DDFONTMAX];

		//---------------------------------------------------------------------------------------
		//フォントの作成
		//	Num			フォント番号
		//	size		フォントサイズ
		//	fontname	フォントの名前
		//-------------------------------------------------------------//
		//使用例
		// void OnCreate(){
		//		Draw_CreateFont(0,30);	//指定なしはＭＳゴシック
		// }
		//使用例　0番に設定したサイズ30のフォントで描画処理を行う
		//	Draw_FontText(0,0,"MainStage",ARGB(255,255,255,0),0);
		//
		//---------------------------------------------------------------------------------------
		void Create(int Num, int size, LPCTSTR	fontname)
		{
			if (pFontData[Num] == NULL) {
				// フォントの作成
				D3DXCreateFont(pD3DDevice,			//LPDIRECT3DDEVICE9	lpD3DDev,
					size,							//UINT				Height,
					0,								//UINT				Width,
					FW_NORMAL,						//UINT				Weight,
					D3DX_DEFAULT,					//UINT				MipLevels,
					FALSE,							//BOOL				Italic,
					DEFAULT_CHARSET,				//DWORD				CharSet,
					OUT_DEFAULT_PRECIS,				//DWORD				OutputPrecision,
					PROOF_QUALITY,					//DWORD				Quality,
					DEFAULT_PITCH | FF_DONTCARE,	//DWORD				PitchAndFamily,
					fontname,						//LPCTSTR			pFacename,
					&pFontData[Num]						//LPD3DXFONT		*ppFont		&pD3DXFont
					);
			}
			else {
				return;
			}
		}

		void CreateItalic(int Num, int size, LPCTSTR	fontname)
		{
			if (pFontData[Num] == NULL) {
				// フォントの作成
				D3DXCreateFont(pD3DDevice,			//LPDIRECT3DDEVICE9	lpD3DDev,
					size,							//UINT				Height,
					0,								//UINT				Width,
					FW_NORMAL,						//UINT				Weight,
					D3DX_DEFAULT,					//UINT				MipLevels,
					TRUE,							//BOOL				Italic,
					DEFAULT_CHARSET,				//DWORD				CharSet,
					OUT_DEFAULT_PRECIS,				//DWORD				OutputPrecision,
					PROOF_QUALITY,					//DWORD				Quality,
					DEFAULT_PITCH | FF_DONTCARE,	//DWORD				PitchAndFamily,
					fontname,						//LPCTSTR			pFacename,
					&pFontData[Num]						//LPD3DXFONT		*ppFont		&pD3DXFont
					);
			}
			else {
				return;
			}
		}


		//---------------------------------------------------------------------------------------
		//フォントテーブル初期化
		//---------------------------------------------------------------------------------------
		void Init()
		{
			for (int i = 0; i < DDFONTMAX; i++) {
				pFontData[i] = NULL;
			}
		}
		//---------------------------------------------------------------------------------------
		//フォント削除
		//---------------------------------------------------------------------------------------
		void Delete()
		{
			for (int i = 0; i < DDFONTMAX; i++) {
				if (pFontData[i] != NULL) {
					RELEASE(pFontData[i]);
				}
			}
		}

		//-------------------------------------------------------------//
		//文字描画
		//	x,y,		文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	t = 0	フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//	
		//フォント番号指定時はOnCreateにてフォントを用意すること
		//void OnCreate()
		//{
		//		Draw_CreateFont(0,15);
		//		Draw_CreateFont(1,30,"HG創英角ﾎﾟｯﾌﾟ体");
		//}
		//
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//-------------------------------------------------------------//
		void TextXYNC(int x, int y, const std::string& msg, D3DCOLOR color, int fontID)
		{
			TextNC(x, y, 0.0f, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	rt	表示領域
		//	z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		int TextNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL,
				NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_LEFT | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
			return h;
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	x,y,z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		void TextNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return;
			//表示テキストサイズ計算
			TEXTMETRIC  TextMetric;
			pFontData[fontID]->GetTextMetrics(&TextMetric);
			int w = x + (TextMetric.tmMaxCharWidth * msg.length());
			int h = y + TextMetric.tmHeight;
			//表示用RECT作成
			RECT	rt = { x, y, w, h };

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL,
				NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			pFontData[fontID]->DrawText(pSprite, msg.c_str(), -1, &rt, DT_LEFT | DT_SINGLELINE, color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
		}

		//-------------------------------------------------------------//
		//文字描画
		//	x,y,		文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	t = 0	フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//	
		//フォント番号指定時はOnCreateにてフォントを用意すること
		//void OnCreate()
		//{
		//		Draw_CreateFont(0,15);
		//		Draw_CreateFont(1,30,"HG創英角ﾎﾟｯﾌﾟ体");
		//}
		//
		//	Draw_TextXY(0,0,"MainStage",ARGB(255,255,255,0),1);
		//-------------------------------------------------------------//
		void TextXY(int x, int y, const std::string& msg, D3DCOLOR color, int fontID)
		{
			int id = camera::GetCameraId();

			TextNC(
				camera::cameras[id]->ixchange(x), camera::cameras[id]->iychange(y),
				0, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	rt	表示領域
		//	z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		int Text(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			int id = camera::GetCameraId();
			OffsetRect(&rc, -camera::cameras[id]->iviewLeft(), -camera::cameras[id]->iviewTop());
			return TextNC(rc, z, msg, color, fontID);
		}

		//---------------------------------------------------------------------------------------
		//文字描画
		//	x,y,z	文字描画位置
		//	string	文字の幅
		//	color	文字色
		//	Num		使用フォント番号
		//-------------------------------------------------------------//
		//使用例
		//	Draw_FontDrawText(0,0,"MainStage",ARGB(255,255,0),1);
		//---------------------------------------------------------------------------------------
		void Text(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			int id = camera::GetCameraId();

			TextNC(
				x - camera::cameras[id]->iviewLeft(), y - camera::cameras[id]->iviewTop(),
				z, msg, color, fontID);
		}

		//--------------------------------------------------
		// 追加
		//--------------------------------------------------
		void TextLeftMiddle(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			int id = camera::GetCameraId();
			TextLeftMiddleNC(x - camera::cameras[id]->iviewLeft(), y - camera::cameras[id]->iviewTop(), z, msg, color, fontID);
		}

		int TextLeftMiddle(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			int id = camera::GetCameraId();
			OffsetRect(&rc, -camera::cameras[id]->iviewLeft(), -camera::cameras[id]->iviewTop());
			return TextLeftMiddleNC(rc, z, msg, color, fontID);
		}

		void TextLeftMiddleNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return;

			//表示テキストサイズ計算
			TEXTMETRIC  TextMetric;
			pFontData[fontID]->GetTextMetrics(&TextMetric);
			int len = TextMetric.tmMaxCharWidth * msg.length();
			int halfH = TextMetric.tmHeight / 2;

			//表示用RECT作成
			RECT rt = { x, y - halfH, x + len, y + halfH };

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);

			pFontData[fontID]->DrawText(pSprite, msg.c_str(), -1, &rt, DT_LEFT | DT_VCENTER | DT_SINGLELINE, color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
		}

		int TextLeftMiddleNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_LEFT | DT_VCENTER | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);

			//戻り値
			//関数が終了すると、描画したテキストの高さが返ります。DT_VCENTER または DT_BOTTOM を指定した場合は、lpRect->top から描画したテキストの下端までのオフセットが返ります。
			//関数が失敗すると、0 が返ります。
			return h;
		}

		void TextCenter(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			int id = camera::GetCameraId();
			TextCenterNC(x - camera::cameras[id]->iviewLeft(), y - camera::cameras[id]->iviewTop(), z, msg, color, fontID);
		}

		int TextCenter(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			int id = camera::GetCameraId();
			OffsetRect(&rc, -camera::cameras[id]->iviewLeft(), -camera::cameras[id]->iviewTop());
			return TextCenterNC(rc, z, msg, color, fontID);
		}

		int TextCenterTop(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			int id = camera::GetCameraId();
			OffsetRect(&rc, -camera::cameras[id]->iviewLeft(), -camera::cameras[id]->iviewTop());
			return TextCenterTopNC(rc, z, msg, color, fontID);
		}

		int TextCenterBottom(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			RECT rc = rt;
			int id = camera::GetCameraId();
			OffsetRect(&rc, -camera::cameras[id]->iviewLeft(), -camera::cameras[id]->iviewTop());
			return TextCenterBottomNC(rc, z, msg, color, fontID);
		}

		void TextCenterNC(int x, int y, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return;

			//表示テキストサイズ計算
			TEXTMETRIC  TextMetric;
			pFontData[fontID]->GetTextMetrics(&TextMetric);
			int halfW = (TextMetric.tmMaxCharWidth * msg.length()) / 2;
			int halfH = TextMetric.tmHeight / 2;

			//表示用RECT作成
			RECT rt = { x - halfW, y - halfH, x + halfW, y + halfH };

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);

			pFontData[fontID]->DrawText(pSprite, msg.c_str(), -1, &rt, DT_CENTER | DT_SINGLELINE, color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);
		}

		int TextCenterNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_CENTER | DT_VCENTER | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);

			return h;
		}

		int TextCenterTopNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_CENTER | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);

			return h;
		}

		int TextCenterBottomNC(const RECT& rt, float z, const std::string& msg, D3DCOLOR color, int fontID)
		{
			draw::ReDrawCheck(color);
			if (pFontData[fontID] == NULL)	return 0;

			//Z軸
			D3DXMATRIX matDraw;	//	座標変換マトリックスの格納先
			D3DXVECTOR3 position(0.0f, 0.0f, z);	//	表示する位置を指定
			D3DXMatrixTransformation(&matDraw, NULL, NULL, NULL, NULL, NULL, &position);
			pSprite->SetTransform(&matDraw);
			//描画
			RECT rc = rt;
			int h = pFontData[fontID]->DrawText(
				pSprite,
				msg.c_str(),
				-1,
				&rc,
				DT_CENTER | DT_BOTTOM | DT_WORDBREAK,
				color);
			//ワールドのリセット
			D3DXMatrixRotationZ(&matDraw, 0.0f);
			pSprite->SetTransform(&matDraw);

			return h;
		}

	}


	//---------------------------------------------------------------------------------------
	// システム関連
	//---------------------------------------------------------------------------------------
	namespace system {

		//##### Customized by S.Ueyama
		void Quit()
		{
			DestroyWindow(hWnd);
		}
		//#####

		//---------------------------------------------------------------------------------------
		//DirectDraw初期化
		//---------------------------------------------------------------------------------------
		void InitDx()
		{
			//DX MAIN INIT
			pD3D = Direct3DCreate9(D3D_SDK_VERSION);
			if (pD3D == NULL) {
				return;
			}



			//WINDOW MODE INIT
			ZeroMemory(&D3DPP, sizeof(D3DPP));
			if (WindowMode == true) {
				D3DPP.Windowed = TRUE;							//WINDOW MODE or FULL MODE
				D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;		//SWAP AUTO Select
				D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;				//FORMAT AUTO Select
				D3DPP.BackBufferCount = 1;
				D3DPP.EnableAutoDepthStencil = TRUE;
				//D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
				//2016 2/27 渡加敷
				D3DPP.AutoDepthStencilFormat = D3DFMT_D24S8;
				D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
			else {
				D3DPP.BackBufferWidth = WINW;
				D3DPP.BackBufferHeight = WINH;
				D3DPP.BackBufferCount = 1;
				D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;				//FORMAT Select
				D3DPP.Windowed = FALSE;						//WINDOW MODE or FULL MODE
				D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;		//SWAP AUTO Select		
				D3DPP.EnableAutoDepthStencil = TRUE;
				D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
				D3DPP.FullScreen_RefreshRateInHz = REFRESHRATE;
				D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}

			// Multithread要注意
			// http://okwave.jp/qa/q4219151.html
			// BehaviorにD3DCREATE_MULTITHREADEDフラグを追加
			// このフラグを追加しないと、読み込み中に描画したら、メモリの問題が発生します。

			//Create Device
			HRESULT hr;
			//HAL(pure vp)
			if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DPP, &pD3DDevice)))
			{
				//HAL(soft vp)
				if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DPP, &pD3DDevice)))
				{
					//REF
					if (FAILED(hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
						D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &D3DPP, &pD3DDevice)))
					{
						assert(0 && "Can't create DirectX Device.");
						return;
					}
				}
			}

			// 両面描画モードの指定
			pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			// Ｚ比較を行なう
			pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			// ディザリングを行なう（高品質描画）
			pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
			// ノーライティングモード 	
			pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			//--------------------------------------
			// テクスチャステージの設定
			//--------------------------------------
			// テクスチャステージの設定 
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);

			// テクスチャフィルターの設定
			pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pD3DDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

			pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

			D3DXCreateSprite(pD3DDevice, &pSprite);


		}

		//---------------------------------------------------------------------------------------
		//DirectDraw終了
		//---------------------------------------------------------------------------------------
		void DelDx()
		{
			RELEASE(pSprite);
			RELEASE(pD3DDevice);
			RELEASE(pD3D);
		}

		//---------------------------------------------------------------------------------------
		// ループ制御
		//---------------------------------------------------------------------------------------
		static bool System_LoopControl(int FPS) {
			static int _Time = timeGetTime();	//初期時間格納
			static float _FPS;					//FPS計算用バッファ
			if (FPS != 0) {
				_FPS = 1000.0f / static_cast<float>(FPS);		//与えられたFPSからFrameTimeを計算	
				if (timeGetTime() - _Time > (unsigned)_FPS) {		//FrameTimeが経過するまで待つ
					_Time = timeGetTime();
					return 1;
				}
				return 0;
			}
			else {
				return 1;
			}
		}

		//---------------------------------------------------------------------------------------
		//WINDOW　プロシージャ
		//---------------------------------------------------------------------------------------
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT	ps;
			HRESULT res;
			switch (msg) {
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE) { DestroyWindow(hWnd); }
				break;
			case WM_CREATE:
				res = timeBeginPeriod(1);
				break;
			case WM_PAINT:
				BeginPaint(hWnd, &ps);
				EndPaint(hWnd, &ps);
				break;
			case WM_DESTROY:
				timeEndPeriod(1);
				PostQuitMessage(0);
				break;
			case WM_MOUSEWHEEL:
				input::delta = GET_WHEEL_DELTA_WPARAM(wParam);
				break;
			default: return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			return (0L);
		}

		//---------------------------------------------------------------------------------------
		// アプリ更新
		//---------------------------------------------------------------------------------------
//		int DoWindow(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
//		{
//			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//			MSG lpMsg;
//
//			{
//
//				WNDCLASS	myProg;
//				LPCSTR		Name = "Window_Class_Name";
//				if (!hPreInst)
//				{
//					myProg.style = CS_HREDRAW | CS_VREDRAW;
//					myProg.lpfnWndProc = WndProc;
//					myProg.cbClsExtra = 0;
//					myProg.cbWndExtra = 0;
//					myProg.hInstance = hInstance;
//					myProg.hIcon = LoadIcon(hInstance, "MAIN");
//					myProg.hCursor = LoadCursor(NULL, IDC_ARROW);
//					myProg.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//					myProg.lpszMenuName = NULL;
//					myProg.lpszClassName = (Name);
//					if (!RegisterClass(&myProg)) return false;
//				}
//
//				RECT rc;
//				SetRect(&rc, 0, 0, WINW, WINH);
//				AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);
//
//				DWORD style;
//				if (WindowMode) style = WS_OVERLAPPEDWINDOW;
//				else           style = WS_POPUP;
//				hWnd = CreateWindow(
//					(Name),
//					(USERNAME),
//					style,
//					0, 0,
//					rc.right - rc.left,
//					rc.bottom - rc.top,
//					NULL,
//					NULL,
//					hInstance,
//					NULL);
//
//
//				InitDx();
//				//resource table init
//        draw::InitImgTable();
//				font::Init();
//				//player input init
//				input::InitPlayerInput();
//				for (int i = 0; i < PLAYER_NUM; i++)
//					input::InitKeyInfo(i);
//				//stage flag init
//				stage::InitStageFlag();
//				math::RandomInit();
//				//sound com init
//#ifdef CI_EXT 
//				CoInitialize(NULL);
//				//	サウンド環境の初期化
//				{
//					ci_ext::MYERROR_DS ret_ds;	//	DirectSoundエラー番号格納用
//					ret_ds = ci_ext::dsInitDirectSound(hWnd);
//					if (ret_ds != ci_ext::eERRORDS_NOTHING)
//					{
//						//	環境が初期化できないので、ウインドウを終了させる
//						std::string szMsg = ci_ext::dsGetErrorMessage(ret_ds);
//						MessageBox(hWnd, szMsg.c_str(), NULL, 0);
//						DestroyWindow(hWnd);
//					}
//				}
//#endif
//#ifdef USE_DIRECTSOUND
//				se::Init();
//#endif
//#ifdef USE_DIRECTSHOW
//				bgm::Init();
//#endif
//				srand((unsigned)::time(NULL));
//        time::isCheck();
//				//user init
//        camera::SetUp();
//				OnCreate();
//#ifdef CI_EXT
//				//	StartOutputDbgString();
//				ShowConsole();
//				ci_ext::TaskManager game;
//				//TaskManagerのポインタをタスクに登録している
//				{ci_ext::TaskFirst dummy(&game); } //{}は意図的なので消去しないこと
//				game.changeScene(0);
//#endif
//      	ShowWindow(hWnd, nCmdShow);
//				UpdateWindow(hWnd);
//				while (true){
//					if (PeekMessage(&lpMsg, NULL, 0, 0, PM_REMOVE)){			//メッセージ処理
//						if (lpMsg.message == WM_QUIT)	break;
//						TranslateMessage(&lpMsg);
//						DispatchMessage(&lpMsg);
//					}
//					else if (hWnd == GetActiveWindow()){
////            draw::Clear();
//						input::CheckKey();
//						//ゲームループ
//#ifdef CI_EXT
//						if (game.run()) DestroyWindow(hWnd);
//						ci_ext::dsCheck_Stream();
//#else
//						GameLoop();
//#endif
//						camera::StepCamera();
////            draw::Refresh();
//						//サウンド終了チェック
//#ifdef USE_DIRECTSHOW
//						bgm::EndCheck();
//#endif
//#ifdef USE_DIRECTSOUND
//						se::EndCheck();
//#endif
//						//ホイールdeltaクリア
//						input::ClearWheelDelta();
//						//TIMER更新
//            FrameTime = time::Update();
//					}
//				}
//				//user release
//				OnDestroy();			//ゲーム後始末処理
//				//resource release
//        draw::DelImgTable();
//				font::Delete();
//				//sound com release
//#ifdef CI_EXT
//				ci_ext::dsRelease();
//#endif
//#ifdef USE_DIRECTSOUND
//				se::Del();
//
//#endif
//#ifdef USE_DIRECTSHOW
//				bgm::Del();
//#endif
//				//directx release
//				DelDx();
//				//pad delete
//				input::DeletePlayerInput();
//				//memory leaks dump
//				//	_CrtDumpMemoryLeaks();
//			}
//			return static_cast<int>(lpMsg.wParam);
//		}
//
	}


#ifdef CI_EXT
	ci_ext::TaskManager* ci_ext::Task::p_ = nullptr;
	int ci_ext::Task::uid_ = 0;
#endif


	//********************************************************************//
	//
	//				BGM再生　関連関数 
	//
	//********************************************************************//
	namespace bgm {
#ifdef USE_DIRECTSHOW

		//関数一覧
		/*
		bool		DShow_Init()						DShowの初期化
		void		DShow_Del();						DShow_の後始末
		DSHOWOBJ	DShow_LoadFile(LPCWSTR)				音楽ファイルの読み込み
		bool		DShow_Play();						再生
		void		DShow_RateControl(DSHOWOBJ,float)	再生速度変更
		void		DShow_EndCheck()					終了チェック
		void		DShow_Stop(DSHOWOBJ)				再生ストップ
		void		DShow_AllStop()						全停止
		LONGLONG	DShow_GetCurrentPos(int index)		再生位置取得
		void		DShow_SetStartPos(int index)		スタート位置に設定
		*/
		//-------------------------------------------------------
		/*
		使い方

		グローバルとして宣言
		DSHOWOBJ		test;

		再生処理
		DShow_Play(test);		//通常再生-すべてループ再生

		停止処理
		DShow_Stop(test);		//同じ音声をすべて停止
		DShow_AllStop();		//再生中のすべての音声を停止

		エフェクト
		DShow_RateControl(test,1.0);	//testの再生速度を指定した倍率に変更する。

		*/
		//--------------------------------------------------------------------------------------------
		// DirectShowの初期化
		//--------------------------------------------------------------------------------------------
		void ReleaseObj(DxShow& bgm) {
			bgm.pMediaControl->Release();
			bgm.pMediaEvent->Release();
			bgm.pMediaSeeking->Release();
			bgm.pBuilder->Release();
			bgm.pBasicAudio->Release();
		}

		void	LoadFile(const std::string& resname, const std::string& filename)
		{
			//既に読み込まれているリソース名なら、エラー
			if (BgmTable.count(filename) != 0) {
				assert(!"同じ名前のリソースが登録済みです。名前の確認をしてください。");
			}
			for (const auto bgm : BgmTable) {
				if (bgm.second.filename == filename) {
					assert(!"同じファイル名のリソースが登録済みです。名前の確認をしてください。");
				}
			}
			BgmObj bgmObj;
			GUID format = TIME_FORMAT_FRAME;
			wchar_t name[256];
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)filename.c_str(), -1, name, 256);

			HRESULT	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
				IID_IGraphBuilder, (void **)&bgmObj.bgm.pBuilder);
			if (FAILED(hr)) {
				debug::BToM("DShow:GraphBuilderオブジェクトの生成に失敗しました");
				goto error;
			}

			if (FAILED(bgmObj.bgm.pBuilder->RenderFile(name, NULL))) {
				debug::BToM("DShow\nファイル読み込みに失敗しました\nファイル名、コーデックの確認をお願いします。");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaControl,
				(void**)&bgmObj.bgm.pMediaControl))) {
				debug::BToMR("DShow\nMediaControlの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaEvent,
				(void**)&bgmObj.bgm.pMediaEvent))) {
				debug::BToMR("DShow\nMediaEventの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IMediaSeeking,
				(void**)&bgmObj.bgm.pMediaSeeking))) {
				debug::BToMR("DShow\nMediaSeekingの設定に失敗しました");
				goto error;
			}
			if (FAILED(bgmObj.bgm.pBuilder->QueryInterface(IID_IBasicAudio,
				(void**)&bgmObj.bgm.pBasicAudio))) {
				debug::BToMR("DShow\nBasicAudioの設定に失敗しました");
				goto error;
			}

			bgmObj.bgm.pMediaSeeking->SetTimeFormat(&format);
			bgmObj.bgm.use = 1;
			bgmObj.filename = filename;
			BgmTable.insert(std::unordered_map<std::string, BgmObj>::value_type(resname, bgmObj));
			return;

		error:
			ReleaseObj(bgmObj.bgm);

		}
		//--------------------------------------------------------------------------------------------
		// DirectShowの後始末
		//--------------------------------------------------------------------------------------------
		void	Del()
		{
			AllStop();
			for (auto& bgm : BgmTable) {
				if (bgm.second.bgm.use == 1) {
					ReleaseObj(bgm.second.bgm);
				}
			}
			BgmTable.clear();
			CoUninitialize();
		}

		void Delete(const std::string& resname)
		{
			/*
			if (BgmTable.count(resname) == 0)
				assert(!"そのリソースは存在しません");
			ReleaseObj(BgmTable.at(resname).bgm);
			//*/
			if (BgmTable.count(resname) == 0)
				return;
			ReleaseObj(BgmTable.at(resname).bgm);
			BgmTable.erase(resname);
		}

		//2016-06-12 追加
		void DeleteAll()
		{
			//for (auto& bgm : BgmTable) {
			//	Delete(bgm.first);
			//}
			for (auto it = BgmTable.begin(); it != BgmTable.end();)
			{
				if ((*it).second.bgm.use != 0)
					ReleaseObj((*it).second.bgm);
				it = BgmTable.erase(it);
			}
		}

		//--------------------------------------------------------------------------------------------
		// GraphBuilderの生成
		//--------------------------------------------------------------------------------------------
		void Init()
		{
			CoInitialize(NULL);
		}

		//--------------------------------------------------------------------------------------------
		// ファイルの再生
		//--------------------------------------------------------------------------------------------
		bool	Play(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return false;
			/*
			REFERENCE_TIME current;
			BgmTable.at(resname).bgm.pMediaSeeking->GetCurrentPosition(&current);
			REFERENCE_TIME duration;
			BgmTable.at(resname).bgm.pMediaSeeking->GetDuration(&duration);
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, nullptr, AM_SEEKING_NoPositioning);
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&current, AM_SEEKING_AbsolutePositioning, &duration, AM_SEEKING_IncrementalPositioning);
			//*/
			HRESULT	hr = BgmTable.at(resname).bgm.pMediaControl->Run();
			if (FAILED(hr)) {
				debug::BToMR("DShow:再生に失敗しました1");
				return false;
			}
			return true;
		}

		//--------------------------------------------------------------------------------------------
		// 再生速度の再生
		//--------------------------------------------------------------------------------------------
		void RateControl(const std::string& resname, float rate)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (rate < 0)	return;
			BgmTable.at(resname).bgm.pMediaSeeking->SetRate(rate);
		}
		//--------------------------------------------------------------------------------------------
		// 再生音量の設定
		// 0から100で設定　0は無音 100は最大 
		//--------------------------------------------------------------------------------------------
		void VolumeControl(const std::string& resname, int volume)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (volume < 0 || volume > 100)	return;
			//0から-10000で設定 0が最大　-10000は無音
			//100で割った値がデシベル
			long vol = (long)(-10000 + (volume * 100));
			BgmTable.at(resname).bgm.pBasicAudio->put_Volume(vol);
		}
		//--------------------------------------------------------------------------------------------
		// 停止
		//--------------------------------------------------------------------------------------------
		void Stop(const std::string& resname)
		{
			if (BgmTable.count(resname) == 0) return;
			if (BgmTable.at(resname).bgm.use == 0) return;
			LONGLONG	start = 0;
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
				NULL, AM_SEEKING_NoPositioning);
			BgmTable.at(resname).bgm.pMediaControl->Stop();
		}

		//--------------------------------------------------------------------------------------------
		// 一時停止
		//--------------------------------------------------------------------------------------------
		void Pause(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			if (BgmTable.at(resname).bgm.use == 1) {
				BgmTable.at(resname).bgm.pMediaControl->Stop();
			}
		}

		//--------------------------------------------------------------------------------------------
		// 全停止
		//--------------------------------------------------------------------------------------------
		void AllStop()
		{
			for (auto& bgm : BgmTable) {
				if (bgm.second.bgm.use == 1) {
					//##### Customized by Sui
					//IMediaSeeking
					LONGLONG	start = 0;
					bgm.second.bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
						NULL, AM_SEEKING_NoPositioning);
					//#####
					bgm.second.bgm.pMediaControl->Stop();
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 終了確認
		//--------------------------------------------------------------------------------------------
		void EndCheck()
		{
			for (auto& bgm : BgmTable) {
				if (bgm.second.bgm.use == 1) {
					//			long code;
								//再生位置取得
					LONGLONG endPos = bgm::GetEndPosition(bgm.first);
					LONGLONG nowPos = bgm::GetCurrentPos(bgm.first);
					//ブロッキングするようなのでやめ
					//bgm.second.bgm.pMediaEvent->WaitForCompletion(0, &code);
					//再生終了時はループを行う。
		//			if(code == EC_COMPLETE){
					if (endPos <= nowPos) {
						LONGLONG	start = 0;
						bgm.second.bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
							NULL, AM_SEEKING_NoPositioning);
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 再生終了位置取得
		//--------------------------------------------------------------------------------------------
		LONGLONG GetEndPosition(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return -1L;
			LONGLONG	end;
			//終了位置の取得
			BgmTable.at(resname).bgm.pMediaSeeking->GetStopPosition(&end);
			return end;
		}

		//--------------------------------------------------------------------------------------------
		// 再生位置取得
		//--------------------------------------------------------------------------------------------
		LONGLONG GetCurrentPos(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return -1L;
			LONGLONG	Current;
			BgmTable.at(resname).bgm.pMediaSeeking->GetCurrentPosition(&Current);
			return Current;
		}

		//--------------------------------------------------------------------------------------------
		// 再生位置初期化
		//--------------------------------------------------------------------------------------------
		void SetStartPos(const std::string& resname)
		{
			if (BgmTable.at(resname).bgm.use == 0) return;
			LONGLONG	start = 0;
			BgmTable.at(resname).bgm.pMediaSeeking->SetPositions(&start, AM_SEEKING_AbsolutePositioning,
				NULL, AM_SEEKING_NoPositioning);
		}

#endif

	}


	//********************************************************************//
	//
	//				SE再生　関連関数
	//
	//********************************************************************//
	namespace se {
#ifdef USE_DIRECTSOUND
		//--------------------------------------------------------------------------------------------
		// DirectSoundの初期化
		//--------------------------------------------------------------------------------------------
		void Init()
		{
			if (Create()) {
				if (!CreatePrimaryBuffer()) {
					debug::BToMR("プライマリバッファの生成に失敗しました");
				}
				CreateSecondaryBuffer();
			}
			else {
				debug::BToMR("DSoundの生成に失敗しました");
			}
		}

		//--------------------------------------------------------------------------------------------
		// DirectSoundの作成
		//--------------------------------------------------------------------------------------------
		bool Create(void)
		{
			HRESULT ret;

			// DirectSound8を作成
			ret = DirectSoundCreate8(NULL, &lpDS, NULL);
			if (FAILED(ret)) {
				debug::BToMR("サウンドオブジェクト作成失敗\n", "");
				return false;
			}

			// 強調モード
			ret = lpDS->SetCooperativeLevel(system::hWnd, DSSCL_EXCLUSIVE | DSSCL_PRIORITY);
			if (FAILED(ret)) {
				debug::BToMR("強調レベル設定失敗\n", "");
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------
		// バッファの作成
		//--------------------------------------------------------------------------------------------
		void CreateSecondaryBuffer()
		{
			SeTable.clear();
		}

		//--------------------------------------------------------------------------------------------
		// サウンド再生
		//--------------------------------------------------------------------------------------------
//#include "ci_ext\MeasureTime.hpp"
		void Play(const std::string& resname)
		{
			int i;
			auto& se = SeTable.at(resname).se;

			//ci_ext::MeasureTime* m = new ci_ext::MeasureTime;

			for (i = 0; i < DSPLAYMAX; i++) {
				if (se.PlayBuffer[i].State == DSNONE) {
					//再生開始
					//			se.PlayBuffer[i].ID = SoundNo;
					se.PlayBuffer[i].State = DSPLAY;
					se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
					se.PlayBuffer[i].Loop = 0;
					break;
				}
			}

			//delete m;
			//std::string msg("+++++++++++++++ time:" + m + "\n");
			//OutputDebugString(msg.c_str());
		}

		//--------------------------------------------------------------------------------------------
		// サウンドループ再生
		//--------------------------------------------------------------------------------------------
		void PlayLoop(const std::string& resname)
		{
			int i;
			auto& se = SeTable.at(resname).se;
			for (i = 0; i < DSPLAYMAX; i++) {
				if (se.PlayBuffer[i].State == DSNONE) {
					//再生開始
					//			se.PlayBuffer[i].ID = SoundNo;
					se.PlayBuffer[i].State = DSPLAY;
					se.PlayBuffer[i].pBuffer->Play(0, 0, 0);
					se.PlayBuffer[i].Loop = 1;
					break;
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// サウンド停止
		//	同じ効果音をすべて停止する
		//--------------------------------------------------------------------------------------------
		void Stop(const std::string& resname)
		{
			if (SeTable.count(resname) == 0) return;
			auto& se = SeTable.at(resname).se;
			for (int j = 0; j < DSPLAYMAX; j++) {
				if (se.PlayBuffer[j].State == DSPLAY) {
					se.PlayBuffer[j].pBuffer->Stop();
					se.PlayBuffer[j].State = DSNONE;
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// すべてのサウンド停止
		//--------------------------------------------------------------------------------------------
		void AllStop()
		{
			for (auto& seObj : SeTable) {
				for (int j = 0; j < DSPLAYMAX; j++) {
					if (seObj.second.se.PlayBuffer[j].State == DSPLAY) {
						seObj.second.se.PlayBuffer[j].pBuffer->Stop();
						seObj.second.se.PlayBuffer[j].State = DSNONE;
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------
		// 終了チェック
		//--------------------------------------------------------------------------------------------
		void EndCheck()
		{
			DWORD Status;
			int a = 0;
			for (auto& seObj : SeTable) {
				for (int j = 0; j < DSPLAYMAX; j++) {
					if (seObj.second.se.PlayBuffer[j].State == DSPLAY) {		//現在再生中の複製バッファ
						//終了と認識する
						seObj.second.se.PlayBuffer[j].pBuffer->GetStatus(&Status);
						if ((Status & DSBSTATUS_PLAYING) != 1) {
							if (seObj.second.se.PlayBuffer[j].Loop) {
								seObj.second.se.PlayBuffer[j].pBuffer->SetCurrentPosition(0);
								seObj.second.se.PlayBuffer[j].pBuffer->Play(0, 0, 0);
							}
							else {
								seObj.second.se.PlayBuffer[j].State = DSNONE;
								seObj.second.se.PlayBuffer[j].BufferSize = 0;
								seObj.second.se.PlayBuffer[j].pBuffer->Stop();
							}
						}
					}
				}
			}
		}
		//--------------------------------------------------------------------------------------------
		// プライマリサウンドバッファの作成
		//--------------------------------------------------------------------------------------------
		bool CreatePrimaryBuffer(void)
		{
			HRESULT ret;
			WAVEFORMATEX wf;

			// プライマリサウンドバッファの作成
			DSBUFFERDESC dsdesc;
			ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
			dsdesc.dwSize = sizeof(DSBUFFERDESC);
			dsdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
			dsdesc.dwBufferBytes = 0;
			dsdesc.lpwfxFormat = NULL;
			ret = lpDS->CreateSoundBuffer(&dsdesc, &lpPrimary, NULL);
			if (FAILED(ret)) {
				debug::BToMR("プライマリサウンドバッファ作成失敗\n", "");
				return false;
			}

			// プライマリバッファのステータスを決定
			wf.cbSize = sizeof(WAVEFORMATEX);
			wf.wFormatTag = WAVE_FORMAT_PCM;
			wf.nChannels = 2;
			wf.nSamplesPerSec = 44100;
			wf.wBitsPerSample = 16;
			wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
			wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
			ret = lpPrimary->SetFormat(&wf);
			if (FAILED(ret)) {
				debug::BToMR("プライマリバッファのステータス失敗\n", "");
				return false;
			}
			return true;
		}

		//--------------------------------------------------------------------------------------------
		//DSoundの後始末
		//--------------------------------------------------------------------------------------------
		bool Del(void)
		{
			for (auto& seObj : SeTable) {
				if (seObj.second.se.BufferSize != 0) {
					seObj.second.se.sBuffer->Release();
				}
			}
			if (lpPrimary) {
				lpPrimary->Release();
				lpPrimary = NULL;
			}
			if (lpDS) {
				lpDS->Release();
				lpDS = NULL;
			}

			SeTable.clear();

			return true;
		}

		void Delete(const std::string& resname)
		{
			if (SeTable.count(resname) == 0) return;
			auto& seObj = SeTable.at(resname);
			if (seObj.se.BufferSize != 0)
				seObj.se.sBuffer->Release();
			SeTable.erase(resname);
		}

		//2016-06-12 追加
		void DeleteAll()
		{
			//std::cout << "Before SeTable Size : " << SeTable.size() << std::endl;
			for (auto it = SeTable.begin(); it != SeTable.end();) {
				if ((*it).second.se.BufferSize != 0)
					(*it).second.se.sBuffer->Release();
				it = SeTable.erase(it);
			}
			//std::cout << "SeTable Size : " << SeTable.size() << std::endl;
		}

		//--------------------------------------------------------------------------------------------
		// サウンドバッファの作成
		//--------------------------------------------------------------------------------------------
		void LoadFile(const std::string& resname, const std::string& filename)
		{
			HRESULT ret;
			MMCKINFO mSrcWaveFile;
			MMCKINFO mSrcWaveFmt;
			MMCKINFO mSrcWaveData;
			LPWAVEFORMATEX wf;

			//既に読み込まれているリソース名なら、エラー
			if (SeTable.count(filename) != 0) {
				assert(!"DSound:同じ名前のリソースが登録済みです。名前の確認をしてください。");
			}
			for (const auto& se : SeTable) {
				if (se.second.filename == filename) {
					assert(!"DSound:同じファイル名のリソースが登録済みです。名前の確認をしてください。");
				}
			}

			// WAVファイルをロード
			HMMIO hSrc;
			hSrc = mmioOpen((LPSTR)filename.c_str(), NULL, MMIO_ALLOCBUF | MMIO_READ | MMIO_COMPAT);
			if (!hSrc) {
				debug::BToMR("WAVファイルロードエラー\n", "");
				return;
			}

			// 'WAVE'チャンクチェック
			ZeroMemory(&mSrcWaveFile, sizeof(mSrcWaveFile));
			ret = mmioDescend(hSrc, &mSrcWaveFile, NULL, MMIO_FINDRIFF);
			if (mSrcWaveFile.fccType != mmioFOURCC('W', 'A', 'V', 'E')) {
				debug::BToMR("WAVEチャンクチェックエラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}

			// 'fmt 'チャンクチェック
			ZeroMemory(&mSrcWaveFmt, sizeof(mSrcWaveFmt));
			ret = mmioDescend(hSrc, &mSrcWaveFmt, &mSrcWaveFile, MMIO_FINDCHUNK);
			if (mSrcWaveFmt.ckid != mmioFOURCC('f', 'm', 't', ' ')) {
				debug::BToMR("fmt チャンクチェックエラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}

			// ヘッダサイズの計算
			int iSrcHeaderSize = mSrcWaveFmt.cksize;
			if (iSrcHeaderSize < sizeof(WAVEFORMATEX))
				iSrcHeaderSize = sizeof(WAVEFORMATEX);

			// ヘッダメモリ確保
			wf = (LPWAVEFORMATEX)malloc(iSrcHeaderSize);
			if (!wf) {
				debug::BToMR("メモリ確保エラー\n", "");
				mmioClose(hSrc, 0);
				return;
			}
			ZeroMemory(wf, iSrcHeaderSize);

			// WAVEフォーマットのロード
			ret = mmioRead(hSrc, (char*)wf, mSrcWaveFmt.cksize);
			if (FAILED(ret)) {
				debug::BToMR("WAVEフォーマットロードエラー\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// fmtチャンクに戻る
			mmioAscend(hSrc, &mSrcWaveFmt, 0);

			// dataチャンクを探す
			while (1) {
				// 検索
				ret = mmioDescend(hSrc, &mSrcWaveData, &mSrcWaveFile, 0);
				if (FAILED(ret)) {
					debug::BToMR("dataチャンクが見つからない\n", "");
					free(wf);
					mmioClose(hSrc, 0);
					return;
				}
				if (mSrcWaveData.ckid == mmioStringToFOURCC("data", 0)) {
					break;
				}
				// 次のチャンクへ
				ret = mmioAscend(hSrc, &mSrcWaveData, 0);
			}

			SeObj seObj;
			// サウンドバッファの作成
			DSBUFFERDESC dsdesc;
			ZeroMemory(&dsdesc, sizeof(DSBUFFERDESC));
			dsdesc.dwSize = sizeof(DSBUFFERDESC);
			dsdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC | DSBCAPS_LOCDEFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
			dsdesc.dwBufferBytes = mSrcWaveData.cksize;
			dsdesc.lpwfxFormat = wf;
			dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;
			ret = lpDS->CreateSoundBuffer(&dsdesc, &seObj.se.sBuffer, NULL);
			seObj.se.BufferSize = (WORD)dsdesc.dwBufferBytes;
			if (FAILED(ret)) {
				debug::BToMR("サウンドバッファの作成エラー\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// ロック開始
			LPVOID pMem1, pMem2;
			DWORD dwSize1, dwSize2;
			ret = (seObj.se.sBuffer)->Lock(0, mSrcWaveData.cksize, &pMem1, &dwSize1, &pMem2, &dwSize2, 0);
			seObj.se.sBuffer->GetFrequency(&seObj.se.Fre);
			if (FAILED(ret)) {
				debug::BToMR("ロック失敗\n", "");
				free(wf);
				mmioClose(hSrc, 0);
				return;
			}

			// データ書き込み
			mmioRead(hSrc, (char*)pMem1, dwSize1);
			mmioRead(hSrc, (char*)pMem2, dwSize2);

			// ロック解除
			(seObj.se.sBuffer)->Unlock(pMem1, dwSize1, pMem2, dwSize2);
			// ヘッダ用メモリを開放
			free(wf);
			// WAVを閉じる
			mmioClose(hSrc, 0);

			seObj.se.sBuffer->SetVolume(-50);

			//複製開始
			for (int t = 0; t < 10; t++) {
				if (lpDS->DuplicateSoundBuffer(seObj.se.sBuffer,
					&(seObj.se.PlayBuffer[t].pBuffer)) != DS_OK) {
					debug::BToMR("再生用バッファ作成失敗\n", "");
					break;
				}
				else {
					seObj.se.PlayBuffer[t].State = DSNONE;
					seObj.se.PlayBuffer[t].BufferSize = seObj.se.BufferSize;
					seObj.se.PlayBuffer[t].Loop = 0;
				}
			}
			SeTable.insert(std::unordered_map<std::string, SeObj>::value_type(resname, seObj));
		}


		//--------------------------------------------------------------------------------------------
		// 周波数変更 再生速度が変わる
		//--------------------------------------------------------------------------------------------
		void SetFrequency(const std::string& resname, int Fre)
		{
			auto se = SeTable.at(resname).se;
			se.sBuffer->SetFrequency(se.Fre + Fre);
		}

		void SetVolume(const std::string& resname, float Vol)
		{
			auto se = SeTable.at(resname).se;
			long volume;
			if (Vol >= 1.0f) {
				volume = DSBVOLUME_MAX;
			}
			else if (Vol <= 0.0f) {
				volume = DSBVOLUME_MIN;
			}
			else {
				volume = static_cast<long>((10 * log10((double)(Vol / 100))) * 100);
			}

			//##### Customized by Sui
			//HRESULT hr = se.sBuffer->SetVolume(volume);
			se.sBuffer->SetVolume(volume);
			for (int i = 0; i < DSPLAYMAX; i++) {
				//if (se.PlayBuffer[i].State == DSNONE){
				se.PlayBuffer[i].pBuffer->SetVolume(volume);
				//break;
			//}
			}
			//std::string msg("+++++++++++++++ volume:" + std::to_string(volume) + "\n");
			//OutputDebugString(msg.c_str());
			//return;
			//#####
		}

#endif
	}

}
