#pragma once

#include <wrl.h>
#include <d3d11.h>

namespace ci_ext
{
//COMモデルのrelease()を自動化する
template <typename TYPE>
using DXPtr = Microsoft::WRL::ComPtr<TYPE>;

//スマートに使えるように
using IDevice = DXPtr<ID3D11Device>;
using IDeviceContext = DXPtr<ID3D11DeviceContext>;
using ISwapChain = DXPtr<IDXGISwapChain>;
using IBuffPtr = DXPtr<ID3D11Buffer>;
using ITexture2D = DXPtr<ID3D11Texture2D>;
using IVertexShader = DXPtr<ID3D11VertexShader>;
using IGeometryShader = DXPtr<ID3D11GeometryShader>;
using IPixelShader = DXPtr<ID3D11PixelShader>;
using IInputLayout = DXPtr<ID3D11InputLayout>;
using ISamplerState = DXPtr<ID3D11SamplerState>;
using IRasterizerState = DXPtr<ID3D11RasterizerState>;
using IBlendState = DXPtr<ID3D11BlendState>;
using IDepthStencilState = DXPtr<ID3D11DepthStencilState>;
using IShaderResourceView = DXPtr<ID3D11ShaderResourceView>;
using IRenderTargetView = DXPtr<ID3D11RenderTargetView>;
using IDepthStencilView = DXPtr<ID3D11DepthStencilView>;
using IBlob = DXPtr<ID3DBlob>;

}