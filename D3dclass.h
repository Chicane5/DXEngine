#pragma once

#ifndef __D3DCLASS_H__
#define __D3DCLASS_H_

// LINKING

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")

//include the headers for those libraries we are linking to 

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dtypes.h>
#include <memory>


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();


	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	std::shared_ptr<ID3D11Device> GetDevice();
	std::shared_ptr<ID3D11DeviceContext> GetDeviceContext();

	void GetProjectionMatrix(DirectX::XMFLOAT4X4&);
	void GetWorldMatrix(DirectX::XMFLOAT4X4&);
	void GetOrthoMatrix(DirectX::XMFLOAT4X4&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	std::shared_ptr<IDXGISwapChain> m_swapChain;
	std::shared_ptr<ID3D11Device> m_device;
	std::shared_ptr<ID3D11DeviceContext> m_deviceContext;
	std::shared_ptr<ID3D11RenderTargetView> m_renderTargetView;
	std::shared_ptr<ID3D11Texture2D> m_depthStencilBuffer;
	std::shared_ptr<ID3D11DepthStencilState> m_depthStencilState;
	std::shared_ptr<ID3D11DepthStencilView> m_depthStencilView;
	std::shared_ptr<ID3D11RasterizerState> m_rasterState;
	DirectX::XMFLOAT4X4 m_projectionMatrix;
	DirectX::XMFLOAT4X4 m_worldMatrix;
	DirectX::XMFLOAT4X4 m_orthoMatrix;


};


#endif