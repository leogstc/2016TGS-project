//------------------------------------------------------------------------------
//ウインドウクラス
//
//※auto std::function std::bind nullptrを使っているので、
//  コンパイラのバージョンに注意してください
//<例1>
//  ci_ext::Window* w = new ci_ext::Window(GetModuleHandle(nullptr), "タイトル", 1280, 720, "Windowsに登録するクラス名");
//  w->doMsgLoop();
//  delete w;
//<例2>
//  ci_ext::Window w(GetModuleHandle(nullptr), "タイトル", 1280, 720, "Windowsに登録するクラス名");
//  w.doMsgLoop();
//<例3>
//  Windowクラスを継承させたGameクラスを作成し、GameクラスのコンストラクタにdoMsgLoop()をかけば
//  ci_ext::Game game(GetModuleHandle(nullptr), "タイトル", 1280, 720, "Windowsに登録するクラス名");
//  ないしは
//  ci_ext::Game(GetModuleHandle(nullptr), "タイトル", 1280, 720, "Windowsに登録するクラス名");
//  でOK
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// 2016-06-11 スレッドを追加 by Sui
// public: hThread, runThread
// private: OnDestroy_default()
//------------------------------------------------------------------------------
#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include "../lib/ci_ext/Console.hpp"

//クラスメソッドとthisポインタをファンクタに登録するマクロ
#define BindThisPtr(x) std::function<LRESULT(HWND, UINT, WPARAM, LPARAM, void*)>(std::bind(x, this,\
std::placeholders::_1,\
std::placeholders::_2,\
std::placeholders::_3,\
std::placeholders::_4,\
std::placeholders::_5))

//クラスメソッドと任意ポインタをファンクタに登録するマクロ
#define BindPtr(ptr,x) std::function<LRESULT(HWND, UINT, WPARAM, LPARAM, void*)>(std::bind(x, ptr,\
std::placeholders::_1,\
std::placeholders::_2,\
std::placeholders::_3,\
std::placeholders::_4,\
std::placeholders::_5))

namespace ci_ext
{

//ウインドウクラス
class Window
{
public:
  //ウインドウハンドル取得
  HWND windowhandle() const { return windowhandle_; }

  //ウインドウメッセージ用
  struct WMessage
  {
    UINT msg;  //WM_* を代入
    std::function<LRESULT(HWND, UINT, WPARAM, LPARAM, void*)> proc_method;   //クラスメソッド代入用
    void* param; //パラメータ
    bool operator==(const UINT m){ return msg == m; }
  };

  //コンストラクタでウインドウ生成
  Window(const std::string& title, int w, int h,
    const char* classname = "default_class_name") 
    :
    title_(title),
    windowhandle_(nullptr),
    width_(w),
    height_(h),
    classname_(classname)
  {
    WNDCLASSEX wcex;//ウインドウ作成に使用
    RECT rcWindowSize = { 0, 0, width_, height_ };//ウインドウサイズ
    HINSTANCE inst = GetModuleHandle(nullptr);//インスタンスハンドルを作成
    id_ = ++uid_;//ユニークIDを管理

    //データ初期化
    wcex.style = (CS_HREDRAW | CS_VREDRAW);
    wcex.hIcon = LoadIcon(inst, IDI_APPLICATION);
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hIconSm = LoadIcon(inst, IDI_APPLICATION);
    wcex.hInstance = inst;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.lpfnWndProc = WndProcFirst;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = classname_.c_str();
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

    //Windowsに登録
    if (!RegisterClassEx(&wcex))
    {
      MessageBox(nullptr, "ウインドウ登録に失敗", nullptr, MB_OK);
      throw "RegisterClassEx()";
    }

    //コールバックを登録
    insertMsg(WM_CREATE, BindThisPtr(&Window::OnCreate_default));
    insertMsg(WM_DESTROY, BindThisPtr(&Window::OnDestroy_default));
    insertMsg(WM_KEYDOWN, BindThisPtr(&Window::OnKeyDown_default));
    insertMsg(WM_ACTIVATEAPP, BindThisPtr(&Window::OnActivateApp_default));
    insertMsg(WM_MOUSEWHEEL, BindThisPtr(&Window::OnMouseWheel_default));

    //クライアント領域のサイズからウインドウを作成
    AdjustWindowRectEx(&rcWindowSize, (WS_CAPTION | WS_SYSMENU), FALSE, WS_EX_APPWINDOW);
    windowhandle_ = CreateWindowEx(
      WS_EX_APPWINDOW, classname_.c_str(), title_.c_str(),
      (WS_CAPTION | WS_SYSMENU), 
      0, 0,//左上座標
      (rcWindowSize.right - rcWindowSize.left),//ウインドウの幅
      (rcWindowSize.bottom - rcWindowSize.top),//ウインドウの高さ
      nullptr, nullptr, inst, static_cast<void*>(this));

    //ウインドウ生成に失敗した？
    if (!windowhandle_)
    {
      MessageBox(nullptr, "ウインドウ生成に失敗", nullptr, MB_OK);
      throw "CreateWindowEx()";
    }

    //ユーザ用初期化関数を呼ぶ、今後の継承された子供クラス用に
    init();

    //ウインドウの状態を更新する
    ShowWindow(windowhandle_, SW_SHOW);
    UpdateWindow(windowhandle_);

    //ここにdoMsgLoop()を記述することで、コンストラクタのみでOKとなるが
    //スレッドを分けていないので、複数個のウインドウを生成すると不具合が起きるので注意する
    //doMsgLoop();
  }

