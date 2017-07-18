/*
�X�C
�Ewindow_->insertMsg�̏������X���[�i�ꕔ�̃I�u�W�F�N�g��pause�����o�O�������j
�E�J�[�\����\���imain.cpp��WinMain��ShowCursor(bool)�ɐݒ�j
�E#ifdef _DEBUG
	ShowConsole();
	��}��

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
    const char USERNAME[256] = "MajoSuku�I";

    const int WINW = 1280;
    const int WINH = 720;

    bool WindowMode = true;//�t���X�N���[���Ή�
    int  KeyboardMode = 0; //pad disable:1 pad enable:0
  }
}

int ci_ext::Object::uid_ = 0;
int ci_ext::Window::uid_ = 0;

HANDLE ci_ext::Window::hThread = NULL; //�X���b�h�n���h��
bool ci_ext::Window::runThread = true; //�X���b�h�����s�\�̃t���O

namespace game
{
//�Q�[���N���X
//���̃N���X�́A���[�g�̊Ǘ��̂ݍs���A�Q�[�����L�̏����̓��[�g�I�u�W�F�N�g�ōs��
class Game
{
  std::shared_ptr<Root> root;
  std::shared_ptr<ci_ext::WindowDx> window_;
  LRESULT OnPause(HWND, UINT, WPARAM w, LPARAM, void* p)
  {
    auto game = static_cast<Game*>(p);//���̏ꍇ��this�|�C���^�ł�OK�ł����T���v���Ȃ̂�
    //��A�N�e�B�u����A�N�e�B�u�ɂȂ������̏���
    //Pause <=> Run�̐؂�ւ�
    if (w == TRUE)
    {
      game->root->runAll();
      DOUT << "�A�N�e�B�u" << std::endl;
    }
    else
    {
      game->root->pauseAll();
      DOUT << "��A�N�e�B�u" << std::endl;
    }
    return 0;
  }
public:
  int num;
  Game()
    :
    window_(new ci_ext::WindowDx(gplib::system::USERNAME, gplib::system::WINW, gplib::system::WINH))
  {
    //�Q�[���I�u�W�F�N�g�̃��[�g���쐬
    root = std::make_shared<Root>(window_);
    root->init(root);

    //�E�C���h�E�̃A�N�e�B�u��A�N�e�B�u�̃��b�Z�[�W�������㏑������
    //window_->insertMsg(WM_ACTIVATEAPP, BindPtr(this, &Game::OnPause), this);

    //�Q�[�����쓮������
    while (run());
  }

  //�Q�[�����C������
  bool run()
  {
    //Windows����̗v������������
    auto ret = window_->doMsg2();

    //���[�g�I�u�W�F�N�g���珈�����s��
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

//�G���g���|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
  //�A�v���P�[�V�����I�����Ƀ����������f�o�b�O�E�C���h�E�֏o��
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  //�m�F�������ꍇ�́A�ȉ��̃R�����g���Ƃ��ăf�o�b�O�J�n����
  //int* ptr = new int[100];

  //�f�o�b�O�I����AVisualStudio�̏o�̓E�C���h�E��
  //  Detected memory leaks!
  //  Dumping objects ->
  //  {165} normal block at 0x00C70910, 400 bytes long.
  //  Data: <                > CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD CD
  //  Object dump complete.
  //�ƃ��[�N�����o���Ă����̂��m�F�ł���

  //���[�J���ϐ��̎����𖾊m�ɂ��邽�߂ɃX�R�[�v�ŋ�؂�
  //�u���b�N��؂邱�ƂŁA�����I�Ƀ�������������邱�Ƃ��ł���
  {
#ifdef _DEBUG
    ShowConsole();//�R���\�[���\���i�X�R�[�v�T�O����j
#endif
	//ShowCursor(false);
    {
      game::Game();//�R���X�g���N�^�ŃA�v���쓮 Game()�ł�OK
    }
	//ShowCursor(true);
    DOUT << "�I�����܂�" << std::endl;
#ifdef _DEBUG
    system("pause");
#endif
  }
  return 0;
}
