#pragma once

#include "../../lib/ci_ext/color_dx11.hpp"
#include "../../lib/ci_ext/type_dx11.hpp"

namespace ci_ext
{

class DX11
{
private:

  //----------------------------------------------------------- 
  //DirectX11�֘A�ϐ�
  //
  // !!!Caution!!!
  //
  //  Release()���V�X�e���ɔC���Ă��邽�߁A�錾�̏��Ԃɒ���
  //  �@����=>�ォ�珇��
  //    �j��=>�����珇��
  //----------------------------------------------------------- 
  IDevice device_;
  IDeviceContext deviceContext_;
  ISwapChain swapChain_;
  IRenderTargetView renderTargetView_;
  IDepthStencilView depthStencilView_;

  D3D_DRIVER_TYPE driverType_ = D3D_DRIVER_TYPE_NULL;
  D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_0;
  ci_ext::Color color_;//�w�i�F

public:
  DX11(HWND windowhandle, int width, int height, const ci_ext::Color& color)
    :
    color_(color)//a,r,g,b�̏�
  {
    //DirectX11�f�o�C�X�ƃX���b�v�`�F�C���쐬
    {
      // �X���b�v�`�F�C���ݒ�
      DXGI_SWAP_CHAIN_DESC sd = {};
      sd.BufferCount = 1;
      sd.BufferDesc.Width = width;
      sd.BufferDesc.Height = height;
      sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      sd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
      sd.BufferDesc.RefreshRate.Numerator = 60;
      sd.BufferDesc.RefreshRate.Denominator = 1;  //1/60 = 60fps
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.OutputWindow = windowhandle;
      sd.SampleDesc.Count = 1;
      sd.SampleDesc.Quality = 0;
      sd.Windowed = TRUE;

      UINT cdev_flags = 0;

      // DirectX11&�n�[�h�E�F�A�Ή��̂�
      D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
      driverType_ = D3D_DRIVER_TYPE_HARDWARE;
      HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        driverType_,
        nullptr,
        cdev_flags,
        &feature_level,
        1,
        D3D11_SDK_VERSION,
        &sd,
        &swapChain_,
        &device_,
        &featureLevel_,
        &deviceContext_);
      if (FAILED(hr))
        throw hr;
    }

    {
      ITexture2D backBuffer;
      HRESULT hr = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
      if (FAILED(hr))
        throw hr;

      hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView_);
      if (FAILED(hr))
        throw hr;
    }

    // render-target view��o�^
    ID3D11RenderTargetView* rtv[1] = { renderTargetView_.Get() };
    deviceContext_->OMSetRenderTargets(1, rtv, nullptr);

    // viewport
    {
      D3D11_VIEWPORT vp = {};
      vp.Width = static_cast<FLOAT>(width);
      vp.Height = static_cast<FLOAT>(height);
      vp.MinDepth = 0.0f;
      vp.MaxDepth = 1.0f;
      vp.TopLeftX = 0;
      vp.TopLeftY = 0;
      deviceContext_->RSSetViewports(1, &vp);
    }
    /*


    // �萔�o�b�t�@ �ϊ��s��i�v���W�F�N�V�����A�r���[�j
    XMVECTOR eye = XMVectorSet(0.0f, 70.0f, -150.0f, 0.0f);
    XMVECTOR lookat = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    physics->init();

    // Bullet�f�o�b�O�`�揀��
    pDebugDrawer->init(pCtx->pDevice.Get(), pCtx->pDevCtx.Get());
    {
    FLOAT aspectRatio = static_cast<FLOAT>(width) / static_cast<FLOAT>(height);
    FLOAT nearZ = 0.01f, farZ = 1000.0f;
    auto projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(32.0f), aspectRatio, nearZ, farZ);
    auto view = XMMatrixLookAtLH(eye, lookat, up);
    pDebugDrawer->setMatrix(projection, view);
    }
    pDebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    physics->dynamicsWorld->setDebugDrawer(pDebugDrawer.get());

    physics->clock.reset();
    return S_OK;
    /*
    void Draw_InitImgTable();
    void Draw_InitFont();
    void InitPlayerInput();

    //�E�C���h�E�n���h����gplib�֓o�^���Ă��珉�����������s��
    gplib::system::hWnd = windowhandle();
    gplib::system::InitDx();
    Draw_InitImgTable();
    Draw_InitFont();
    gplib::math::RandomInit();
    gplib::camera::InitCamera(gplib::system::WINW / 2.0f, gplib::system::WINH / 2.0f, 0, 0);
    gplib::font::Draw_CreateFont(0, 20, "consolas");
    //input
    InitPlayerInput();
    for (int i = 0; i < gplib::system::PLAYER_NUM; i++)
    gplib::input::InitKeyInfo(i);
    */
  }

  void clear()
  {
    // �w��F�ŉ�ʃN���A
    deviceContext_->ClearRenderTargetView(renderTargetView_.Get(), color_);
  }
  void clear(ci_ext::Color& color)
  {
    // �w��F�ŉ�ʃN���A
    deviceContext_->ClearRenderTargetView(renderTargetView_.Get(), color);
  }

  void present()
  {
    //���ʂ��E�C���h�E�ɔ��f
    swapChain_->Present(0, 0);
  }
};



}