  //破棄時に処理
  virtual ~Window()
  {
    DOUT << "window was destroyed..." << std::endl;
  }

  //ウインドウ生成後に呼ばれる
  //各ウインドウメッセージに対する処理を追記する場合は、ここにinsertMsg()を使って登録してください
  virtual void init() {}

  //メッセージに対する処理を登録する
  //🔴関数やstaticなメソッドで登録
  //二番目の引数には、関数名を記述する
  //🔴クラスメソッドで登録
  //二番目の引数には、呼び出し元クラスのポインタをbindする必要があるため、
  //BindPtrもしくはBindThisPtrマクロを使ってください。
  //  <例>
  //  this->insertMsg(WM_LBUTTONDOWN, BindThisPtr(&Window::OnLButtonDown));
  //  otherWindow->insertMsg(WM_LBUTTONDOWN, BindThisPtr(otherWindow, &Window::OnLButtonDown));
  //※一見ではBindThisPtrマクロポインタが不要に見えますが、関数先でクラスメソッドへのポインタの型があいまいになるために
  //　このような記述が必要です。主に継承後のクラスメソッドを登録しようとすると問題がおきます。
  //  改善策があればご連絡を
  void insertMsg(int msg, std::function<LRESULT(HWND, UINT, WPARAM, LPARAM, void*)> proc, void* param = nullptr)
  {
    WMessage t = { (UINT)msg, proc, param };
    auto it = std::find(wm_.begin(), wm_.end(), msg);
    if (wm_.end() == it)
      wm_.push_back(t);
    else
    {
      //登録するメッセージが既に登録されているなら上書き
      it->msg = t.msg;
      it->param = t.param;
      it->proc_method = t.proc_method;
    }
  }

  //ウインドウに登録されているメッセージに対する処理を消去する
  void deleteMsg(int msg)
  {
    auto m = std::find(wm_.begin(), wm_.end(), msg);
    if (wm_.end() != m)
      wm_.erase(m);
  }

