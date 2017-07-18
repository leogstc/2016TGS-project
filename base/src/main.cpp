/*
スイ
・window_->insertMsgの処理をスルー（一部のオブジェクトのpause処理バグを解決）
・カーソル非表示可（main.cpp→WinMain→ShowCursor(bool)に設定）
・#ifdef _DEBUG
	ShowConsole();
	を挿入

*/


#include <iostream>
#include <Windows.h>

#include "lib/ci_ext/object.hpp"
#include "lib/ci_ext/Console.hpp"
#include "lib/ci_ext/xinput.hpp" 
#include "lib/gplib.h" 
#include <string>

#include "object/root.h"

namespace gplib{
  namespace system{
    const char USERNAME[256] = "MajoSuku！";

    const int WINW = 1280;
    const int WINH = 720;

    bool WindowMode = true;//フルスクリーン対応
    int  KeyboardMode = 0; //pad disable:1 pad enable:0
  }
}

int ci_ext::Object::uid_ = 0;
int ci_ext::Window::uid_ = 0;

HANDLE ci_ext::Window::hThread = NULL; //スレッドハンドル
bool ci_ext::Window::runThread = true; //スレッドを実行可能のフラグ

namespace game
{
//ゲームクラス
//このクラスは、ルートの管理のみ行い、ゲーム特有の処理はルートオブジェクトで行う
class Game
{
  std::shared_ptr<Root> root;
  std::shared_ptr<ci_ext::WindowDx> window_;
  LRESULT OnPause(HWND, UINT, WPARAM w, LPARAM, void* p)
  {
    auto game = static_cast<Game*>(p);//この場合はthisポインタでもOKですがサンプルなので
    //非アクティブからアクティブになった時の処理
    //Pause <=> Runの切り替え
    if (w == TRUE)
    {
      game->root->runAll();
      DOUT << "アクティブ" << std::endl;
    }
    else
    {
      game->root->pauseAll();
      DOUT << "非アクティブ" << std::endl;
    }
    return 0;
  }
public:
  int num;
  Game()
    :
    window_(new ci_ext::WindowDx(gplib::system::USERNAME, gplib::system::WINW, gplib::system::WINH))
  {
    //ゲームオブジェクトのルートを作成
    root = std::make_shared<Root>(window_);
    root->init(root);

    //ウインドウのアクティブ非アクティブのメッセージ処理を上書きする
    //window_->insertMsg(WM_ACTIVATEAPP, BindPtr(this, &Game::OnPause), this);

    //ゲームを駆動させる
    while (run());
  }

  //ゲームメイン処理
  bool run()
  {
    //Windowsからの要求を処理する
    auto ret = window_->doMsg2();

    //ルートオブジェクトから処理を行う
    if (ret == ci_ext::Window::MsgReturn::update)
    {
      window_->update_before();
      root->updateWithChildren();
      root->renderWithChildren();
	  root->renderLaterWithChildren();
      window_->update_after();
    }
    return ret == ci_ext::Window::MsgReturn::quit ? false : true;
  }

};

}

//エントリポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
  //アプリケーション終了時にメモリ情報をデバッグウインドウへ出力
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  //確認したい場合は、以下のコメントをとってデバッグ開始する
  //int* ptr = new int[100];

  //デバッグ終了後、VisualStudioの出力ウインドウに
  //  Detected memory leaks!
  //  Dumping objects ->
  //  {165} normal block at 0x00C70910, 400 bytes long.
  //  Data: <                > CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD
  //  Object dump complete.
  //とリークを検出してくれるのが確認できる

  //ローカル変数の寿命を明確にするためにスコープで区切る
  //ブロックを切ることで、明示的にメモリを解放することができる
  {
#ifdef _DEBUG
    ShowConsole();//コンソール表示（スコープ概念あり）
#endif
	//ShowCursor(false);
    {
      game::Game();//コンストラクタでアプリ駆動 Game()でもOK
    }
	//ShowCursor(true);
    DOUT << "終了します" << std::endl;
#ifdef _DEBUG
    system("pause");
#endif
  }
  return 0;
}