  //windowsメッセージを処理し、メッセージの無いときはupdate関連メソッドを呼ぶ
  bool doMsg()
  {
    MSG msg;
    if (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
    {
      if (!GetMessage(&msg, nullptr, 0, 0)) return false;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
//    else if (GetActiveWindow() == windowhandle_)
    {
      //メッセージが無いならゲームの処理へ移行
      update_before(); 
      update();        
      update_after();  
    }
    return true;
  }
  
  //windowsメッセージを処理し、メッセージ外の処理は呼び出し側で処理する場合に使う
  enum class MsgReturn
  {
    quit,
    update,
    no_update
  };
  MsgReturn doMsg2()
  {
    MSG msg;
    if (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
    {
      if (!GetMessage(&msg, nullptr, 0, 0)) return MsgReturn::quit;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
//    else if (GetActiveWindow() == windowhandle_)
    {
      return MsgReturn::update;
    }
    return MsgReturn::no_update;
  }
  //メッセージループ
  //WM_QUITが呼ばれるまでループさせる
  void doMsgLoop()
  {
    while (doMsg());
  }

  //Windowクラスを継承してゲームクラスを作る場合、
  //update()をoverrideすることでGameLoopとすることができる
  virtual void update_before() {} //前処理
  virtual void update() {}        //本処理
  virtual void update_after() {}  //後処理

  int nowWheelDelta() const { return nowwheeldelta_; }
private:
  HWND windowhandle_;   //ウインドウハンドル
  static int uid_;      //ユニークなID
  int id_;              //ID
  std::string title_;   //ウインドウタイトル
  int width_, height_;  //クライアント領域の幅、高さ
  std::vector<WMessage> wm_;    //ウインドウプロシージャで処理するメッセージ＆関数を登録
  const std::string classname_; //Windowsに登録する名前
  int nowwheeldelta_; //マウスホイール量

public:
  static HANDLE hThread; //スレッドハンドル
  static bool runThread; //スレッドを実行可能のフラグ

private:

  //ウインドウプロシージャ
  //wm_に登録されているメッセージを登録されている関数で処理し
  //その他はWindowsに任せる
  LRESULT CALLBACK wndproc(HWND h, UINT m, WPARAM w, LPARAM l)
  {
    for (size_t i = 0; i < wm_.size(); ++i)
    {
      if (wm_[i].msg == m)
      {
        if (wm_[i].proc_method)   
          return wm_[i].proc_method(h, m, w, l, wm_[i].param);
      }
    }
    return DefWindowProc(h, m, w, l);
  }

  //WM_CREATEで行う処理
  //クラスメソッドにクラスポインタを登録している構造上、virtual化はできないので
  //別途init()で初期化を行ってください
  LRESULT OnCreate_default(HWND, UINT, WPARAM, LPARAM, void*)
  {
    return 0;
  }

  //WM_DESTROYの処理
  LRESULT OnDestroy_default(HWND, UINT, WPARAM, LPARAM, void*)
  {
	  //スレッドのフラグをオフ
	  runThread = false;
	  DWORD result;
	  while (true) {
		  //スレッドがあるかチェック
		  if (ci_ext::Window::hThread != NULL)
			  //スレッドが終わったかチェック
			  GetExitCodeThread(hThread, &result);
		  else
			  break;

		  //終わったらハンドルを閉じる。
		  if (result != STILL_ACTIVE) {
			  //CloseHandleで閉じる。 | hThreadが閉じている場合はプログラムが落ちる
			  CloseHandle(hThread);
			  //Set to be NULL (debug of GetExitCodeThread(no thread exist) at other objects)
			  ci_ext::Window::hThread = NULL;
			  //ループを抜ける。
			  break;
		  }
	  }

    PostQuitMessage(0);
    return 0;
  }

  //WM_KEYDOWの処理
  //ESCAPEキーで終了のみ記述
  LRESULT OnKeyDown_default(HWND h, UINT, WPARAM w, LPARAM, void*)
  {
    if(w == VK_ESCAPE) 
      DestroyWindow(h);
    return 0;
  }

  //WM_ACTIVEAPPの処理
  //ウインドウがアクティブ、非アクティブになったときに呼ばれる
  LRESULT OnActivateApp_default(HWND h, UINT, WPARAM w, LPARAM, void*)
  {
    //非アクティブからアクティブになった時の処理
    //例としてはPauseから復帰させるとか？
    if (w == TRUE)
      DOUT << "ウインドウがアクティブになりました" << std::endl;
    else
      DOUT << "ウインドウが非アクティブになりました" << std::endl;
    return 0;
  }

  //WM_MOUSEWHEELの処理
  //マウスホイールの処理
  LRESULT OnMouseWheel_default(HWND, UINT, WPARAM w, LPARAM, void*)
  {
    nowwheeldelta_ = GET_WHEEL_DELTA_WPARAM(w);
    return 0;
  }

  //ウインドウハンドルにthisポインタを登録する
  void SetUserPointer(HWND h)
  {
    SetWindowLong(h, GWL_USERDATA, (LONG)this);
  }

  //最初に登録するウインドウプロシージャ
  //CreateWindow()内でプロシージャを呼び、その時点ではWndProcで行っているポインタ取り出しができないため、
  //別途ポインタを登録し呼び出す処理を記述
  static LRESULT CALLBACK WndProcFirst(HWND h, UINT m, WPARAM w, LPARAM l)
  {
    Window* wnd = reinterpret_cast<Window*>(GetWindowLong(h, GWL_USERDATA));
    if (wnd == nullptr)
    {
      //ウィンドウの作成時の場合
      if (m == WM_CREATE)
      {
        //CreateWindowEx()最後の引数のポインタを取得する
        //ここではthisポインタを送っている
        wnd = reinterpret_cast<Window*>(((LPCREATESTRUCT)l)->lpCreateParams);
        //ウインドウハンドルにポインタを関連付けし、ウインドウプロシージャを再登録する
        wnd->SetUserPointer(h);
        SetWindowLong(h, GWL_WNDPROC, reinterpret_cast<LONG>(WndProc));
      }
      else
        return DefWindowProc(h, m, w, l);
    }
    return wnd->wndproc(h, m, w, l);
  }

  //Windowsに登録するプロシージャ。
  //ここでハンドルに登録されているポインタを取り出し、各ウインドウのプロシージャへ振り分ける
  static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l)
  {
    Window* wnd = reinterpret_cast<Window*>(GetWindowLong(h, GWL_USERDATA));
    return wnd->wndproc(h, m, w, l);
  }


};

}